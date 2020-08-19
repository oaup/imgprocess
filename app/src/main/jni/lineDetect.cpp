// lineDetect.cpp : Defines the entry point for the console application.
//


#include "lineDetect.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core_c.h"
using namespace cv;
LineTool::LineTool() {
	m_param.ValleyGrayDepth = 30;
	//Merge Line para_thresh
	m_param.MinVerLineLength = 20;
	m_param.MinHorLineLength = 30;
	m_param.MaxLineWidth = 15;
	m_param.FilterSmallDSCC = TRUE;
	m_param.RLS = FALSE;
	m_param.bHasSlantLine = FALSE;
	m_param.MaxGap = 15;
	m_bParamsSet = TRUE;
	//Lines min Length
	m_param.m_minHorLenth = 300;
	m_param.m_minVerLenth = 200;
	m_param.m_minRectPointDistance = 20;
	//MergeFrameLine para_thresh
	m_param.m_angleThresh1 = 2;
	m_param.m_angleThresh2 = 10;
	m_param.m_nMidLinesThresh = 2;
	m_param.m_midlineDiffThresh = 10;
}
NkLinePara LineTool::getDefaultPara(){
	return m_param;
}
void LineTool::setParam(const NkLinePara &para) {
	m_param = para;
	m_bParamsSet = TRUE;
}
//define sort function for HorLines point.y 
bool SortByLine_y(const FORMLINE &A, const FORMLINE &B){
	return A.StPnt.y < B.StPnt.y;
}

int LineTool::distance(int x1, int y1, int x2, int y2){
	return sqrt((double)((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)));
}

// int  LineTool::convertPixToCImage(Pix *oriPix, CImage &resultCImage){
// 	if (!oriPix){
// 		return -1;
// 	}
// 	int finalRet = 0;
// 	int pixelDepth = oriPix->d;
// 	int imageWidth = oriPix->w;
// 	int imageHeight = oriPix->h;
// 	BOOL initRet = FALSE;
// 	if (pixelDepth == 32)
// 		initRet = resultCImage.Initialize(imageWidth, imageHeight, 24, 0);
// 	else
// 		initRet = resultCImage.Initialize(imageWidth, imageHeight, pixelDepth, 0);
// 	if (!initRet)
// 		return -1;
// 	LPBYTE cimageData = resultCImage.LockRawImg();
// 	int cimageLinebytes = resultCImage.GetLineSizeInBytes();
// 	switch (pixelDepth){
// 	case 1:{
// 		for (int y = 0; y<imageHeight; y++){
// 			l_uint32 *pixLineData = oriPix->data + y * oriPix->wpl;
// 			LPBYTE cimageLineData = cimageData + (imageHeight - 1 - y) * cimageLinebytes;
// 			for (int x = 0; x<imageWidth; x++){
// 				l_int32 bitValue = GET_DATA_BIT(pixLineData, x);
// 				int byteLoc = x / 8;
// 				int bitLoc = 7 - x % 8;
// 				LPBYTE byteAddress = cimageLineData + byteLoc;
// 				int operBase = 1 << bitLoc;
// 				if (bitValue == 1){//set data
// 					*byteAddress = *byteAddress | operBase;//(int)pow((float)2,(int)bitLoc);
// 				}
// 				else{//clear data
// 					*byteAddress = *byteAddress & ~operBase;//~((int)pow((float)2,(int)bitLoc));
// 				}
// 			}
// 		}
// 	}
// 		break;
// 	case 8:{
// 		for (int y = 0; y<imageHeight; y++){
// 			l_uint32 *pixLineData = oriPix->data + y * oriPix->wpl;
// 			LPBYTE cimageLineData = cimageData + (imageHeight - 1 - y) * cimageLinebytes;
// 			for (int x = 0; x<imageWidth; x++){
// 				l_int32 byteValue = GET_DATA_BYTE(pixLineData, x);
// 				*(cimageLineData + x) = byteValue;
// 			}
// 		}
// 	}
// 		break;
// 	case 24:
// 	case 32:{
// 		int byteStep = pixelDepth / 8;
// 		for (int y = 0; y<imageHeight; y++){
// 			l_uint32 *pixLineData = oriPix->data + y * oriPix->wpl;
// 			LPBYTE   cimageLineData = cimageData + (imageHeight - 1 - y) * cimageLinebytes;
// 			for (int x = 0; x<imageWidth; x++){
// 				l_int32 rValue = GET_DATA_BYTE(pixLineData, x*byteStep + 0);
// 				l_int32 gValue = GET_DATA_BYTE(pixLineData, x*byteStep + 1);
// 				l_int32 bValue = GET_DATA_BYTE(pixLineData, x*byteStep + 2);
// 				//l_int32 alphaValue = GET_DATA_BYTE(pixLineData, x*byteStep+3);
// 				*(cimageLineData + x * 3) = bValue;
// 				*(cimageLineData + x * 3 + 1) = gValue;
// 				*(cimageLineData + x * 3 + 2) = rValue;
// 				//*(cimageData + x*byteStep+3) = alphaValue;
// 			}
// 		}
// 	}
// 		break;
// 	default:
// 		finalRet = -1;
// 	}
// 	resultCImage.UnlockRawImg();
// 	return finalRet;
// }

// Pix* LineTool::convertCImageToPix(CImage &oriCImage){
// 	int imageType = oriCImage.GetType();
// 	int pixelDepth = 0;
// 	switch (imageType){
// 	case DIB_1BIT:
// 		pixelDepth = 1;
// 		break;
// 	case DIB_8BIT:
// 		pixelDepth = 8;
// 		break;
// 	case DIB_24BIT:
// 		pixelDepth = 24;
// 		break;
// 	case DIB_32BIT:
// 		pixelDepth = 32;
// 		break;
// 	default:
// 		pixelDepth = 0;
// 	}
// 	if (pixelDepth == 0)
// 		return NULL;
// 
// 	int imageWidth = oriCImage.GetWidth();
// 	int imageHeight = oriCImage.GetHeight();
// 	LPBYTE cimageData = oriCImage.LockRawImg();
// 	int cimageLinebytes = oriCImage.GetLineSizeInBytes();
// 	Pix *resultPix = NULL;
// 	if (pixelDepth == 24)
// 		resultPix = pixCreate(imageWidth, imageHeight, 32);
// 	else
// 		resultPix = pixCreate(imageWidth, imageHeight, pixelDepth);
// 	if (resultPix == NULL){
// 		oriCImage.UnlockRawImg();
// 		return resultPix;
// 	}
// 	int pixLineWords = resultPix->wpl;
// 	switch (pixelDepth){
// 	case 1:{
// 		for (int y = 0; y < imageHeight; y++){
// 			l_uint32 *pixLineData = resultPix->data + y * pixLineWords;
// 			LPBYTE cimageLineData = cimageData + (imageHeight - 1 - y) * cimageLinebytes;
// 			for (int x = 0; x < imageWidth; x++){
// 				int byteLoc = x / 8;
// 				int bitLoc = 7 - x % 8;
// 				LPBYTE byteAddress = cimageLineData + byteLoc;
// 				int operBase = 1 << bitLoc;
// 				//int bitValue = (*byteAddress & (int)pow((float)2,(int)bitLoc)) >> bitLoc;
// 				int bitValue = (*byteAddress & operBase) >> bitLoc;
// 				if (bitValue == 1)
// 					SET_DATA_BIT(pixLineData, x);
// 				else
// 					CLEAR_DATA_BIT(pixLineData, x);
// 			}
// 		}
// 	}
// 		break;
// 	case 8:{
// 		for (int y = 0; y < imageHeight; y++){
// 			l_uint32 *pixLineData = resultPix->data + y * pixLineWords;
// 			LPBYTE cimageLineData = cimageData + (imageHeight - 1 - y) * cimageLinebytes;
// 			for (int x = 0; x < imageWidth; x++){
// 				int byteValue = *(cimageLineData + x);
// 				SET_DATA_BYTE(pixLineData, x, byteValue);
// 			}
// 		}
// 	}
// 		break;
// 	case 24:
// 	case 32:{
// 		for (int y = 0; y < imageHeight; y++){
// 			l_uint32 *pixLineData = resultPix->data + y * pixLineWords;
// 			LPBYTE cimageLineData = cimageData + (imageHeight - 1 - y) * cimageLinebytes;
// 			for (int x = 0; x < imageWidth; x++){
// 				int bValue = 0, gValue = 0, rValue = 0, alphaValue = 0;
// 				if (pixelDepth == 24){
// 					bValue = *(cimageLineData + x * 3);
// 					gValue = *(cimageLineData + x * 3 + 1);
// 					rValue = *(cimageLineData + x * 3 + 2);
// 				}
// 				else{//32 bit
// 					bValue = *(cimageLineData + x * 4);
// 					gValue = *(cimageLineData + x * 4 + 1);
// 					rValue = *(cimageLineData + x * 4 + 2);
// 					alphaValue = *(cimageLineData + x * 4 + 3);
// 				}
// 				//pix store 32 bit
// 				SET_DATA_BYTE(pixLineData, x * 4, rValue);
// 				SET_DATA_BYTE(pixLineData, x * 4 + 1, gValue);
// 				SET_DATA_BYTE(pixLineData, x * 4 + 2, bValue);
// 				SET_DATA_BYTE(pixLineData, x * 4 + 3, alphaValue);
// 			}
// 		}
// 	}
// 		break;
// 	default:
// 		pixDestroy(&resultPix);
// 		resultPix = NULL;
// 	}
// 	oriCImage.UnlockRawImg();
// 	return resultPix;
// }

