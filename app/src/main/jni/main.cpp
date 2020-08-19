#include<jni.h>
#include <android/bitmap.h>
#include <android/log.h>
#include "opencv/cv.h"
#include "quadDetect.h"
#include "ImgEffects.h"
#include "lineDetect.h"
#include "com_oaup_ocr_imgprocess_image_Preprocess.h"


#ifndef eprintf
#define eprintf(...) __android_log_print(ANDROID_LOG_ERROR,"@",__VA_ARGS__)
#endif

#define RGBA_A(p) (((p) & 0xFF000000) >> 24)
#define RGBA_R(p) (((p) & 0x00FF0000) >> 16)
#define RGBA_G(p) (((p) & 0x0000FF00) >>  8)
#define RGBA_B(p)  ((p) & 0x000000FF)
#define MAKE_RGBA(r,g,b,a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

LineTool m_lineTool;

jobject matToBitmap(JNIEnv *env, cv::Mat &srcMat){
       eprintf("mat channels: %d\n", srcMat.channels());
       if(srcMat.channels() != 3 && srcMat.channels() != 1){
          eprintf("invalid mat channels\n");
          return NULL;
       }
//create bitmap
       jclass bitmapClass = (jclass)env->FindClass("android/graphics/Bitmap");
       jmethodID bitmapCreate = env->GetStaticMethodID(bitmapClass, "createBitmap",
                        "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
       jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
       jmethodID bmpClsValueOfMid = env->GetStaticMethodID(bitmapConfigClass, "valueOf",
                        "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
       /*jmethodID bitmapConfigNativeToConfig = env->GetStaticMethodID(bitmapConfigClass,
                        "nativeToConfig","(I)Landroid/graphics/Bitmap$Config;");

        jobject bitmapConfigObject = env->CallStaticObjectMethod(bitmapConfigClass,
                                     bitmapConfigNativeToConfig,5);*/
       jobject bitmapConfigObject = env->CallStaticObjectMethod(bitmapConfigClass, bmpClsValueOfMid,
                                    env->NewStringUTF("ARGB_8888"));

       if(!bitmapClass || !bitmapCreate || !bitmapConfigClass || !bitmapConfigObject)
            return 0;
       jobject bitmapObject = env->CallStaticObjectMethod(bitmapClass, bitmapCreate,
                                                          srcMat.cols, srcMat.rows, bitmapConfigObject);
       if(!bitmapObject)
        return 0;
        void * pixels = NULL;
         AndroidBitmapInfo bmpInfo;
         memset(&bmpInfo, 0, sizeof(bmpInfo));
         AndroidBitmap_getInfo(env, bitmapObject, &bmpInfo);
         // Check format, only RGB565 & RGBA are supported
          eprintf("bitmap format: %d\n",bmpInfo.format);
         if (bmpInfo.width <= 0 || bmpInfo.height <= 0 ||
             bmpInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888){
             eprintf("invalid bitmap\n");
             eprintf("bitmap format failed: %d\n",bmpInfo.format);
             return NULL;
         }
         int nRes = AndroidBitmap_lockPixels(env, bitmapObject, &pixels);
         if(pixels == NULL){
            eprintf("fail to lock bitmap: %d\n", nRes);
            return NULL;
         }
         int imageWidth = srcMat.cols;
         int imageHeight = srcMat.rows;
            for (int y = 0; y<imageHeight; y++) {
                uint32_t *bitmapLine = ((uint32_t *)pixels) + y * imageWidth;
                cv::Vec3b* pMatLine3 = srcMat.ptr<cv::Vec3b>(y);
                uchar* pMatLLine1 = srcMat.ptr<uchar>(y);
                for (int x = 0; x<imageWidth; x++) {
                    if(srcMat.channels()==3){
                        cv::Vec3b &curPixel = pMatLine3[x];
                        *(bitmapLine + x) = MAKE_RGBA(curPixel[0],curPixel[1],curPixel[2],255);
                    }else{
                        *(bitmapLine + x) = MAKE_RGBA(pMatLLine1[x],pMatLLine1[x],pMatLLine1[x],255);
                    }
                }
            }
       //end create bitmap
        AndroidBitmap_unlockPixels(env, bitmapObject);
       return bitmapObject;
}
int bitmapToMat(JNIEnv *env,jobject srcBitMap, cv::Mat& resultMat){
    // Lock the bitmap to get the buffer
    void * pixels = NULL;
    int imageWidth,imageHeight;
    AndroidBitmapInfo bmpInfo;
    memset(&bmpInfo, 0, sizeof(bmpInfo));
    AndroidBitmap_getInfo(env, srcBitMap, &bmpInfo);
    // Check format, only RGB565 & RGBA are supported
    if (bmpInfo.width <= 0 || bmpInfo.height <= 0 ||
       bmpInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        eprintf("invalid bitmap\n");
        return -1;
    }
    int nRes = AndroidBitmap_lockPixels(env, srcBitMap, &pixels);
    if(pixels == NULL){
       eprintf("fail to lock bitmap: %d\n", nRes);
       return -1;
    }
    imageWidth = bmpInfo.width;
    imageHeight = bmpInfo.height;
//    cv::Mat resultMat;
    resultMat.create(imageHeight, imageWidth, CV_8UC3);

    for (int y = 0; y<imageHeight; y++) {
        uint32_t *bitmapLine = ((uint32_t *)pixels) + y * imageWidth;
        cv::Vec3b* pMatLine3 = resultMat.ptr<cv::Vec3b>(y);
        for (int x = 0; x<imageWidth; x++) {
            uint32_t v = *(bitmapLine + x);
            //int aValue = RGBA_A(v);
            // eprintf("print lock A: %d\n", aValue);

            //if(aValue != 255)
               // eprintf("A: %d at (%d, %d) \n", aValue, x, y);

            cv::Vec3b &curPixel = pMatLine3[x];
            curPixel[2] = RGBA_B(v);
            curPixel[1] = RGBA_G(v);
            curPixel[0] = RGBA_R(v);
            //curPixel[3] = RGBA_A(v);
        }
    }
    // cv::imwrite("/mnt/sdcard/opencv/ConvertedRGB.txt", );
    AndroidBitmap_unlockPixels(env, srcBitMap);
    return 0;
}

/*
 * Class:     net_northking_ocr_image_Preprocess
 * Method:    detectCornersNative
 * Signature: (Landroid/graphics/Bitmap;Ljava/util/List;)I
 */
JNIEXPORT jint JNICALL Java_com_oaup_ocr_imgprocess_image_Preprocess_detectCornersNative
  (JNIEnv *env, jobject classObj, jobject srcBitMap, jobject resultPoints){
      int nRes=0;
      cv::Mat resultMat;
      nRes =  bitmapToMat(env,srcBitMap, resultMat);
      if(nRes != 0){
        return -1;
      }
      // "/mnt/sdcard/opencv/A02.jpg";
//      bool bSuccessed = cv::imwrite("/mnt/sdcard/opencv/Converted.jpg", resultMat);
       nkocr::QuadDetect detPt;
       vector<Point> resContour;
       nRes = detPt.detect(resultMat, resContour);
       if(nRes != 0){
         return -2;
       }
       //get class and method
       jclass listClass = env->FindClass("java/util/List");
       jmethodID listGet = env->GetMethodID(listClass,"get","(I)Ljava/lang/Object;");
       jmethodID listSize = env->GetMethodID(listClass,"size","()I");
       jmethodID listClear = env->GetMethodID(listClass,"clear","()V");
       jmethodID listAdd = env->GetMethodID(listClass,"add","(Ljava/lang/Object;)Z");
       jclass pointClass = env->FindClass("android/graphics/Point");
       jmethodID pointConstructor = env->GetMethodID(pointClass,"<init>","(II)V");
       if(listClass == 0 || listGet == 0 || listSize == 0 || listClear==0||
          listAdd==0 || pointClass==0 || pointConstructor==0)
    	return -3;
       //add point
       //clear list
       env->CallVoidMethod(resultPoints,listClear);
       for(int pointIndex=0;pointIndex<resContour.size();pointIndex++){
            cv::Point &curPoint = resContour[pointIndex];
            jobject pointObject = env->NewObject(pointClass,pointConstructor,
                                                            curPoint.x,curPoint.y);
            if(pointObject){
                    env->CallBooleanMethod(resultPoints,listAdd,pointObject);
            }
       }
       //end add point

       int nContourSize(0);
       nContourSize = resContour.size();
      for(int i = 0;i<nContourSize;i++){
          Point pt1 = resContour[i % nContourSize];
          Point pt2 = resContour[(i+1) % nContourSize];
          cv::line(resultMat, pt1, pt2, CV_RGB(255,0,0),2);
      }
//      bool bSuccessed = cv::imwrite("/mnt/sdcard/opencv/resultMat.jpg", resultMat);
//      if(!bSuccessed){
//         return -4;
//      }

    return 0;
  }

/*
 * Class:     net_northking_ocr_image_Preprocess
 * Method:    doPerspectiveNative
 * Signature: (Landroid/graphics/Bitmap;Ljava/util/List;)Landroid/graphics/Bitmap;
 */
JNIEXPORT jobject JNICALL Java_com_oaup_ocr_imgprocess_image_Preprocess_doPerspectiveNative
    (JNIEnv *env, jobject classObj, jobject srcBitmap, jobject srcListPoint){

       int nRes=0;
       cv::Mat srcMat, dstMat;
       nRes =  bitmapToMat(env,srcBitmap, srcMat);
       if(nRes != 0){
         eprintf("bitmap to mat failed");
         return 0;
       }
       //get class and method
       jclass listClass = env->FindClass("java/util/List");
       jmethodID listGet = env->GetMethodID(listClass,"get","(I)Ljava/lang/Object;");
       jmethodID listSize = env->GetMethodID(listClass,"size","()I");
       jmethodID listClear = env->GetMethodID(listClass,"clear","()V");
       jmethodID listAdd = env->GetMethodID(listClass,"add","(Ljava/lang/Object;)Z");
       jclass pointClass = env->FindClass("android/graphics/Point");
       jmethodID pointConstructor = env->GetMethodID(pointClass,"<init>","(II)V");
       jfieldID pointXFeild = env->GetFieldID(pointClass, "x", "I");
       jfieldID pointYFeild = env->GetFieldID(pointClass, "y", "I");

       if(listClass == 0 || listGet == 0 || listSize == 0 || listClear==0||
          listAdd==0 || pointClass==0 || pointConstructor==0)
           	return 0;
       int nListSize = env->CallIntMethod(srcListPoint,listSize);
       vector<Point> resContour;
       for(int pointIndex=0;pointIndex<nListSize;pointIndex++){
            jobject curAndroidPoint = env->CallObjectMethod(srcListPoint,listGet,pointIndex);
            if(curAndroidPoint){
                    cv::Point cvPoint;
                    cvPoint.x = env->GetIntField(curAndroidPoint,pointXFeild);
                    cvPoint.y = env->GetIntField(curAndroidPoint,pointYFeild);
                    resContour.push_back(cvPoint);
            }
       }
       //end add point
       nkocr::QuadDetect detectPerspective;
       nRes = detectPerspective.doPerspective(srcMat,resContour, dstMat);
       if(nRes != 0){
        eprintf("doPerspective failed:%d",nRes);
        return 0;
       }
      bool bSuccessed = cv::imwrite("/mnt/sdcard/opencv/PerspectiveBefore.jpg", dstMat);
//       if(nRes != 0){
//          return srcBitmap;
//       }
       return matToBitmap(env,dstMat);
    }

JNIEXPORT jobject JNICALL Java_com_oaup_ocr_imgprocess_image_Preprocess_cvt2GrayNative
  (JNIEnv *env, jobject classObj, jobject srcBitmap){
     cv::Mat srcMat, dstMat;
     int nRes = 0;
     nRes =  bitmapToMat(env,srcBitmap, srcMat);
     if(nRes != 0){
     eprintf("bitmap to mat failed");
     return 0;}
     nkocr::ImgEffects imgEffect;
     nRes = imgEffect.cvt2Gray(srcMat, dstMat);
     if(nRes != 0){
     eprintf("cvt2Gray failed\n");
     return 0;
     }
     eprintf("cvt2Gray successed\n");
     return  matToBitmap(env,dstMat);
  }

JNIEXPORT jobject JNICALL Java_com_oaup_ocr_imgprocess_image_Preprocess_cvt2BWNative
  (JNIEnv *env, jobject classObj, jobject srcBitmap){
     cv::Mat srcMat, dstMat;
     int nRes = bitmapToMat(env,srcBitmap, srcMat);
     if(nRes != 0){
     eprintf("bitmap to mat failed");
     return 0;}
     nkocr::ImgEffects imgEffect;
     nRes = imgEffect.cvt2BW(srcMat, dstMat);
     if(nRes != 0){
      eprintf("cvt to binary failed");
     return 0;}
      //get class and method
     return matToBitmap(env,dstMat);
  }

JNIEXPORT jobject JNICALL Java_com_oaup_ocr_imgprocess_image_Preprocess_enhanceNative
  (JNIEnv *env, jobject classObj, jobject srcBitmap){

   cv::Mat srcMat, dstMat;
   int nRes = bitmapToMat(env,srcBitmap, srcMat);
   if(nRes != 0){
   eprintf("bitmap to mat failed");
   return 0;
   }
   nkocr::ImgEffects imgEffect;
   nRes = imgEffect.enhance(srcMat, dstMat);
   if(nRes != 0){
   eprintf("enhance failed");
   return 0;
   }
    //get class and method
   return matToBitmap(env,dstMat);
  }

JNIEXPORT void JNICALL Java_com_oaup_ocr_imgprocess_image_Preprocess_setDetectLineParamNative
  (JNIEnv *env, jobject, jint, jint, jint, jint, jint, jint, jint){

  }

JNIEXPORT jint JNICALL Java_com_oaup_ocr_imgprocess_image_Preprocess_detectFrameNative
  (JNIEnv *env, jobject classObj, jobject srcBitmap){
      cv::Mat srcMat;
      int nRes = bitmapToMat(env, srcBitmap, srcMat);
      if(nRes != 0){
         eprintf("bitmap to mat failed");
         return 0;
      }
      return m_lineTool.detectFrame(srcMat);
  }

  JNIEXPORT jdouble JNICALL Java_com_oaup_ocr_imgprocess_image_Preprocess_detectLinesNative
    (JNIEnv * env, jobject classObj, jobject srcBitmap, jobject dstHorLines, jobject dstVerLines){
       cv::Mat srcMat;
          int nRes = bitmapToMat(env, srcBitmap, srcMat);
          if(nRes != 0){
             eprintf("bitmap to mat failed");
             return -1;
          }
          vector<FORMLINE> horLines;
          horLines.clear();
          vector<FORMLINE> verLines;
          verLines.clear();
          double angle = 0.0;
         nRes = m_lineTool.detectLines(srcMat,horLines,verLines,angle);
         if(nRes != 0){
           return -2;
         }
           //get class and method
           jclass listClass = env->FindClass("java/util/List");
           jmethodID listGet = env->GetMethodID(listClass,"get","(I)Ljava/lang/Object;");
           jmethodID listSize = env->GetMethodID(listClass,"size","()I");
           jmethodID listClear = env->GetMethodID(listClass,"clear","()V");
           jmethodID listAdd = env->GetMethodID(listClass,"add","(Ljava/lang/Object;)Z");
           jclass pointClass = env->FindClass("android/graphics/Point");
           jmethodID pointConstructor = env->GetMethodID(pointClass,"<init>","(II)V");

           jclass formLineClass = env->FindClass("net/northking/ocr/imgprocess/image/docFormLineInfo");
           jmethodID formLineConstructor = env->GetMethodID(formLineClass,"<init>","()V");

           if(listClass == 0 || listGet == 0 || listSize == 0 || listClear==0||
              listAdd==0 || formLineClass==0 || formLineConstructor==0 || pointClass ==0
              || pointConstructor == 0)
        	return -3;
           //add point
           //clear list
           env->CallVoidMethod(dstHorLines,listClear);
           for(int lineIndex=0;lineIndex<horLines.size();lineIndex++){
                 FORMLINE &curFormLine = horLines[lineIndex];
                 DSCC_CPoint StPnt = curFormLine.StPnt;//Start point
                 DSCC_CPoint EdPnt = curFormLine.EdPnt;//End point
                 jobject stPointObject = env->NewObject(pointClass,pointConstructor,
                                                                             StPnt.x,StPnt.y);
                 jobject edPointObject = env->NewObject(pointClass,pointConstructor,
                                                                             EdPnt.x,EdPnt.y);
                 jobject docFormLineObject = env->NewObject(formLineClass,formLineConstructor,
                                                                 curFormLine.nIndex,
                                                                 curFormLine.nStyle,
                                                                 stPointObject,edPointObject,
                                                                 curFormLine.Angle,
                                                                 curFormLine.Width,
                                                                 curFormLine.Q,curFormLine.bSlant,
                                                                 curFormLine.nUseType);
                 if(docFormLineObject){
                         env->CallBooleanMethod(dstHorLines,listAdd,docFormLineObject);
                 }
            }

           env->CallVoidMethod(dstVerLines,listClear);
           for(int lineIndex=0;lineIndex<verLines.size();lineIndex++){
                 FORMLINE &curFormLine = verLines[lineIndex];
                 DSCC_CPoint StPnt = curFormLine.StPnt;//Start point
                 DSCC_CPoint EdPnt = curFormLine.EdPnt;//End point
                 jobject stPointObject = env->NewObject(pointClass,pointConstructor,
                                                                             StPnt.x,StPnt.y);
                 jobject edPointObject = env->NewObject(pointClass,pointConstructor,
                                                                             EdPnt.x,EdPnt.y);
                 jobject docFormLineObject = env->NewObject(formLineClass,formLineConstructor,
                                                                 curFormLine.nIndex,
                                                                 curFormLine.nStyle,
                                                                 stPointObject,edPointObject,
                                                                 curFormLine.Angle,
                                                                 curFormLine.Width,
                                                                 curFormLine.Q,curFormLine.bSlant,
                                                                 curFormLine.nUseType);
                 if(docFormLineObject){
                         env->CallBooleanMethod(dstVerLines,listAdd,docFormLineObject);
                 }
           }

       //end add point

        return angle;
    }