LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
#opencv
OPENCVROOT := E:\\Android\\library\\OpenCV-android-sdk
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=SHARED
include ${OPENCVROOT}/sdk/native/jni/OpenCV.mk

LOCAL_SRC_FILES := main.cpp quadDetect.cpp ImgEffects.cpp\
                   ConnTree.cpp DetectLine.cpp DirLine.cpp DirLineChains.cpp DirLineDSCC.cpp\
                   DirLineDump.cpp DirLineFilterSkew.cpp FormId.cpp GetProcessFile.cpp\
                   GroupRunLength.cpp ImageObj.cpp ImageTool.cpp lineDetect.cpp\
                   nkDirLine.cpp ParalLine.cpp ProcForm.cpp ReadWriteHMM.cpp Tools.cpp TrainHMM.cpp\

LOCAL_LDLIBS += -llog -ljnigraphics
LOCAL_MODULE := nkImageLib

include $(BUILD_SHARED_LIBRARY)