// Pix* LineTool::convertMatToPix(cv::Mat &oriMat, bool isBinary){
// 
// 	// accept only char type matrices
// 	/*if(oriMat.depth()!=sizeof(uchar)){
// 	return NULL;
// 	}*/
// 	int channels = oriMat.channels();
// 	int pixelDepth = 0;
// 	switch (channels){
// 	case 1:{
// 		if (!isBinary)
// 			pixelDepth = 8;
// 		else
// 			pixelDepth = 1;
// 		}
// 		break;
// 	case 3:
// 		pixelDepth = 32;
// 		break;
// 	default:
// 		pixelDepth = 0;
// 		break;
// 	}
// 	if (pixelDepth == 0)
// 		return NULL;
// 	int imageWidth = oriMat.cols;
// 	int imageHeight = oriMat.rows;
// 	Pix *resultPix = pixCreate(imageWidth, imageHeight, pixelDepth);
// 	if (!resultPix)
// 		return resultPix;
// 	int pixLineWords = resultPix->wpl;
// 	switch (pixelDepth){
// 	case 1:{
// 		for (int y = 0; y < imageHeight; y++){
// 			l_uint32 *pixLineData = resultPix->data + y * pixLineWords;
// 			uchar* pMatLine = oriMat.ptr<uchar>(y);
// 			for (int x = 0; x < imageWidth; x++){
// 				if (pMatLine[x] == 0)
// 					SET_DATA_BIT(pixLineData, x);
// 				else
// 					CLEAR_DATA_BIT(pixLineData, x);
// 			}
// 		}
// 	}
// 		break;
// 	case 8:{
// 		for (int y = 0; y < imageHeight; y++){
// 			l_uint32 *pixLineData = resultPix->data + y * pixLineWords;
// 			//LPBYTE cimageLineData = cimageData + (imageHeight-1-y) * cimageLinebytes;
// 			uchar* pMatLine = oriMat.ptr<uchar>(y);
// 			for (int x = 0; x < imageWidth; x++){
// 				int byteValue = pMatLine[x];//*(cimageLineData + x);
// 				SET_DATA_BYTE(pixLineData, x, byteValue);
// 			}
// 		}
// 	}
// 		break;
// 	case 32:{
// 		for (int y = 0; y < imageHeight; y++){
// 			l_uint32 *pixLineData = resultPix->data + y * pixLineWords;
// 			cv::Vec3b* pMatLine3 = oriMat.ptr<cv::Vec3b>(y);
// 			for (int x = 0; x < imageWidth; x++){
// 				cv::Vec3b &curPixel = pMatLine3[x];
// 				SET_DATA_BYTE(pixLineData, x * 4, curPixel[2]);
// 				SET_DATA_BYTE(pixLineData, x * 4 + 1, curPixel[1]);
// 				SET_DATA_BYTE(pixLineData, x * 4 + 2, curPixel[0]);
// 			}
// 		}
// 	}
// 		break;
// 	default:
// 		pixDestroy(&resultPix);
// 		resultPix = NULL;
// 	}
// 	return resultPix;
// }

// int LineTool::convertPixToMat(Pix *oriPix, cv::Mat &resultMat){
// 	if (!oriPix){
// 		return -1;
// 	}
// 	int finalRet = 0;
// 	int pixelDepth = oriPix->d;
// 	int imageWidth = oriPix->w;
// 	int imageHeight = oriPix->h;
// 	switch (pixelDepth){
// 	case 1:
// 	case 8:{
// 		resultMat.create(imageHeight, imageWidth, CV_8UC1);
// 		for (int y = 0; y<imageHeight; y++){
// 			l_uint32 *pixLineData = oriPix->data + y * oriPix->wpl;
// 			uchar* pMatLine = resultMat.ptr<uchar>(y);
// 			for (int x = 0; x<imageWidth; x++){
// 				l_int32 curValue = 0;
// 				if (pixelDepth == 8){
// 					curValue = GET_DATA_BYTE(pixLineData, x);
// 					pMatLine[x] = curValue;
// 				}
// 				else{
// 					curValue = GET_DATA_BIT(pixLineData, x);
// 					if (!curValue)
// 						pMatLine[x] = 255;
// 					else
// 						pMatLine[x] = 0;
// 				}
// 			}
// 		}
// 	}
// 		break;
// 	case 24:
// 	case 32:{
// 		resultMat.create(imageHeight, imageWidth, CV_8UC3);
// 		int byteStep = pixelDepth / 8;
// 		for (int y = 0; y<imageHeight; y++){
// 			l_uint32 *pixLineData = oriPix->data + y * oriPix->wpl;
// 			cv::Vec3b* pMatLine3 = resultMat.ptr<cv::Vec3b>(y);
// 			for (int x = 0; x<imageWidth; x++){
// 				l_int32 rValue = GET_DATA_BYTE(pixLineData, x*byteStep + 0);
// 				l_int32 gValue = GET_DATA_BYTE(pixLineData, x*byteStep + 1);
// 				l_int32 bValue = GET_DATA_BYTE(pixLineData, x*byteStep + 2);
// 				//l_int32 alphaValue = GET_DATA_BYTE(pixLineData, x*byteStep+3);
// 				cv::Vec3b &curPixel = pMatLine3[x];
// 				curPixel[0] = bValue;
// 				curPixel[1] = gValue;
// 				curPixel[2] = rValue;
// 				//*(cimageData + x*byteStep+3) = alphaValue;
// 			}
// 		}
// 	}
// 		break;
// 	default:
// 		finalRet = -1;
// 	}
// 	return finalRet;
// }

