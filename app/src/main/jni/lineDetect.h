//#pragma once
//
//#include "resource.h"
#ifndef NKOCR_LINE_DETECT_H_
#define NKOCR_LINE_DETECT_H_

#include <vector>
#include "DirLine.h"
#include "nkDirLine.h"
#include "cv.h"
//#include "allheaders.h"
class LineTool {
private:
	NkLinePara m_param;
	CDirLine m_horLine;
	CDirLine m_verLine;
	//add for v3.0
	NkDirLine m_bHorLine;
	NkDirLine m_bVerLine;
	CImage	m_formImg;
	DSCC_CRect	m_rcFormRgn;
	double	m_dSkewAngle;	//Skew angle of form image
	int	m_bParamsSet;		//Flag to indicate if having set line detection parameter
	int	m_bLineDetected;	//Flag to indicate if having detected frame lines
	int	m_bCellExtracted;	//Flag to indicate if having extracted form cells
	int	m_bLineErased;		//Flag to indicate if having erased frame lines
	int	m_bGetSkewAngle;	//Flag to indicate if having estimated skew angle
private:
	int  distance(int x1, int y1, int x2, int y2);
	int  pointRotate(double angle, float centerX, float centerY,const std::vector<cv::Point> &srcPoints, std::vector<cv::Point> &dstPoints);
	double getAngle(const cv::Point &lineStart, const cv::Point &lineEnd);
//	int dumpFirstLine(const cv::Mat &oriMat);
public:
	LineTool();
	const CDirLine& getHorLine();
	const CDirLine& getVerLine();
	//set Param
	void setParam(const NkLinePara &para);
	NkLinePara getDefaultPara();

	//detect Lines and rects
	int detectOuterRects(const cv::Mat &oriMat, double &anlge, std::vector<cv::Rect> &rects);
	int detectLines(const cv::Mat &oriMat, std::vector<FORMLINE> &horLines,std::vector<FORMLINE> &verLines, double &angle);
	int getOuterRects(std::vector<FORMLINE> &horLines, std::vector<FORMLINE> &verLines,std::vector<cv::Rect> &rects);
	int imageRorate(const cv::Mat &image_original, cv::Mat &image_Rorate, double skewAngle);

	//convert image save format
// 	int  convertPixToCImage(Pix *oriPix, CImage &resultCImage);
// 	Pix* convertCImageToPix(CImage &oriCImage);
// 	Pix* convertMatToPix(cv::Mat &oriMat, bool isBinary);
// 	int  convertPixToMat(Pix *oriPix, cv::Mat &resultMat);
	int  convertMatToCImage(const cv::Mat &oriMat, CImage &resultCImage, bool isBinary);
	int  convertCImageToMat(CImage &oriCImage, cv::Mat &resultMat);
	//add for v3.0
public:
	//new add filter lines 
	int	detectFrame(const cv::Mat &oriMat);
	int getLines(std::vector<FORMLINE> &horLines,std::vector<FORMLINE> &verLines, double &angle);

private:
	double	getAverSkewAngle();
	int	mergeFrameLine(int nTDistance);
	int	getCrossPoint(FORMLINE &l1, FORMLINE &l2, DSCC_CPoint &crossPnt, int nTDistance);
};

#endif