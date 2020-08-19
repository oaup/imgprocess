#pragma once

#include "DirLine.h"
#include <iostream>
#include<vector>

class NkDirLine: public CDirLine{

public:
	int		m_nCharWidth;	//Character width estimated
	int		m_nCharHeight;  //Character height estimeated
//	int		m_charWidth[128];//Character width histogram, used to estimate character width
//	int		m_charHeight[128];//Character height histogram, used to estimate character height
	std::vector<int> m_charWidth;
	std::vector<int> m_charHeight;
public:
	int		detect(CImage &oriImage, const DSCC_RECT &rBoundRange);
	int		selectLines(CImage &oriImage, int minLen, double Q, int maxWidth);
	int		deleteSlantLine(double slantAngle);
	int     deleteShortLine( int length);
	int		mergeLine(int nFirst, int nSecond, int bDeleteSecond = TRUE);
	int		isFormedByStroke(CImage& oriImage, CHAINS chains);
	int	isStroke(CImage &oriImage, CHAINS chains);
	int		getAverCharWidth(int  bLastStrip);

};

class NkLinePara : public PARAMETER{

	public:
		int m_minVerLenth;
		int m_minHorLenth;
		int m_minRectPointDistance;//rect: the distance of two points

		//MergeFrameLine para_thresh
		int m_angleThresh1;//MergeFrameLine:two lines of Angle threshold(xs >= xe)
		int m_angleThresh2;//MergeFrameLine:two lines of Angle threshold(xs < xe)
		int m_nMidLinesThresh;//Frame Line numbers at one point in one direction
		int m_midlineDiffThresh;//The y distance between two lines in the mid x (x = min(Li.EdPnt.x,Lj.EdPnt.x),max(Li.StPnt.x,Lj.EdPnt.x))
public:
	NkLinePara();
};
//