int LineTool::imageRorate(const cv::Mat &image_original, cv::Mat &image_Rorate, double skewAngle){
	skewAngle = -skewAngle / 3.1415 * 180;;
	cv::Mat rotateMat;
	cv::Point2f center = cv::Point2f(image_original.cols / 2, image_original.rows / 2);
	rotateMat = cv::getRotationMatrix2D(center, skewAngle, 1);
	cv::warpAffine(image_original, image_Rorate, rotateMat, image_original.size());
	return 0;
}

// int  LineTool::pointRotate(double angle, float centerX, float centerY,const std::vector<cv::Point> &srcPoints, std::vector<cv::Point> &dstPoints){
// 	dstPoints.clear();
// 	if (srcPoints.size() < 1)
// 		return 0;
// 	PTA *tempPoint = ptaCreate(srcPoints.size());
// 	if (!tempPoint)
// 		return -1;
// 	for (int pointIndex = 0; pointIndex < srcPoints.size(); pointIndex++){
// 		const cv::Point &curPoint = srcPoints[pointIndex];
// 		ptaAddPt(tempPoint, curPoint.x, curPoint.y);
// 	}
// 	PTA *pointResult = ptaRotate(tempPoint, centerX, centerY, angle);
// 	if (!pointResult){
// 		ptaDestroy(&tempPoint);
// 		return -1;
// 	}
// 	else{
// 		for (int pointIndex = 0; pointIndex < srcPoints.size(); pointIndex++){
// 			float tempX, tempY;
// 			ptaGetPt(pointResult, pointIndex, &tempX, &tempY);
// 			dstPoints.push_back(cv::Point(tempX, tempY));
// 		}
// 	}
// 	ptaDestroy(&tempPoint);
// 	ptaDestroy(&pointResult);
// 	return 0;
// }

int  LineTool::pointRotate(double angle, float centerX, float centerY, const std::vector<cv::Point> &srcPoints, std::vector<cv::Point> &dstPoints) {
	int sum = srcPoints.size();
	if (sum > 0)
	{
		angle = -angle;
		int tempX = 0, tempY = 0;
		cv::Point tempDst;
		for (int i = 0; i < sum; i++) {
			tempX = srcPoints[i].x - centerX;
			tempY = srcPoints[i].y - centerY;
			tempDst.x = cvRound(tempX*cos(angle) + tempY*sin(angle) + centerX);
			tempDst.y = cvRound(-tempX*sin(angle) + tempY*cos(angle) + centerY);
			dstPoints.push_back(tempDst);
		}
		return 0;
	} else {
		cout << "Point rotate failed!" << endl;
		return -1;
	}
	
// 	int x =  - centerX;
// 	int y = src.y - centerY;
// 
// 	dst.x = cvRound(x * cos(angle) + y * sin(angle) + center.x);
// 	dst.y = cvRound(-x * sin(angle) + y * cos(angle) + center.y);
}

double LineTool::getAngle(const cv::Point &lineStart, const cv::Point &lineEnd){
	if (lineEnd.x != lineStart.x) {
		double atg = atan(((double)(lineStart.y - lineEnd.y)) / ((double)(lineEnd.x - lineStart.x)));
		if (lineEnd.x > lineStart.x)
			return atg;
		else{
			if (lineEnd.y < lineStart.y)
				return atg + PI;
			else
				return atg - PI;
		}
	}else{
		if (lineEnd.y < lineStart.y)
			return PI / 2;
		else if (lineEnd.y > lineStart.y)
			return -PI / 2;
		else
			return 0;
	}
}

int LineTool::detectOuterRects(const cv::Mat &oriMat, double &angle, std::vector<cv::Rect> &rects) {
	//¶þÖµ»¯Í¼Ïñ
// 	cv::Mat grayMat, thresholdMat;
// 	if (oriMat.channels() == 1){
// 		oriMat.copyTo(grayMat);
// 	}else{
// 		cvtColor(oriMat, grayMat, CV_BGR2GRAY);
// 	}
// 	int threshold_value = std::min(oriMat.cols, oriMat.rows);
// 	if ((threshold_value % 2) == 0){
// 		threshold_value++;
// 	}
// 
// 	adaptiveThreshold(grayMat, thresholdMat, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, threshold_value, 5);

	int Detectvalue = detectFrame(oriMat);
	if (Detectvalue != 0){
		std::cout << "DeteceFrame process error!" << std::endl;
		return -1;
	}
	std::vector<FORMLINE> horLines, verLines;
	int getLineValue = getLines(horLines, verLines, angle);
	if (getLineValue != 0){
		std::cout << "getLines process error!" << std::endl;
		return -2;
	}
	int getrectsValue = getOuterRects(horLines, verLines, rects);
	if (getrectsValue != 0){
		std::cout << "getOuterRects process error!" << std::endl;
		return -3;
	}
	return 0;
}

