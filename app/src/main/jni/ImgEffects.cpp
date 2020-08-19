#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "ImgEffects.h"

using namespace cv;
using namespace std;

namespace nkocr {

	ImgEffects::ImgEffects() {
		// Parameters initialization
		// m_Param.m_blockSize = 601;
	}

	int ImgEffects::cvt2Gray(const string &filename, Mat &grayMat) {
		const Mat &inputMat = imread(filename);
		return cvt2Gray(inputMat, grayMat);
	}

	int ImgEffects::cvt2Gray(const Mat &inputMat, Mat &grayMat) {
		if (inputMat.channels() != 1) {
			cvtColor(inputMat, grayMat, CV_BGR2GRAY);
		}
		else {
			inputMat.copyTo(grayMat);
		}
		return 0;
	}

	int ImgEffects::cvt2BW(const string &filename, Mat &bwMat) {
		const Mat &inputMat = imread(filename);
		return cvt2BW(inputMat, bwMat);
	}

	int ImgEffects::cvt2BW(const Mat &inputMat, Mat &bwMat) {
		Mat grayMat;
		if (inputMat.channels() != 1) {
			cvtColor(inputMat, grayMat, CV_BGR2GRAY);
		}
		else {
			inputMat.copyTo(grayMat);
		}

		int blockSize = min(grayMat.rows, grayMat.cols);

		if (blockSize % 2 == 0)
			blockSize++;

		adaptiveThreshold(grayMat, bwMat, 255.0, CV_ADAPTIVE_THRESH_MEAN_C, 
			CV_THRESH_BINARY, blockSize, 0);

		return 0;
	}

	int ImgEffects::enhance(const string &filename, Mat &ehMat) {
		const Mat &inputMat = imread(filename);
		return enhance(inputMat, ehMat);
	}

	int ImgEffects::enhance(const Mat &inputMat, Mat &ehMat) {
		if (inputMat.channels() != 1)
		{
			vector<Mat> rgbMat;
			split(inputMat, rgbMat);

			vector<Mat> resRgbMat;
			for (int i = 0; i < rgbMat.size(); i++)	{
				Mat curChMat = rgbMat[i];
				Mat curResChMat = Mat::zeros(curChMat.size(), CV_8UC1);
				enhanceOneCh(curChMat, curResChMat);
				resRgbMat.push_back(curResChMat);
			}

			merge(resRgbMat, ehMat);
		}
		else {
			enhanceOneCh(inputMat, ehMat);
		}
		
		return 0;		
	}

	int ImgEffects::enhanceOneCh(const Mat& inputChMat, Mat& resChMat) {
		if (inputChMat.channels() != 1) {
			return -1;
		}

		int height = inputChMat.rows;
		int width = inputChMat.cols;
		int size = height * width;

		vector<float> histo, p;

		// initialization
		for (int i = 0; i < 256; i++) {
			histo.push_back(0);
			p.push_back(0);
		}

		for (int j = 0; j < height; j++) {
			const uchar *curLine = inputChMat.ptr<uchar>(j);
			for (int i = 0; i < width; i++) {
				uchar curPtVal = curLine[i];
				histo[curPtVal]++;
			}
		}


		for (int i = 0; i < 256; i++) {
			histo[i] /= size;
		}

		for (int i = 0; i < 256; i++) {
			for (int k = 0; k < i; k++) {
				p[i] += histo[k];
			}
		}

		resChMat = Mat::zeros(inputChMat.size(), CV_8UC1);
		for (int j = 0; j < height; j++) {
			const uchar *curLine = inputChMat.ptr<uchar>(j);
			uchar *curLineEh = resChMat.ptr<uchar>(j);
			for (int i = 0; i < width; i++) {
				uchar val = curLine[i];
				curLineEh[i] = (uchar)(p[val] * 255 + 0.5);
			}
		}

		return 0;
	}
}