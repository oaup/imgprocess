#ifndef _IMG_EFFECTS_H
#define _IMG_EFFECTS_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <string>

using namespace cv;
using namespace std;

namespace nkocr {
	class ImgEffectsParam {
	public:
		// int m_blockSize;
	};

	class ImgEffects {
	public:
		ImgEffects();

		int cvt2Gray(const string &filename, Mat &grayMat);
		int cvt2BW(const string &filename, Mat &bwMat);
		int enhance(const string &filename, Mat &ehMat);

		int cvt2Gray(const Mat &inputMat, Mat &grayMat);
		int cvt2BW(const Mat &inputMat, Mat &bwMat);
		int enhance(const Mat &inputMat, Mat &ehMat);
	private:
		int enhanceOneCh(const Mat& inputChMat, Mat& resChMat);
		// ImgEffectsParam m_Param;
	};
}

#endif // !_IMG_EFFECTS_H