int LineTool::detectLines(const cv::Mat &oriMat,  std::vector<FORMLINE> &horLines,
	std::vector<FORMLINE> &verLines, double &angle){
	CImage cImg;
	CImage		filteredImg;
	std::vector<FORMLINE> horLines_first, verLines_first;

	// for debugging
	time_t starttime, stoptime;
	float usedtime;
	int retVal = convertMatToCImage(oriMat, cImg, true);
	if (retVal != 0){
		std::cout << "Error in convert Mat to CImage!" << std::endl;
		return -1;
	}
	//horizontal chains detection
	int dsccHorvalue = m_horLine.DSCCFiltering(cImg, filteredImg, TRUE);
	if (dsccHorvalue != 0){
		std::cout << " HorLine DSCCFiltering process error!" << std::endl;
		return -1;
	}
	int mergeHorvalue = m_horLine.MergeChains();
	if (mergeHorvalue != 0){
		std::cout << "HorLine MergeChians process error!" << std::endl;
		return -1;
	}
	//horizontal chains to lines
	for (int i = 0; i<m_horLine.m_nChains; i++){
		CHAINS &Chains = m_horLine.m_pChains[i];
		FORMLINE line;
		int chainsToLine_Horvalue = m_horLine.ChainsToFORMLINE(line, Chains);
		if (chainsToLine_Horvalue != 0){
			std::cout << "HorLine ChainsToFORMLINE process error!" << std::endl;
			return -1;
		}
		if (((line.EdPnt.x - line.StPnt.x) > m_param.MinHorLineLength)){
			horLines_first.push_back(line);
		}
	}
	//vertical chains detection
	int dsccVervalue = m_verLine.DSCCFiltering(cImg, filteredImg, FALSE);
	if (dsccVervalue != 0){
		std::cout << "VerLine DSCCFiltering process error!" << std::endl;
		return -1;
	}
	int mergeVervalue = m_verLine.MergeChains();
	if (mergeVervalue != 0){
		std::cout << "VerLine MergeChains process error!" << std::endl;
		return -1;
	}
	//vertical chains to lines
	for (int j = 0; j<m_verLine.m_nChains; j++){
		CHAINS &Chains = m_verLine.m_pChains[j];
		FORMLINE line;
		int chainsToLine_Vervalue = m_verLine.ChainsToFORMLINE(line, Chains);
		if (chainsToLine_Vervalue != 0){
			std::cout << "VerLine ChainsToFORMLINE process error!" << std::endl;
			return -1;
		}
		if (((line.EdPnt.y - line.StPnt.y) > m_param.MinVerLineLength)){
			verLines_first.push_back(line);
		}
	}

	//compute angle
	double singleAngle = 0,sumAngle = 0;
	for (int horIndex = 0; horIndex < horLines_first.size();horIndex++){
		FORMLINE & curHorLine = horLines_first[horIndex];
		cv::Point pt1, pt2;
		pt1.x = curHorLine.StPnt.x;
		pt1.y = curHorLine.StPnt.y;
		pt2.x = curHorLine.EdPnt.x;
		pt2.y = curHorLine.EdPnt.y;

		singleAngle = getAngle(pt1,pt2);
		sumAngle = singleAngle + sumAngle;
	}
	if (horLines_first.size()>0){
		angle = sumAngle /( 2 * horLines_first.size());
	}else{
		angle = 0;
	}
	
	//line to point
	std::vector<cv::Point> horLines_srcStart, horLines_srcEnd,verLines_srcStart,verLines_srcEnd;
	for (int horIndex = 0; horIndex < horLines_first.size(); horIndex++){
		FORMLINE & curHorLine = horLines_first[horIndex];
		cv::Point curLineStart, curLineEnd;
		curLineStart.x = curHorLine.StPnt.x;
		curLineStart.y = curHorLine.StPnt.y;
		curLineEnd.x = curHorLine.EdPnt.x;
		curLineEnd.y = curHorLine.EdPnt.y;
		horLines_srcStart.push_back(curLineStart);
		horLines_srcEnd.push_back(curLineEnd);
	}
	for (int verIndex = 0; verIndex < verLines_first.size(); verIndex++){
		FORMLINE & curVerLine = verLines_first[verIndex];
		cv::Point curLineStart, curLineEnd;
		curLineStart.x = curVerLine.StPnt.x;
		curLineStart.y = curVerLine.StPnt.y;
		curLineEnd.x = curVerLine.EdPnt.x;
		curLineEnd.y = curVerLine.EdPnt.y;
		verLines_srcStart.push_back(curLineStart);
		verLines_srcEnd.push_back(curLineEnd);
	}
	//rorate line
	std::vector<cv::Point> horLines_dstStart, horLines_dstEnd, verLines_dstStart, verLines_dstEnd;
	int rotate_value1 = pointRotate(angle, oriMat.cols/2, oriMat.rows/2, horLines_srcStart, horLines_dstStart);
	if (rotate_value1 != 0){
		std::cout << "First pointRotate process error!" << std::endl;
		return -1;
	}
	int rotate_value2 = pointRotate(angle, oriMat.cols / 2, oriMat.rows / 2, horLines_srcEnd, horLines_dstEnd);
	if (rotate_value2 != 0){
		std::cout << "Second pointRotate process error!" << std::endl;
		return -1;
	}
	int rotate_value3 = pointRotate(angle, oriMat.cols / 2, oriMat.rows / 2, verLines_srcStart, verLines_dstStart);
	if (rotate_value3 != 0){
		std::cout << "Third pointRotate process error!" << std::endl;
		return -1;
	}
	int rotate_value4 = pointRotate(angle, oriMat.cols / 2, oriMat.rows / 2, verLines_srcEnd, verLines_dstEnd);
	if (rotate_value4 != 0){
		std::cout << "Fourth pointRotate process error!" << std::endl;
		return -1;
	}
	// point to form line	 
	for(int pointIndex = 0; pointIndex < horLines_dstEnd.size();pointIndex++){
		cv::Point & curhorpointStart = horLines_dstStart[pointIndex];
		cv::Point & curhorpointEnd = horLines_dstEnd[pointIndex];
		FORMLINE singlehorline ;
		singlehorline.StPnt.x = curhorpointStart.x;
		singlehorline.StPnt.y = curhorpointStart.y;
		singlehorline.EdPnt.x = curhorpointEnd.x;
		singlehorline.EdPnt.y = curhorpointEnd.y;
		horLines.push_back(singlehorline);
		
	}
	for (int pointIndex = 0; pointIndex < verLines_dstEnd.size(); pointIndex++){
		cv::Point & curverpointStart = verLines_dstStart[pointIndex];
		cv::Point & curverpointEnd = verLines_dstEnd[pointIndex];
		FORMLINE singleverline;
		singleverline.StPnt.x = curverpointStart.x;
		singleverline.StPnt.y = curverpointStart.y;
		singleverline.EdPnt.x = curverpointEnd.x;
		singleverline.EdPnt.y = curverpointEnd.y;
		verLines.push_back(singleverline);
	}	
	return 0;
}


int LineTool::convertMatToCImage(const cv::Mat &oriMat, CImage &resultCImage, bool isBinary){
	int finalRet = 0;
	int nChannels = oriMat.channels();
	int cImagePixelDepth;
	int imageWidth = oriMat.cols;
	int imageHeight = oriMat.rows;
	int nWidthByte = oriMat.step;
	//BYTE* lpRawImg = new BYTE[imageHeight*nWidthByte];

	int bUpSideDown = TRUE;

	switch (nChannels){
	case 1:
		if (isBinary)
			cImagePixelDepth = 1;
		else
			cImagePixelDepth = 8;
		break;
	case 3:
		cImagePixelDepth = 24;
		break;
	}

	//BOOL initRet = resultCImage.Initialize(imageWidth, imageHeight, cImagePixelDepth, 0);
	//BOOL initRet = resultCImage.Initialize(lpRawImg, imageWidth, imageHeight, nWidthByte, cImagePixelDepth, bUpSideDown);
	int initRet = resultCImage.Initialize(imageWidth, imageHeight, nWidthByte, cImagePixelDepth, bUpSideDown);
	if (!initRet)
		return -1;

	uchar* cimageData = resultCImage.LockRawImg();
	int cimageLinebytes = resultCImage.GetLineSizeInBytes();

	switch (cImagePixelDepth){
	case 1:
		for (int y = 0; y<imageHeight; y++){
			// l_uint32 *pixLineData = oriPix->data + y * oriPix->wpl;
			const uchar* pMatLine = oriMat.ptr<uchar>(y);
			uchar* cimageLineData = cimageData + (imageHeight - 1 - y) * cimageLinebytes;
			for (int x = 0; x<imageWidth; x++){
				int byteValue = pMatLine[x];
				int byteLoc = x / 8;
				int bitLoc = 7 - x % 8;
				uchar* byteAddress = cimageLineData + byteLoc;
				int operBase = 1 << bitLoc;
				if (byteValue == 0){//set data
					*byteAddress = *byteAddress | operBase;//(int)pow((float)2,(int)bitLoc);
				}else{//clear data
					*byteAddress = *byteAddress & ~operBase;//~((int)pow((float)2,(int)bitLoc));
				}
			}
		}
		break;
	case 8:
		for (int y = 0; y<imageHeight; y++){
			const uchar* pMatLine = oriMat.ptr<uchar>(y);
			uchar* cimageLineData = cimageData + (imageHeight - 1 - y) * cimageLinebytes;
			for (int x = 0; x<imageWidth; x++){
				int byteValue = pMatLine[x];
				*(cimageLineData + x) = byteValue;
			}
		}
		break;
	case 24:
		for (int y = 0; y < imageHeight; y++){
			const uchar* pMatLine = oriMat.ptr<uchar>(y);
			const cv::Vec3b* pMatLine3 = oriMat.ptr<cv::Vec3b>(y);
			uchar* cimageLineData = cimageData + (imageHeight - 1 - y) * cimageLinebytes;
			for (int x = 0; x < imageWidth; x++){
				const cv::Vec3b &curPixel = pMatLine3[x];
				*(cimageLineData + x * 3) = curPixel[0];
				*(cimageLineData + x * 3 + 1) = curPixel[1];
				*(cimageLineData + x * 3 + 2) = curPixel[2];
			}
		}
		break;
	default:
		finalRet = -1;
		break;
	}
	resultCImage.UnlockRawImg();
	return finalRet;
}

