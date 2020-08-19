
// #include "stdafx.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"

#include <iostream>
#include <algorithm>

#include "quadDetect.h"

namespace nkocr {

	bool sortByX(const Point2f &pt1, const Point2f &pt2) {
		return pt1.x < pt2.x;
	}

	bool sortByY(const Point2f &pt1, const Point2f &pt2) {
		return pt1.y < pt2.y;
	}

	QuadDetect::QuadDetect() {
		m_param.m_clusterNum = 2;
		m_param.m_outRows = 600;
		m_param.m_outCols = 800;
	}

	QuadDetectParam &QuadDetect::getParam() {
		return m_param;
	}

	void QuadDetect::setParam(QuadDetectParam &param) {
		m_param = param;
	}

	
	int QuadDetect::combination(int n, int r, vector<vector<int> > &res) {
		std::vector<bool> v(n);
		std::fill(v.begin() + n - r, v.end(), true);

		do {
			vector<int> curComb;
			for (int i = 0; i < n; ++i) {
				if (v[i]) {
					// std::cout << (i + 1) << " ";
					curComb.push_back(i);
				}
			}
			// std::cout << "\n";
			res.push_back(curComb);
		} while (std::next_permutation(v.begin(), v.end()));
		return 0;
	}

	void QuadDetect::sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center) {
		std::vector<cv::Point2f> top, bot;

		for (int i = 0; i < corners.size(); i++) {
			if (corners[i].y < center.y)
				top.push_back(corners[i]);
			else
				bot.push_back(corners[i]);
		}
		corners.clear();

		if (top.size() == 2 && bot.size() == 2) {
			cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
			cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
			cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
			cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];

			corners.push_back(tl);
			corners.push_back(tr);
			corners.push_back(br);
			corners.push_back(bl);
		}
	}

	int QuadDetect::detectByYUV(const Mat &inputMat, vector<Point> &resContour) {
		Mat yuvMat;

		cvtColor(inputMat, yuvMat, CV_BGR2YUV);
		const int minRows = 300;
		float shrinkedRate = 1;
		if (inputMat.rows > minRows) {
			shrinkedRate = (float)inputMat.rows / minRows;
			int correspondedCols = inputMat.cols / shrinkedRate;			
			resize(yuvMat, yuvMat, Size(correspondedCols, minRows));
		}
		else {
			shrinkedRate = 1;
		}

		Mat fYuvMat(yuvMat.rows, yuvMat.cols, CV_32FC3);

		for (int j = 0; j < yuvMat.rows; j++) {
			Vec3b *curLine = yuvMat.ptr<Vec3b>(j);
			for (int i = 0; i < yuvMat.cols; i++) {
				Vec3b curPixel = curLine[i];
				fYuvMat.at<Vec3f>(j, i) = Vec3f((float)curPixel[0],
					(float)curPixel[1], (float)curPixel[2]);
			}
		}

		Mat labels;

		Mat centers(1, m_param.m_clusterNum, CV_32FC3);
		int rowsNum = fYuvMat.rows;
		int colsNum = fYuvMat.cols;
		Mat fPixels = fYuvMat.reshape(0, 1);

		kmeans(fPixels, m_param.m_clusterNum, labels,
			TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0),
			3, KMEANS_PP_CENTERS, centers);

		Mat labelPixel = labels.reshape(0, rowsNum);

		Mat labelMat = Mat(labelPixel.rows, labelPixel.cols, CV_8UC3);


		for (int j = 0; j < labelPixel.rows; j++) {
			for (int i = 0; i < labelPixel.cols; i++) {
				Vec3b fgColor = Vec3b(255, 255, 255);
				Vec3b bgColor = Vec3b(0, 0, 0);
				int label = labelPixel.at<int>(j, i);
				if (label == 0)
					labelMat.at<Vec3b>(j, i) = fgColor;
				else
					labelMat.at<Vec3b>(j, i) = bgColor;
			}
		}

		cvtColor(labelMat, labelMat, CV_BGR2GRAY);

		vector< vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(labelMat, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		vector<vector<Point> > contoursPoly(contours.size());
		Mat contoursMat = Mat::zeros(labelMat.size(), CV_8UC1);


		for (int i = 0; i < contours.size(); i++) {
			Scalar monoColor = Scalar(255);
			approxPolyDP(Mat(contours[i]), contoursPoly[i], 3, true);
			drawContours(contoursMat, contours, i, monoColor, 1);
		}

		// draw biggest contour
		int maxIdx = -1;
		int maxArea = 0;
		Scalar color = CV_RGB(255, 0, 0);
		for (int i = 0; i < contoursPoly.size(); i++) {
			double curArea = contourArea(contoursPoly[i]);
			if (maxArea < curArea && (curArea / (contoursMat.cols * contoursMat.rows) < 0.95)) {
				maxArea = curArea;
				maxIdx = i;
			}
		}

		// select 4 points from contourspoly[maxIdx]
		vector<vector<int> > allCombs;
		vector<Point> probableContour = contoursPoly[maxIdx];
		// vector<Point> resContour;

		int n = probableContour.size();
		int r = 4;
		if (n >= 4 && n <= 30) {
			combination(n, r, allCombs);
			double maxCombArea = 0.0;
			int maxCombIdx = -1;

			for (int i = 0; i < allCombs.size(); i++) {
				vector<int> &curComb = allCombs[i];
				vector<Point> curCont;
				for (int j = 0; j < curComb.size(); j++) {
					int ptIdx = curComb[j];
					curCont.push_back(probableContour[ptIdx]);
				}
				double curCombArea = contourArea(curCont);
				if (maxCombArea < curCombArea) {
					maxCombIdx = i;
					maxCombArea = curCombArea;
					resContour = curCont;
				}
			}
		}
		else {
			resContour.push_back(Point(yuvMat.cols / 4, yuvMat.rows / 4));
			resContour.push_back(Point(yuvMat.cols * 3 / 4, yuvMat.rows / 4));
			resContour.push_back(Point(yuvMat.cols * 3 / 4, yuvMat.rows * 3 / 4));
			resContour.push_back(Point(yuvMat.cols / 4, yuvMat.rows * 3 / 4));
			return -1;
		}

		for (int i = 0; i < resContour.size(); i++) {
			Point &curPoint = resContour[i];
			curPoint.x = (int)(curPoint.x * shrinkedRate);
			curPoint.y = (int)(curPoint.y * shrinkedRate);
		}

		return 0;
	}

	int QuadDetect::detectByAB(const Mat &inputMat, vector<Point> &resContour) {
		Mat labMat;

		cvtColor(inputMat, labMat, CV_BGR2Lab);
		const int minRows = 300;
		float shrinkedRate = 1;
		if (inputMat.rows > minRows) {
			shrinkedRate = (float)inputMat.rows / minRows;
			int correspondedCols = inputMat.cols / shrinkedRate;
			resize(labMat, labMat, Size(correspondedCols, minRows));
		}
		else {
			shrinkedRate = 1;
		}

		Mat fAbMat(labMat.rows, labMat.cols, CV_32FC2);

		for (int j = 0; j < labMat.rows; j++) {
			Vec3b *curLine = labMat.ptr<Vec3b>(j);
			for (int i = 0; i < labMat.cols; i++) {
				Vec3b curPixel = curLine[i];
				fAbMat.at<Vec2f>(j, i) = Vec2f(
					(float)curPixel[1], (float)curPixel[2]);
			}
		}

		Mat labels;

		Mat centers(1, m_param.m_clusterNum, CV_32FC2);
		int rowsNum = fAbMat.rows;
		int colsNum = fAbMat.cols;
		Mat fPixels = fAbMat.reshape(0, 1);

		kmeans(fPixels, m_param.m_clusterNum, labels,
			TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0),
			3, KMEANS_PP_CENTERS, centers);

		Mat labelPixel = labels.reshape(0, rowsNum);

		Mat labelMat = Mat(labelPixel.rows, labelPixel.cols, CV_8UC3);


		for (int j = 0; j < labelPixel.rows; j++) {
			for (int i = 0; i < labelPixel.cols; i++) {
				Vec3b fgColor = Vec3b(255, 255, 255);
				Vec3b bgColor = Vec3b(0, 0, 0);
				int label = labelPixel.at<int>(j, i);
				if (label == 0)
					labelMat.at<Vec3b>(j, i) = fgColor;
				else
					labelMat.at<Vec3b>(j, i) = bgColor;
			}
		}

		cvtColor(labelMat, labelMat, CV_BGR2GRAY);

		vector< vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(labelMat, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		vector<vector<Point> > contoursPoly(contours.size());
		Mat contoursMat = Mat::zeros(labelMat.size(), CV_8UC1);


		for (int i = 0; i < contours.size(); i++) {
			Scalar monoColor = Scalar(255);
			approxPolyDP(Mat(contours[i]), contoursPoly[i], 3, true);
			drawContours(contoursMat, contours, i, monoColor, 1);
		}

		// draw biggest contour
		int maxIdx = -1;
		int maxArea = 0;
		Scalar color = CV_RGB(255, 0, 0);
		for (int i = 0; i < contoursPoly.size(); i++) {
			double curArea = contourArea(contoursPoly[i]);
			if (maxArea < curArea && (curArea / (contoursMat.cols * contoursMat.rows) < 0.95)) {
				maxArea = curArea;
				maxIdx = i;
			}
		}

		// select 4 points from contourspoly[maxIdx]
		vector<vector<int> > allCombs;
		vector<Point> probableContour = contoursPoly[maxIdx];
		// vector<Point> resContour;

		int n = probableContour.size();
		int r = 4;
		if (n >= 4 && n <= 30) {
			combination(n, r, allCombs);
			double maxCombArea = 0.0;
			int maxCombIdx = -1;

			for (int i = 0; i < allCombs.size(); i++) {
				vector<int> &curComb = allCombs[i];
				vector<Point> curCont;
				for (int j = 0; j < curComb.size(); j++) {
					int ptIdx = curComb[j];
					curCont.push_back(probableContour[ptIdx]);
				}
				double curCombArea = contourArea(curCont);
				if (maxCombArea < curCombArea) {
					maxCombIdx = i;
					maxCombArea = curCombArea;
					resContour = curCont;
				}
			}
		}
		else {
			resContour.push_back(Point(labMat.cols / 4, labMat.rows / 4));
			resContour.push_back(Point(labMat.cols * 3 / 4, labMat.rows / 4));
			resContour.push_back(Point(labMat.cols * 3 / 4, labMat.rows * 3 / 4));
			resContour.push_back(Point(labMat.cols / 4, labMat.rows * 3 / 4));
			return -1;
		}

		for (int i = 0; i < resContour.size(); i++) {
			Point &curPoint = resContour[i];
			curPoint.x = (int)(curPoint.x * shrinkedRate);
			curPoint.y = (int)(curPoint.y * shrinkedRate);
		}

		return 0;
	}

	int QuadDetect::detect(const Mat &inputMat, vector<Point> &resContour) {
		int retVal = detectByYUV(inputMat, resContour);
		if (retVal == -1)
			retVal = detectByAB(inputMat, resContour);

		return retVal;		
	}

	int QuadDetect::detect(const string &filepath, vector<Point> &resContour) {
		const Mat &inputMat = imread(filepath);
		return detect(inputMat, resContour);
	}

	int QuadDetect::doPerspective(const Mat &inputMat, vector<Point> &contour, Mat &resMat) {
		vector<Point2f> corners;

		for (int i = 0; i < contour.size(); i++) {
			Point curPoint = contour[i];
			corners.push_back(Point2f((float)curPoint.x, (float)curPoint.y));
		}

		Point2f center;
		// Get mass center
		for (int i = 0; i < corners.size(); i++)
			center += corners[i];
		center *= (1. / corners.size());

		sortCorners(corners, center);
		if (corners.size() == 0) {
			std::cout << "The corners were not sorted correctly!" << std::endl;
			return -1;
		}
		
		// compute four distances from pairs of points.
		int pointNum = corners.size();

		vector<Point2f> vertexByX;
		vector<Point2f> vertexByY;

		for (vector<Point2f>::iterator iter = corners.begin();
			iter != corners.end(); iter++) {
			vertexByX.push_back(*iter);
		}

		for (vector<Point2f>::iterator iter = corners.begin();
			iter != corners.end(); iter++) {
			vertexByY.push_back(*iter);
		}
		

		std::sort(vertexByX.begin(),vertexByX.end(),sortByX);
		std::sort(vertexByY.begin(),vertexByY.end(),sortByY);

		double sideAverRow = 0.0;
		double sideAverCol = 0.0;

		{
			Point2f ptX1 = vertexByX[0];
			Point2f ptX2 = vertexByX[1];
			Point2f ptX3 = vertexByX[2];
			Point2f ptX4 = vertexByX[3];

			double sideX1 = sqrt((ptX1.x - ptX2.x) * (ptX1.x - ptX2.x) +
				(ptX1.y - ptX2.y) * (ptX1.y - ptX2.y));
			double sideX2 = sqrt((ptX3.x - ptX4.x) * (ptX3.x - ptX4.x) +
				(ptX3.y - ptX4.y) * (ptX3.y - ptX4.y));
			sideAverRow = (sideX1 + sideX2) / 2;
		}

		{
			Point2f ptY1 = vertexByY[0];
			Point2f ptY2 = vertexByY[1];
			Point2f ptY3 = vertexByY[2];
			Point2f ptY4 = vertexByY[3];

			double sideY1 = sqrt((ptY1.x - ptY2.x) * (ptY1.x - ptY2.x) +
				(ptY1.y - ptY2.y) * (ptY1.y - ptY2.y));
			double sideY2 = sqrt((ptY3.x - ptY4.x) * (ptY3.x - ptY4.x) +
				(ptY3.y - ptY4.y) * (ptY3.y - ptY4.y));
			sideAverCol = (sideY1 + sideY2) / 2;
		}

		//double minSideLen = DBL_MAX;
		//double maxSideLen = 0.0;

		//for (int i = 0; i < pointNum; i++) {
		//	Point pt1 = corners[i % pointNum];
		//	Point pt2 = corners[(i + 1) % pointNum];
		//	// compute distance between points

		//	int x1 = pt1.x;
		//	int y1 = pt1.y;
		//	int x2 = pt2.x;
		//	int y2 = pt2.y;

		//	double curSideLen = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
		//	if (maxSideLen < curSideLen) {
		//		maxSideLen = curSideLen;
		//	}
		//	if (minSideLen > curSideLen) {
		//		minSideLen = curSideLen;
		//	}
		//}

		//double sideRate = maxSideLen / minSideLen;

		resMat = cv::Mat::zeros((int)sideAverRow, (int)sideAverCol, CV_8UC3);

		std::vector<cv::Point2f> quad_pts;
		quad_pts.push_back(cv::Point2f(0, 0));
		quad_pts.push_back(cv::Point2f(resMat.cols, 0));
		quad_pts.push_back(cv::Point2f(resMat.cols, resMat.rows));
		quad_pts.push_back(cv::Point2f(0, resMat.rows));

		cv::Mat transmtx = cv::getPerspectiveTransform(corners, quad_pts);
		cv::warpPerspective(inputMat, resMat, transmtx, resMat.size());

		return 0;
	}

	int QuadDetect::doPerspective(const string &filepath, vector<Point> &contour, Mat &resMat) {
		const Mat &inputMat = imread(filepath);
		return doPerspective(inputMat, contour, resMat);
	}
}