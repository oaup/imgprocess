#ifndef _QUADDETECT_H
#define _QUADDETECT_H

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"

#include <vector>

using namespace std;
using namespace cv;

namespace nkocr {
	class QuadDetectParam {
	public:
		int m_clusterNum;
		int m_outRows;
		int m_outCols;
	};
	class QuadDetect {
	public:
		QuadDetect();
		int detect(const string &filepath, vector<Point> &resContour);
		int detect(const Mat &inputMat, vector<Point> &resContour);
		int doPerspective(const Mat &inputMat, vector<Point> &contour, Mat &resMat);
		int doPerspective(const string &filepath, vector<Point> &contour, Mat &resMat);
		QuadDetectParam &getParam();
		void setParam(QuadDetectParam &param);
		
	private:
		int detectByYUV(const Mat &inputMat, vector<Point> &resContour);
		int detectByAB(const Mat &inputMat, vector<Point> &resContour);
		int combination(int n, int r, vector<vector<int> > &res);
		void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center);
		// static bool sortByX(Point2f &pt1, Point2f &pt2);
		// static bool sortByY(Point2f &pt1, Point2f &pt2);
	// member
		QuadDetectParam m_param;
	};
}

#endif