int LineTool::convertCImageToMat(CImage &oriCImage, cv::Mat &resultMat){
	int finalRet = 0;
	int imageType = oriCImage.GetType();
	int pixelDepth = 0;
	switch (imageType){
	case DIB_1BIT:
		pixelDepth = 1;
		break;
	case DIB_8BIT:
		pixelDepth = 8;
		break;
	case DIB_24BIT:
		pixelDepth = 24;
		break;
	case DIB_32BIT:
		pixelDepth = 32;
		break;
	default:
		pixelDepth = 0;
	}

	int imageWidth = oriCImage.GetWidth();
	int imageHeight = oriCImage.GetHeight();
	uchar* cimageData = oriCImage.LockRawImg();
	int cimageLinebytes = oriCImage.GetLineSizeInBytes();


	switch (pixelDepth){
	case 1:{
		resultMat.create(imageHeight, imageWidth, CV_8UC1);
		for (int y = 0; y < imageHeight; y++){
			uchar* cimageLineData = cimageData + (imageHeight - 1 - y) * cimageLinebytes;
			uchar* pMatLine = resultMat.ptr<uchar>(y);
			for (int x = 0; x < imageWidth; x++){
				int byteLoc = x / 8;
				int bitLoc = 7 - x % 8;
			    uchar* byteAddress = cimageLineData + byteLoc;
				int operBase = 1 << bitLoc;
				int bitValue = (*byteAddress & operBase) >> bitLoc;
				if (bitValue == 0)
					pMatLine[x] = 255;
				else
					pMatLine[x] = 0;
			}
		}
	}
		break;
	case 8:{
		resultMat.create(imageHeight, imageWidth, CV_8UC1);
		for (int y = 0; y < imageHeight; y++){
			uchar* cimageLineData = cimageData + (imageHeight - 1 - y) * cimageLinebytes;
			uchar* pMatLine = resultMat.ptr<uchar>(y);
			for (int x = 0; x < imageWidth; x++){
				int byteValue = *(cimageLineData + x);
				pMatLine[x] = byteValue;
			}
		}
	}
		break;
	case 24:
	case 32:{
		resultMat.create(imageHeight, imageWidth, CV_8UC3);
		for (int y = 0; y < imageHeight; y++){
			cv::Vec3b* pMatLine3 = resultMat.ptr<cv::Vec3b>(y);
			uchar* cimageLineData = cimageData + (imageHeight - 1 - y) * cimageLinebytes;
			for (int x = 0; x < imageWidth; x++){
				int bValue = 0, gValue = 0, rValue = 0, alphaValue = 0;
				if (pixelDepth == 24){
					bValue = *(cimageLineData + x * 3);
					gValue = *(cimageLineData + x * 3 + 1);
					rValue = *(cimageLineData + x * 3 + 2);
				}else{//32 bit
					bValue = *(cimageLineData + x * 4);
					gValue = *(cimageLineData + x * 4 + 1);
					rValue = *(cimageLineData + x * 4 + 2);
					alphaValue = *(cimageLineData + x * 4 + 3);
				}
						//mat store 24 bit
				cv::Vec3b &curPixel = pMatLine3[x];
				curPixel[0] = bValue;
				curPixel[1] = gValue;
				curPixel[2] = rValue;
			}
		}
	}
		break;
	default:
		finalRet = -1;
		break;
	}

	oriCImage.UnlockRawImg();
	return finalRet;

}


const CDirLine & LineTool::getHorLine(){
	//return m_horLine;
	return m_bHorLine;
}
const CDirLine & LineTool::getVerLine(){
	//return m_verLine;
	return m_bVerLine;
}

int LineTool::detectFrame(const cv::Mat &oriMat) {
	cv::Mat grayMat, thresholdMat;
	if (oriMat.channels() == 1) {
		oriMat.copyTo(grayMat);
	} else {
		cv::cvtColor(oriMat, grayMat, CV_BGR2GRAY);
	}
	int threshold_value = std::min(oriMat.cols, oriMat.rows);
	if ((threshold_value % 2) == 0) {
		threshold_value++;
	}

	cv::adaptiveThreshold(grayMat, thresholdMat, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, threshold_value, 5);

	m_bLineDetected = FALSE;
	if (!m_bParamsSet)
		return -1;
	int converRet = convertMatToCImage(thresholdMat, m_formImg, true);
	if (converRet != 0)
		return -2;
	
// 	cv::Mat testMat;
// 	convertCImageToMat(m_formImg, testMat);
// 	imwrite("D:\\3.jpg", testMat);

	//set paras
	m_rcFormRgn = DSCC_CRect(0, 0, m_formImg.GetWidth() - 1, m_formImg.GetHeight() - 1);
	int paraHorValue = m_bHorLine.SetDetectParams(TRUE, m_param);//Set horizontal parameters
	if (paraHorValue != 0){
		std::cout << "Set horizontal detectParams process error!" << std::endl;
		return -3;
	}
	int paraVerValue = m_bVerLine.SetDetectParams(FALSE, m_param);//Set vertical parameters
	if (paraVerValue != 0){
		std::cout << "Set vertical detectParams process error!" << std::endl;
		return -4;
	}
	
	//detect lines start
	m_bHorLine.m_nCharWidth = 0;
	m_bHorLine.m_nCharHeight = 0;

	if (m_bHorLine.detect(m_formImg, m_rcFormRgn) != 0){        //Detect horizontal lines
	
		std::cout << "horLine detect error!" << std::endl;
		return -5;
	}
	m_bVerLine.m_nCharHeight = m_bHorLine.m_nCharHeight;
	m_bVerLine.m_nCharWidth = m_bHorLine.m_nCharWidth;

	if (m_bVerLine.detect(m_formImg, m_rcFormRgn) != 0){   //detect vertical lines
		std::cout << "verLine detect error!" << std::endl;
		return -6;
	}

	//dumpFirstLine(oriMat);   //dump lines after the selectLines function
	m_dSkewAngle = getAverSkewAngle();
	m_bGetSkewAngle = TRUE;
	if (m_param.bHasSlantLine == FALSE){
		m_bHorLine.deleteSlantLine(m_dSkewAngle);
		m_bVerLine.deleteSlantLine(m_dSkewAngle);
	}
	//Merge Frame Lines
	int mergeFrameValue;
	if (m_bHorLine.m_nCharWidth == 0){  //Parameter are set for value-added tax processing
		mergeFrameValue = mergeFrameLine(50);
		if (mergeFrameValue != 0){
			return -7;
		}
	}else{
		mergeFrameValue = mergeFrameLine(max(m_bHorLine.m_nCharWidth, m_bHorLine.m_nCharHeight));
		if (mergeFrameValue != 0){
			return -8;
		}
	}
	if (m_param.m_minHorLenth > 0 || m_param.m_minVerLenth > 0){
		m_bHorLine.deleteShortLine(m_param.m_minHorLenth);
		m_bVerLine.deleteShortLine(m_param.m_minVerLenth);
	}
	m_dSkewAngle = getAverSkewAngle();
	m_bGetSkewAngle = TRUE;
	if (m_param.bHasSlantLine == FALSE) {
		m_bHorLine.deleteSlantLine(m_dSkewAngle);
		m_bVerLine.deleteSlantLine(m_dSkewAngle);
	}
	m_bLineDetected = TRUE;
	return 0;
}
/*******************************************************************************************************
/*	Name:		GetAverSkewAngle
/*	Function:	Calculate average skew angle of the form image
/*
/******************************************************************************************************/

double LineTool::getAverSkewAngle() {//Get slant angle with double-average filter
	int i, cnt, totalCnt, Len;
	double averAngle, sum, totalSum;
	m_dSkewAngle = 0;
	if (m_bHorLine.m_nLine == 0) {
		m_bGetSkewAngle = TRUE;
		return 0;
	}

	//First average 
	sum = 0;
	cnt = 0;
	for (i = 0; i < m_bHorLine.m_nLine; i++) {
		if (fabs(m_bHorLine.m_pLine[i].Angle) < 10 * PI / 180) {
			Len = GetDistance(m_bHorLine.m_pLine[i].StPnt, m_bHorLine.m_pLine[i].EdPnt);
			sum += m_bHorLine.m_pLine[i].Angle*Len;
			cnt += Len;
		}
	}	
	if (cnt != 0)
		averAngle = sum / (double)cnt;
	else
		averAngle = 0;

	//Second average
	totalSum = 0;
	totalCnt = 0;
	for (i = 0; i < m_bHorLine.m_nLine; i++) {
		if (fabs(m_bHorLine.m_pLine[i].Angle - averAngle) < 5 * PI / 180) {
			Len = GetDistance(m_bHorLine.m_pLine[i].StPnt, m_bHorLine.m_pLine[i].EdPnt);
			totalSum += m_bHorLine.m_pLine[i].Angle*Len;
			totalCnt += Len;
		}
	}
	if (m_bHorLine.m_nLine < 20) {
		sum = 0;
		cnt = 0;
		for (i = 0; i < m_bVerLine.m_nLine; i++) {
			if (fabs(m_bVerLine.m_pLine[i].Angle + PI / 2) < 10 * PI / 180) {
				Len = GetDistance(m_bVerLine.m_pLine[i].StPnt, m_bVerLine.m_pLine[i].EdPnt);
				sum += m_bVerLine.m_pLine[i].Angle*Len;
				cnt += Len;
			}
		}
		if (cnt != 0)
			averAngle = sum / (double)cnt;
		else
			averAngle = 0;

		for (i = 0; i < m_bVerLine.m_nLine; i++) {
			if (fabs(m_bVerLine.m_pLine[i].Angle - averAngle) < 5 * PI / 180) {
				Len = GetDistance(m_bVerLine.m_pLine[i].StPnt, m_bVerLine.m_pLine[i].EdPnt);
				totalSum += (m_bVerLine.m_pLine[i].Angle + PI / 2)*Len;
				totalCnt += Len;
			}
		}
	}

	if (cnt != 0)//condition error!  if(TotalCnt!=0)   question!
		m_dSkewAngle = totalSum / (double)totalCnt;

	for (i = 0; i<m_bHorLine.m_nLine; i++) {
		if (fabs(m_bHorLine.m_pLine[i].Angle - m_dSkewAngle) > 10 * PI / 180)
			m_bHorLine.m_pLine[i].bSlant = TRUE;
		else
			m_bHorLine.m_pLine[i].bSlant = FALSE;
	}
	for (i = 0; i<m_bVerLine.m_nLine; i++) {
		if (fabs(m_bVerLine.m_pLine[i].Angle - m_dSkewAngle + PI / 2) > 10 * PI / 180)
			m_bVerLine.m_pLine[i].bSlant = TRUE;
		else
			m_bVerLine.m_pLine[i].bSlant = FALSE;
	}
	m_bGetSkewAngle = TRUE;
	return m_dSkewAngle;
}

/**********************************************************************************************
/*	Name:		MergeFrameLine
/*	Function:	Merge frame lines within tolerance
/*
/**********************************************************************************************/
int LineTool::mergeFrameLine(int nTDistance) {
	int i, j, k, xs, xe, ys, ye, xmid, ymid;
	FORMLINE Li, Lj;
	DSCC_CPoint Cross, Mid;
	int	bMerge; 
	int nMidLines;
	for (i = 0; i < m_bHorLine.m_nLine; i++) {
		bMerge = FALSE;
		Li = m_bHorLine.m_pLine[i];
		for (j = i + 1; j<m_bHorLine.m_nLine; j++) {
			Lj = m_bHorLine.m_pLine[j];
			xs = min(Li.EdPnt.x, Lj.EdPnt.x);
			xe = max(Li.StPnt.x, Lj.StPnt.x);
			xmid = (xs + xe) / 2;
			double LixMid = m_bHorLine.fYofLine(Li, xmid);
			double LjxMid = m_bHorLine.fYofLine(Lj, xmid);
			//if(fabs(LixMid - LjxMid) > -5 )	continue; 
			if (fabs(m_bHorLine.fYofLine(Li, xmid) - m_bHorLine.fYofLine(Lj, xmid)) > m_param.m_midlineDiffThresh)	continue;
			if (xs >= xe && (fabs(Li.Angle - Lj.Angle) < m_param.m_angleThresh1*PI / 180 || fabs(fabs(Li.Angle - Lj.Angle) - PI) < m_param.m_angleThresh1*PI / 180)) {
				m_bHorLine.mergeLine(i, j);
				j--;
				bMerge = TRUE;
			} else if (xe < xs + nTDistance && min(GetDistance(Li.StPnt, Lj.EdPnt), GetDistance(Lj.StPnt, Li.EdPnt)) < nTDistance
				&& (fabs(Li.Angle - Lj.Angle) < m_param.m_angleThresh2*PI / 180 || fabs(fabs(Li.Angle - Lj.Angle) - PI) < m_param.m_angleThresh2*PI / 180)) {
				xmid = (xs + xe) / 2;
				ymid = (int)((m_bHorLine.fYofLine(Li, xmid) + m_bHorLine.fYofLine(Lj, xmid)) / 2);
				Mid = DSCC_CPoint(xmid, ymid);
				nMidLines = 0;
				for (k = 0; k < m_bVerLine.m_nLine; k++) {
					if (getCrossPoint(Li, m_bVerLine.m_pLine[k], Cross, nTDistance) == 0)
						if (GetDistance(Cross, Mid) < nTDistance / 2 && Cross.x > xs - 3 && Cross.x < xe + 3)
							nMidLines++;
				}
				if (nMidLines <= m_param.m_nMidLinesThresh) {
					m_bHorLine.mergeLine(i, j);
					j--;
					bMerge = TRUE;
				}
			}
		}
		if (bMerge)		
			i--;
	}
	for (i = 0; i < m_bVerLine.m_nLine; i++) {
		bMerge = FALSE;
		Li = m_bVerLine.m_pLine[i];
		for (j = i + 1; j<m_bVerLine.m_nLine; j++) {
			Lj = m_bVerLine.m_pLine[j];

			ys = min(Li.EdPnt.y, Lj.EdPnt.y);
			ye = max(Li.StPnt.y, Lj.StPnt.y);
			ymid = (ys + ye) / 2;
			if (fabs(m_bVerLine.fYofLine(Li, ymid) - m_bVerLine.fYofLine(Lj, ymid)) > m_param.m_midlineDiffThresh)	continue;
			if (ys >= ye && (fabs(Li.Angle - Lj.Angle) < m_param.m_angleThresh2*PI / 180 || fabs(fabs(Li.Angle - Lj.Angle) - PI) < m_param.m_angleThresh2*PI / 180)) {
				m_bVerLine.mergeLine(i, j);
				j--;
				bMerge = TRUE;
			} else if (ye < ys + nTDistance && min(GetDistance(Li.StPnt, Lj.EdPnt), GetDistance(Lj.StPnt, Li.EdPnt)) < nTDistance
				&& (fabs(Li.Angle - Lj.Angle) < m_param.m_angleThresh2*PI / 180 || fabs(fabs(Li.Angle - Lj.Angle) - PI) < m_param.m_angleThresh2*PI / 180)) {
				ymid = (ys + ye) / 2;
				xmid = (int)((m_bVerLine.fYofLine(Li, ymid) + m_bVerLine.fYofLine(Lj, ymid)) / 2);
				Mid = DSCC_CPoint(xmid, ymid);
				nMidLines = 0;
				for (k = 0; k < m_bHorLine.m_nLine; k++) {
					if (getCrossPoint(Li, m_bHorLine.m_pLine[k], Cross, nTDistance) == 0)
						if (GetDistance(Cross, Mid) < nTDistance / 2 && Cross.y > ys - 3 && Cross.y < ye + 3)
							nMidLines++;
				}
				if (nMidLines <= m_param.m_nMidLinesThresh) {
					m_bVerLine.mergeLine(i, j);
					j--;
					bMerge = TRUE;
				}
			}
		}
		if (bMerge)   
			bMerge = FALSE;
	}
	return 0;
}
/*****************************************************************************************
/*	Name:		GetCrossPoint
/*	Function:	Calculate the cross point of two frame lines within the tolerance
/*
/******************************************************************************************/
int LineTool::getCrossPoint(FORMLINE &l1, FORMLINE &l2, DSCC_CPoint &crossPnt, int nTDistance) {
	if (fabs(l1.Angle - l2.Angle) < 10 * PI / 180 || fabs(fabs(l1.Angle - l2.Angle) - PI) < 10 * PI / 180)
		return -1;

	double x1, y1, x2, y2, _x1, _y1, _x2, _y2, dx, _dx, dy, _dy, x, y;
	DSCC_POINT s1 = l1.StPnt; DSCC_POINT e1 = l1.EdPnt; DSCC_POINT s2 = l2.StPnt; DSCC_POINT e2 = l2.EdPnt;

	x1 = s1.x; y1 = s1.y; x2 = e1.x; y2 = e1.y; _x1 = s2.x; _y1 = s2.y; _x2 = e2.x; _y2 = e2.y;
	dx = x2 - x1; dy = y2 - y1; _dx = _x2 - _x1; _dy = _y2 - _y1;


	if (_dx*dy == dx*_dy)	return -1;
	if (dx == 0) {
		x = x1;
		y = _y1 + (x1 - _x1)*_dy / _dx;
	} else {
		x = (dx*_dx*(_y1 - y1) - dx*_dy*_x1 + _dx*dy*x1) / (_dx*dy - dx*_dy);
		y = (dy*(x - x1) + y1*dx) / dx;
	}
	crossPnt.x = (int)(x + 0.5);
	crossPnt.y = (int)(y + 0.5);

	if (fabs(GetDistance(x, y, x1, y1) + GetDistance(x, y, x2, y2) -
		GetDistance(x1, y1, x2, y2)) > 2 * (double)nTDistance
		|| fabs(GetDistance(x, y, _x1, _y1) + GetDistance(x, y, _x2, _y2) -
		GetDistance(_x1, _y1, _x2, _y2)) > 2 * (double)nTDistance)
		return -2;
	return 0;
}


int LineTool::getLines(std::vector<FORMLINE> &horLines,
	std::vector<FORMLINE> &verLines, double &angle) {
	if (!m_bLineDetected)
		return -1;

	std::vector<FORMLINE> horLines_first, verLines_first;
	for (int lineIndex = 0; lineIndex < m_bHorLine.m_nLine;lineIndex++){
		FORMLINE transferLine = m_bHorLine.m_pLine[lineIndex];
		horLines_first.push_back(transferLine);
	}
	for (int lineIndex = 0; lineIndex < m_bVerLine.m_nLine;lineIndex++){
		FORMLINE transferLine = m_bVerLine.m_pLine[lineIndex];
		verLines_first.push_back(transferLine);
	}
	angle = m_dSkewAngle;

	//line to point
	std::vector<cv::Point> horLines_srcStart, horLines_srcEnd, verLines_srcStart, verLines_srcEnd;
	for (int horIndex = 0; horIndex < horLines_first.size(); horIndex++){
		FORMLINE & curHorLine = horLines_first[horIndex];
		cv::Point curLineStart, curLineEnd;
		curLineStart.x = curHorLine.StPnt.x;
		curLineStart.y = curHorLine.StPnt.y;
		curLineEnd.x = curHorLine.EdPnt.x;
		curLineEnd.y = curHorLine.EdPnt.y;
		horLines_srcStart.push_back(curLineStart);
		horLines_srcEnd.push_back(curLineEnd);
	}
	for (int verIndex = 0; verIndex < verLines_first.size(); verIndex++){
		FORMLINE & curVerLine = verLines_first[verIndex];
		cv::Point curLineStart, curLineEnd;
		curLineStart.x = curVerLine.StPnt.x;
		curLineStart.y = curVerLine.StPnt.y;
		curLineEnd.x = curVerLine.EdPnt.x;
		curLineEnd.y = curVerLine.EdPnt.y;
		verLines_srcStart.push_back(curLineStart);
		verLines_srcEnd.push_back(curLineEnd);
	}
	//rorate line
	std::vector<cv::Point> horLines_dstStart, horLines_dstEnd, verLines_dstStart, verLines_dstEnd;

	int rotate_value1 = pointRotate(angle, m_rcFormRgn.Width() / 2, m_rcFormRgn.Height() / 2, horLines_srcStart, horLines_dstStart);
	if (rotate_value1 != 0){
		std::cout << "First pointRotate process error!" << std::endl;
		return -1;
	}
	int rotate_value2 = pointRotate(angle, m_rcFormRgn.Width() / 2, m_rcFormRgn.Height() / 2, horLines_srcEnd, horLines_dstEnd);
	if (rotate_value2 != 0){
		std::cout << "Second pointRotate process error!" << std::endl;
		return -1;
	}
	int rotate_value3 = pointRotate(angle, m_rcFormRgn.Width() / 2, m_rcFormRgn.Height() / 2, verLines_srcStart, verLines_dstStart);
	if (rotate_value3 != 0){
		std::cout << "Third pointRotate process error!" << std::endl;
		return -1;
	}
	int rotate_value4 = pointRotate(angle, m_rcFormRgn.Width() / 2, m_rcFormRgn.Height() / 2, verLines_srcEnd, verLines_dstEnd);
	if (rotate_value4 != 0){
		std::cout << "Fourth pointRotate process error!" << std::endl;
		return -1;
	}
	// point to form line	 
	for (int pointIndex = 0; pointIndex < horLines_dstEnd.size(); pointIndex++){
		cv::Point & curhorpointStart = horLines_dstStart[pointIndex];
		cv::Point & curhorpointEnd = horLines_dstEnd[pointIndex];
		FORMLINE singlehorline;
		singlehorline.StPnt.x = curhorpointStart.x;
		singlehorline.StPnt.y = curhorpointStart.y;
		singlehorline.EdPnt.x = curhorpointEnd.x;
		singlehorline.EdPnt.y = curhorpointEnd.y;
		horLines.push_back(singlehorline);
	}
	for (int pointIndex = 0; pointIndex < verLines_dstEnd.size(); pointIndex++){
		cv::Point & curverpointStart = verLines_dstStart[pointIndex];
		cv::Point & curverpointEnd = verLines_dstEnd[pointIndex];
		FORMLINE singleverline;
		singleverline.StPnt.x = curverpointStart.x;
		singleverline.StPnt.y = curverpointStart.y;
		singleverline.EdPnt.x = curverpointEnd.x;
		singleverline.EdPnt.y = curverpointEnd.y;
		verLines.push_back(singleverline);
	}
	return 0;
}

int LineTool::getOuterRects(std::vector<FORMLINE> &horLines,
	std::vector<FORMLINE> &verLines, std::vector<cv::Rect> &rects){
	if (!m_bLineDetected)
		return -1;
	std::vector<cv::Point> Rects_pt1, Rects_pt2, Rects_pt3, Rects_pt4;
	//sort the HorLines by value y
	std::sort(horLines.begin(), horLines.end(), SortByLine_y);

	for (std::vector<FORMLINE>::iterator it_hor = horLines.begin(); it_hor != horLines.end(); it_hor++){
		FORMLINE &lineHor = *it_hor;
		int i = 1;
		int j = 1;

		cv::Point temporary_pt1, temporary_pt2, temporary_pt3, temporary_pt4;
		for (std::vector<FORMLINE>::iterator it_ver = verLines.begin(); it_ver != verLines.end(); it_ver++){
			FORMLINE &lineVer = *it_ver;

			if (distance(lineHor.StPnt.x, lineHor.StPnt.y, lineVer.StPnt.x, lineVer.StPnt.y) < m_param.m_minRectPointDistance){
				while (i){              //The first assignment
					temporary_pt1.x = lineHor.StPnt.x;
					temporary_pt1.y = lineHor.StPnt.y;
					temporary_pt3.x = lineVer.EdPnt.x;
					temporary_pt3.y = lineVer.EdPnt.y;
					i--;
				}
				temporary_pt1.x = std::min((long)temporary_pt1.x, (long)lineVer.StPnt.x);
				temporary_pt1.y = std::min((long)temporary_pt1.y, (long)lineVer.StPnt.y);

				temporary_pt3.x = std::min((long)lineVer.EdPnt.x, (long)temporary_pt3.x);
				temporary_pt3.y = std::max((long)lineVer.EdPnt.y, (long)temporary_pt3.y);
			}
			if (distance(lineHor.EdPnt.x, lineHor.EdPnt.y, lineVer.StPnt.x, lineVer.StPnt.y) < m_param.m_minRectPointDistance){
				while (j) { //The first assignment
					temporary_pt2.x = lineHor.EdPnt.x;
					temporary_pt2.y = lineHor.EdPnt.y;
					temporary_pt4.x = 0;
					temporary_pt4.y = 0;
					j--;
				}
				temporary_pt2.x = std::max((long)lineVer.StPnt.x, (long)temporary_pt2.x);
				temporary_pt2.y = std::min((long)lineVer.StPnt.y, (long)temporary_pt2.y);

				temporary_pt4.x = std::max((long)lineVer.EdPnt.x, (long)temporary_pt4.x);
				temporary_pt4.y = std::max((long)lineVer.EdPnt.y, (long)temporary_pt4.y);
			}
		}
		if (temporary_pt1.x > 0 && temporary_pt1.y > 0 &&
			temporary_pt2.x > 0 && temporary_pt2.y > 0 &&
			temporary_pt3.x > 0 && temporary_pt3.y > 0 &&
			temporary_pt4.x > 0 && temporary_pt4.y > 0){

			for (std::vector<FORMLINE>::iterator horIndex = it_hor + 1; horIndex != horLines.end(); horIndex++){
				FORMLINE &UnderLineHor = *horIndex;
				if ((distance(temporary_pt3.x, temporary_pt3.y, UnderLineHor.StPnt.x, UnderLineHor.StPnt.y) < m_param.m_minRectPointDistance) &&
					(distance(temporary_pt4.x, temporary_pt4.y, UnderLineHor.EdPnt.x, UnderLineHor.EdPnt.y) < m_param.m_minRectPointDistance)){
					Rects_pt1.push_back(temporary_pt1);
					Rects_pt2.push_back(temporary_pt2);
					Rects_pt3.push_back(temporary_pt3);
					Rects_pt4.push_back(temporary_pt4);
					it_hor = horIndex;
					break;
				}
			}
		}
	}
	cv::Rect singleRect;
	for (int rectIndex = 0; rectIndex < Rects_pt1.size(); rectIndex++){
		cv::Point & curRect_pt1 = Rects_pt1[rectIndex];
		cv::Point & curRect_pt2 = Rects_pt2[rectIndex];
		cv::Point & curRect_pt3 = Rects_pt3[rectIndex];
		cv::Point & curRect_pt4 = Rects_pt4[rectIndex];
		singleRect.x = curRect_pt1.x;
		singleRect.y = curRect_pt1.y;
		singleRect.width = std::max(curRect_pt2.x, curRect_pt4.x) - curRect_pt1.x;
		singleRect.height = std::max(curRect_pt3.y, curRect_pt4.y) - curRect_pt1.y;
		rects.push_back(singleRect);
	}

	return 0;
}

/***************************************************************************
/*	Name:		DumpFormLine
/*	Function:	Dump form frame lines to an image for debugging
/*
/****************************************************************************/

// int LineTool::dumpFirstLine(const cv::Mat &oriMat){
// 	cv::Mat singleChannelImage, drawing_Mat;
// 	drawing_Mat = cvCreateMat(oriMat.rows, oriMat.cols, IPL_DEPTH_8U);
// 	oriMat.copyTo(singleChannelImage);
// 	cvtColor(singleChannelImage, drawing_Mat, CV_GRAY2BGR);
// 	for (int col_nline = 0; col_nline < m_bHorLine.m_nLine; col_nline++){
// 		FORMLINE & curhorLines = m_bHorLine.m_pLine[col_nline];
// 		cv::Point pt1, pt2;
// 		cv::Rect col_rect1, col_rect2;
// 		pt1.x = curhorLines.StPnt.x;
// 		pt1.y = curhorLines.StPnt.y;
// 		pt2.x = curhorLines.EdPnt.x;
// 		pt2.y = curhorLines.EdPnt.y;
// 		
// 		col_rect1.x = pt1.x - 4;
// 		col_rect1.y = pt1.y - 4;
// 		col_rect1.width = 8;
// 		col_rect1.height = 8;
// 
// 		col_rect2.x = pt2.x - 4;
// 		col_rect2.y = pt2.y - 4;
// 		col_rect2.width = 8;
// 		col_rect2.height = 8;
// 
// 		line(drawing_Mat, pt1, pt2, CV_RGB(0, 255, 0), 3);
// 		cv::rectangle(drawing_Mat, col_rect1, CV_RGB(255, 0, 0), 2);
// 		cv::rectangle(drawing_Mat, col_rect2, CV_RGB(0, 0, 255), 2);
// 	}
// 
// 	for (int col_nline = 0; col_nline < m_bVerLine.m_nLine; col_nline++){
// 		FORMLINE &curverLines = m_bVerLine.m_pLine[col_nline];
// 
// 		cv::Point pt1, pt2;
// 		cv::Rect col_rect1, col_rect2;
// 		pt1.x = curverLines.StPnt.x;
// 		pt1.y = curverLines.StPnt.y;
// 		pt2.x = curverLines.EdPnt.x;
// 		pt2.y = curverLines.EdPnt.y;
// 
// 		col_rect1.x = pt1.x - 4;
// 		col_rect1.y = pt1.y - 4;
// 		col_rect1.width = 8;
// 		col_rect1.height = 8;
// 		
// 		col_rect2.x = pt2.x - 4;
// 		col_rect2.y = pt2.y - 4;
// 		col_rect2.width = 8;
// 		col_rect2.height = 8;
// 
// 		line(drawing_Mat, pt1, pt2, CV_RGB(0, 255, 255), 3);
// 		cv::rectangle(drawing_Mat, col_rect1, CV_RGB(255, 255, 0), 2);
// 		cv::rectangle(drawing_Mat, col_rect2, CV_RGB(255, 0, 255), 2);
// 	}
// 	imwrite("D:\\1.jpg", drawing_Mat);
// 	return 0;
// }
