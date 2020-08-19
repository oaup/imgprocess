#include <algorithm>
#include "nkDirLine.h"

#define GAPBETWEENLINE  5
/****************************************************************************
/*	Name:		Detect
/*	Function:	Detect frame lines in the region, determinate how many strips
/*				used automatically.
/*	Parameter:	Image -- Form Image
/*				rBoundRange -- Image region to analyse
/*	return:		0 -- Succeed
/*				1 -- Error
/*
/***************************************************************************/

NkLinePara::NkLinePara(){
	ValleyGrayDepth = 30;
	//Merge Line para_thresh
	MinHorLineLength = 30;
	MinVerLineLength = 20;
	MaxLineWidth = 15;
	FilterSmallDSCC = TRUE;
	RLS = FALSE;
	bHasSlantLine = FALSE;
	MaxGap = 15;
	//Lines min length
	m_minHorLenth = 300;
	m_minVerLenth = 200;
	m_minRectPointDistance = 20;
	//MergeFrameLine para_thresh
	m_angleThresh1 = 2;
	m_angleThresh2 = 10;
	m_nMidLinesThresh = 2;
	m_midlineDiffThresh = 10;
	bSceneImg = 0;
}

int NkDirLine::detect(CImage &oriImage, const DSCC_RECT &rBoundRange){
	FreeMem();
	m_rcBoundRange = rBoundRange;
	DSCC_RECT rRange;
	int w = rBoundRange.right - rBoundRange.left + 1;
	int h = rBoundRange.bottom - rBoundRange.top + 1;
	int dw = 400;
	int dh = 400;
	int nStrip;
	if (!m_bIsHorLine) {
		nStrip = (rBoundRange.right - rBoundRange.left + dw - 1) / dw;//不明白什么意思ma
		rRange.top = rBoundRange.top;
		rRange.bottom = rBoundRange.bottom;
	} else {
		nStrip = (rBoundRange.bottom - rBoundRange.top + dh - 1) / dh;
		rRange.left = rBoundRange.left;
		rRange.right = rBoundRange.right;
	}
	m_nStrip =  nStrip;
	if (nStrip == 0){
		return -1;
	}
	m_pTree = (CConnTree**)malloc(nStrip*sizeof(CConnTree*));//用malloc应该不会调用构造函数ma.
	for (int i = 0; i<nStrip; i++) {
		m_pTree[i] = new CConnTree();
		m_pTree[i]->m_IsHorConn = m_bIsHorLine;
		m_nCurTree = i;
		if (!m_bIsHorLine) {
			rRange.left = rBoundRange.left + i*dw;
			rRange.right = rBoundRange.left + min((i + 1)*dw + 20, w - 1);
		} else {
			rRange.top = rBoundRange.top + i*dh;
			rRange.bottom = rBoundRange.top + min((i + 1)*dh + 20, h - 1);
		}


		BuildConnTree(oriImage, rRange);
		CalTree();
		m_nChainStart[i] = m_nChain;
		MergeChains();

		if ((m_bIsHorLine && m_Param.MinHorLineLength == 0) || (!m_bIsHorLine && m_Param.MinVerLineLength == 0)){
			if (getAverCharWidth(i == nStrip - 1) != 0){
				return -1;
			}
		}
		m_nOldChain = m_nChain;
	}
	/*	if( m_bIsHorLine )
	{
	StoreProject("E:\\Result\\Width.bmp", CharWidth, 150, 128);
	StoreProject("E:\\Result\\Height.bmp", CharHeight, 150, 128);
	}
	*/	
	
	if (m_Param.MinHorLineLength != 0){
		m_nCharWidth = m_Param.MinHorLineLength;
	}
	if (m_Param.MinVerLineLength != 0){
		m_nCharHeight = m_Param.MinVerLineLength;
	}

	if (m_bIsHorLine){
		if (selectLines(oriImage, max(m_nCharWidth, m_nCharHeight) + 5, 0.8, m_Param.MaxLineWidth) != 0) {
			return -1;
		}
	}else{
		if (selectLines(oriImage, max(m_nCharHeight, m_nCharWidth) + 5, 0.8, m_Param.MaxLineWidth) != 0) {
			return -1;
		}
	}
	/*
#ifdef _DEBUG
	if (m_bIsHorLine) {   //ma本来为E:\\Result\\HNodes.bmp
		DumpNodes("E:\\HNodes.bmp", Image.GetWidth() - 1, Image.GetHeight() - 1);
		DumpChain("E:\\HChain.bmp", Image.GetWidth() - 1, Image.GetHeight() - 1);
		DumpChains("E:\\HChains.bmp", Image.GetWidth() - 1, Image.GetHeight() - 1);
	} else {
		DumpNodes("E:\\VNodes.bmp", Image.GetWidth() - 1, Image.GetHeight() - 1);
		DumpChain("E:\\VChain.bmp", Image.GetWidth() - 1, Image.GetHeight() - 1);
		DumpChains("E:\\VChains.bmp", Image.GetWidth() - 1, Image.GetHeight() - 1);
	}
#endif
	*/
	return 0;
}
/****************************************************************************
/*	Name:		SelectLines
/*	Function:	Select frame lines from all candidates
/*	Parameter:	Image -- Form Image
/*				MinLen -- Minimum line length
/*				Q -- Minimum line quality
/*				MaxWidth -- Maximum line width
/*	return:		0 -- Succeed
/*				1 -- Error
/*
/***************************************************************************/
int NkDirLine::selectLines(CImage &oriImage, int minLen, double Q, int maxWidth) {
	if (m_nChains <= 0)
		return -1;

	static int MaxCnt = 50;
	if (m_nLine == 0)
		m_pLine = (FORMLINE *)malloc(sizeof(FORMLINE)*MaxCnt);
	if (m_pLine == NULL)	return -1;

	int i, j, k;
	for (i = 0; i<m_nChains; i++) {
		if (m_pChains[i].Width < maxWidth /*&& m_pChains[i].xe - m_pChains[i].xs>max((double)minLen, 10 * m_pChains[i].Width)*/ && ChainsQuality(m_pChains[i])>Q && !isStroke(oriImage, m_pChains[i])) {      //Commented 2000-3-1  S14.tif
			ChainsToFORMLINE(m_pLine[m_nLine], m_pChains[i]);
			m_pLine[m_nLine].nIndex = i;
			if (++m_nLine == MaxCnt) {
				MaxCnt += 50;
				m_pLine = (FORMLINE *)realloc(m_pLine, sizeof(FORMLINE)*MaxCnt);
				if (m_pLine == NULL)
					return -1;
			}
		}
	}

	for (i = 0; i<m_nLine; i++) {       //Delete some chains with too large angle
		if (m_bIsHorLine) {
			if (min(fabs(m_pLine[i].Angle), fabs(m_pLine[i].Angle + PI)) > PI / 4) {
				for (j = i; j<m_nLine - 1; j++)
					m_pLine[j] = m_pLine[j + 1];
				m_nLine--;
				i--;
			}
		} else {
			if (min(fabs(m_pLine[i].Angle + PI / 2), fabs(m_pLine[i].Angle - PI / 2)) > PI / 4) {
				for (j = i; j < m_nLine - 1; j++)
					m_pLine[j] = m_pLine[j + 1];
				m_nLine--;
				i--;
			}
		}
	}

	int l;
	int GAP = 3;
	if (m_bIsHorLine) {	            //Delete repeated lines
		for (i = 0; i<m_nLine - 1; i++) {
			for (j = i + 1; j<m_nLine; j++) {
				if (fabs(m_pLine[i].Angle - m_pLine[j].Angle)>1.5*PI / 180)	continue;
				int xs = max(m_pLine[i].StPnt.x, m_pLine[j].StPnt.x);
				int xe = min(m_pLine[i].EdPnt.x, m_pLine[j].EdPnt.x);
				if (xs > xe - GAPBETWEENLINE)	continue;
				double w = max(m_pLine[i].Width, m_pLine[j].Width) / 2;
				l = -1;
				if (m_pLine[i].StPnt.x < m_pLine[j].StPnt.x + GAP && m_pLine[i].EdPnt.x > m_pLine[j].EdPnt.x - GAP
					&& fabs(fYofLine(m_pLine[i], m_pLine[j].StPnt.x) - m_pLine[j].StPnt.y) < w + GAP
					&& fabs(fYofLine(m_pLine[i], m_pLine[j].EdPnt.x) - m_pLine[j].StPnt.y) < w + GAP)
					l = i;
				else if (m_pLine[j].StPnt.x < m_pLine[i].StPnt.x + GAP && m_pLine[j].EdPnt.x > m_pLine[i].EdPnt.x - GAP
					&& fabs(fYofLine(m_pLine[j], m_pLine[i].StPnt.x) - m_pLine[i].StPnt.y) < w + GAP
					&& fabs(fYofLine(m_pLine[j], m_pLine[i].EdPnt.x) - m_pLine[i].EdPnt.y) < w + GAP)
					l = j;
				if (l>0) {
					if (l != i)			
						m_pLine[i] = m_pLine[l];
					for (k = j; k < m_nLine - 1; k++)
						m_pLine[k] = m_pLine[k + 1];
					m_nLine--;
					j = i;
				} else {
					if (xs == m_pLine[i].StPnt.x) {
						if (fabs(fYofLine(m_pLine[j], xs) - m_pLine[i].StPnt.y) >= w + GAP)	continue;
						if (fabs(fYofLine(m_pLine[i], xe) - m_pLine[j].EdPnt.y) >= w + GAP)	continue;
					} else {
						if (fabs(fYofLine(m_pLine[i], xs) - m_pLine[j].StPnt.y) >= w + GAP)	continue;
						if (fabs(fYofLine(m_pLine[j], xe) - m_pLine[i].EdPnt.y) >= w + GAP)	continue;
					}
					mergeLine(i, j);
					j = i;
				}
			}
		}
	} else {
		for (i = 0; i<m_nLine - 1; i++) {
			for (j = i + 1; j<m_nLine; j++) {
				if (fabs(m_pLine[i].Angle - m_pLine[j].Angle)>1.5*PI / 180)	continue;
				int ys = max(m_pLine[i].StPnt.y, m_pLine[j].StPnt.y);
				int ye = min(m_pLine[i].EdPnt.y, m_pLine[j].EdPnt.y);
				if (ys > ye - GAPBETWEENLINE)	continue;
				double w = max(m_pLine[i].Width, m_pLine[j].Width);
				l = -1;
				if (m_pLine[i].StPnt.y < m_pLine[j].StPnt.y + GAP && m_pLine[i].EdPnt.y > m_pLine[j].EdPnt.y - GAP
					&& fabs(fYofLine(m_pLine[i], m_pLine[j].StPnt.y) - m_pLine[j].StPnt.x) <= w + GAP
					&& fabs(fYofLine(m_pLine[i], m_pLine[j].EdPnt.y) - m_pLine[j].EdPnt.x) <= w + GAP)
					l = i;
				else if (m_pLine[j].StPnt.y < m_pLine[i].StPnt.y + GAP && m_pLine[j].EdPnt.y > m_pLine[i].EdPnt.y - GAP
					&& fabs(fYofLine(m_pLine[j], m_pLine[i].StPnt.y) - m_pLine[i].StPnt.x) <= w + GAP
					&& fabs(fYofLine(m_pLine[j], m_pLine[i].EdPnt.y) - m_pLine[i].EdPnt.x) <= w + GAP)
					l = j;
				if (l > 0) {
					if (l != i)		m_pLine[i] = m_pLine[l];
					for (k = j; k < m_nLine - 1; k++)
						m_pLine[k] = m_pLine[k + 1];
					m_nLine--;
					j = i;
				} else {
					if (ys == m_pLine[i].StPnt.y) {
						if (fabs(fYofLine(m_pLine[j], ys) - m_pLine[i].StPnt.x) >= w + GAP)	continue;
						if (fabs(fYofLine(m_pLine[i], ye) - m_pLine[j].EdPnt.x) >= w + GAP)	continue;
					} else {
						if (fabs(fYofLine(m_pLine[i], ys) - m_pLine[j].StPnt.x) >= w + GAP)		continue;
						if (fabs(fYofLine(m_pLine[j], ye) - m_pLine[i].EdPnt.x) >= w + GAP)		continue;
					}
					mergeLine(i, j);
					j = i;
				}
			}
		}
	}
	
	FORMLINE	tmpLine;
	for (i = 0; i < m_nLine - 1; i++){
		k = i;
		for (j = i + 1; j < m_nLine; j++) {
			if (m_bIsHorLine) {
				if (m_pLine[j].StPnt.y < m_pLine[k].StPnt.y)			
					k = j;
			} else {
				if (m_pLine[j].StPnt.x < m_pLine[k].StPnt.x)			
					k = j;
			}
		}
		if (k != i) {
			tmpLine = m_pLine[i];
			m_pLine[i] = m_pLine[k];
			m_pLine[k] = tmpLine;
		}
	}
	return 0;
}
/***************************************************************************************
/*	Name:		DeleteSlantLine
/*	Function:	Delete all slant lines if we can there are no slant lines on the form
/*	Parameter:	SlantAngle -- Skew angle of the form image
/*	return:		0 -- Succeed
/*				1 -- Error
/*
/**************************************************************************************/
int NkDirLine::deleteSlantLine(double slantAngle) {
	int	i, j;
	if (m_bIsHorLine) {
		i = 0;
		while (i<m_nLine) {
			if (fabs(m_pLine[i].Angle - slantAngle) > 7 * PI / 180) {
				for (j = i; j < m_nLine - 1; j++)
					m_pLine[j] = m_pLine[j + 1];
				m_nLine--;
			} else
				i++;
		}
	} else {
		i = 0;
		while (i<m_nLine) {
			if (fabs(m_pLine[i].Angle + PI / 2 - slantAngle) > 7 * PI / 180) {
				for (j = i; j < m_nLine - 1; j++)
					m_pLine[j] = m_pLine[j + 1];
				m_nLine--;
			} else
				i++;
		}
	}
	return 0;
}
/***************************************************************************************
/*	Name:		DeleteshortLine
/*	Function:	Delete Lines that the length is not enough
/*	Parameter:	length-- The length of the threshold
/*	return:		0 -- Succeed
/*				1 -- Error
/*
/**************************************************************************************/
int  NkDirLine::deleteShortLine(int length){
	int	i, j , LineHorLength,LineVerLength;
	if (m_bIsHorLine) {
		i = 0;
		while (i<m_nLine) {
			LineHorLength = m_pLine[i].EdPnt.x - m_pLine[i].StPnt.x;

			if (LineHorLength < length) {
				for (j = i; j < m_nLine - 1; j++)
					m_pLine[j] = m_pLine[j + 1];
				m_nLine--;
			}
			else
				i++;
		}
	}
	else {
		i = 0;
		while (i<m_nLine) {
			LineVerLength = m_pLine[i].EdPnt.y - m_pLine[i].StPnt.y;

			if (LineVerLength < length) {
				for (j = i; j < m_nLine - 1; j++)
					m_pLine[j] = m_pLine[j + 1];
				m_nLine--;
			}
			else
				i++;
		}
	}
	return 0;
}
/**********************************************************************************
/*	Name:		MergeLine
/*	Function:	Merge frame lines
/*	Parameter:	nFirst -- First frame line
/*				nSecond -- Second frame line
/*				bDeleteSecond -- TRUE  Delete the second line after merged
/*								 FALSE Do not delete the second line
/*	return:		0 -- Succeed
/*				1 -- Error
/*
/*********************************************************************************/
int NkDirLine::mergeLine(int nFirst, int nSecond, int bDeleteSecond) {
	if (nFirst < 0 || nFirst >= m_nLine || nSecond < 0 || nSecond >= m_nLine)
		return -1;
	int	xs, xe, ys, ye;
	if (m_pLine[nFirst].nIndex < 0 || m_pLine[nSecond].nIndex < 0) {
		if ((m_pLine[nFirst].nIndex < 0 && m_pLine[nFirst].nStyle != 3) ||
			(m_pLine[nSecond].nIndex < 0 && m_pLine[nSecond].nStyle != 3))
			return -1;
		if (m_bIsHorLine) {
			xs = min(m_pLine[nFirst].StPnt.x, m_pLine[nSecond].StPnt.x);
			xe = max(m_pLine[nFirst].EdPnt.x, m_pLine[nSecond].EdPnt.x);
			if (xs == m_pLine[nFirst].StPnt.x)
				ys = m_pLine[nFirst].StPnt.y;
			else
				ys = m_pLine[nSecond].StPnt.y;
			if (xe == m_pLine[nFirst].EdPnt.x)
				ye = m_pLine[nFirst].EdPnt.y;
			else
				ye = m_pLine[nSecond].EdPnt.y;
		} else {
			ys = min(m_pLine[nFirst].StPnt.y, m_pLine[nSecond].StPnt.y);
			ye = max(m_pLine[nFirst].EdPnt.y, m_pLine[nSecond].EdPnt.y);
			if (ys = m_pLine[nFirst].StPnt.y)
				xs = m_pLine[nFirst].StPnt.x;
			else
				xs = m_pLine[nSecond].StPnt.x;
			if (ye == m_pLine[nFirst].EdPnt.y)
				xe = m_pLine[nFirst].EdPnt.x;
			else
				xe = m_pLine[nSecond].EdPnt.x;
		}
		m_pLine[nFirst].StPnt = DSCC_CPoint(xs, ys);
		m_pLine[nFirst].EdPnt = DSCC_CPoint(xe, ye);
		m_pLine[nFirst].Angle = GetAngle(DSCC_CPoint(xs, ys), DSCC_CPoint(xe, ye));
		m_pLine[nFirst].Width = max(m_pLine[nFirst].Width, m_pLine[nSecond].Width);
		m_pLine[nFirst].Q = min(m_pLine[nFirst].Q, m_pLine[nSecond].Q);
		if (m_pLine[nFirst].nIndex < 0 && m_pLine[nFirst].nStyle == 3) {//the first line is virtual line
			m_pLine[nFirst].nIndex = m_pLine[nSecond].nIndex;
			m_pLine[nFirst].nStyle = m_pLine[nSecond].nStyle;
		}
	}
	if (m_bIsHorLine) {
		xs = min(m_pLine[nFirst].EdPnt.x, m_pLine[nSecond].EdPnt.x);
		xe = max(m_pLine[nFirst].StPnt.x, m_pLine[nSecond].StPnt.x);
		if (xs == m_pLine[nFirst].EdPnt.x && xe == m_pLine[nFirst].StPnt.x)//Totally overlap
			m_pLine[nFirst] = m_pLine[nSecond];
		else if (xs != m_pLine[nSecond].EdPnt.x || xe != m_pLine[nSecond].StPnt.x)
			AddChain(m_pLine[nFirst].nIndex, m_pChains[m_pLine[nSecond].nIndex].pHead);
		ChainsToFORMLINE(m_pLine[nFirst], m_pChains[m_pLine[nFirst].nIndex]);
	} else {
		ys = min(m_pLine[nFirst].EdPnt.y, m_pLine[nSecond].EdPnt.y);
		ye = max(m_pLine[nFirst].StPnt.y, m_pLine[nSecond].StPnt.y);
		if (ys == m_pLine[nFirst].EdPnt.y && ye == m_pLine[nFirst].StPnt.y)//Totally overlap
			m_pLine[nFirst] = m_pLine[nSecond];
		else if (ys != m_pLine[nSecond].EdPnt.y || ye != m_pLine[nSecond].StPnt.y) {
			AddChain(m_pLine[nFirst].nIndex, m_pChains[m_pLine[nSecond].nIndex].pHead);
			ChainsToFORMLINE(m_pLine[nFirst], m_pChains[m_pLine[nFirst].nIndex]);
		}
	}
	if (bDeleteSecond) {
		for (int i = nSecond; i < m_nLine - 1; i++)
			m_pLine[i] = m_pLine[i + 1];
		m_nLine--;
	}
	return 0;
}
/**********************************************************************************
/*	Name:		IsStroke
/*	Function:	Test if the CHAINS is formed by strokes of characters
/*	Parameter:	Image -- Form Image
/*				Chains -- CHAINS to analyse
/*	return:		TRUE -- CHAINS is formed by strokes
/*				FALSE -- CHAINS is not formed by strokes
/*
/*********************************************************************************/
int NkDirLine::isStroke(CImage &oriImage, CHAINS chains) {
	int Len;
	if (m_bIsHorLine)	
		Len = m_nCharWidth * 5 / 2;
	else				
		Len = m_nCharHeight * 5 / 2;
	if (chains.Num > 2 * Len)	
		return FALSE;
	if (chains.Num < 0.9*(chains.xe - chains.xs) && isFormedByStroke(oriImage, chains))
		return TRUE;

	int nChain = chains.pHead;
	int nTree = InWhichTree(nChain);
	ConnComp cc;
	while (nChain >= 0) {
		if (m_pChain[nChain].Num >= 10) {
			if (m_pTree[nTree]->GetConnComp(cc, m_pChain[nChain].pHead))
				return FALSE;
			int width = cc.rcBound.right - cc.rcBound.left + 1;
			int height = cc.rcBound.bottom - cc.rcBound.top + 1;
			//			if(width > 128 || height > 128)
			if (width > 80 || height > 80)
				return FALSE;
			if (width > 15 && width < 3 * height && cc.nPixelsNum > /*0.08*/0.15*width*height)//width is the length of the line, height is the width of the line
				return TRUE;
		}
		nChain = m_pChain[nChain].pRight;
	}
	return FALSE;
}
/**********************************************************************************
/*	Name:		IsFormedByStroke
/*	Function:	Test if the CHAINS is formed by a stroke of a character
/*				Called by IsStroke()
/*	Parameter:	Image -- Form Image
/*				Chains -- CHAINS to analyse
/*	return:		TRUE -- CHAINS is formed by a stroke
/*				FALSE -- CHAINS is not formed by a stroke
/*
/*********************************************************************************/
int NkDirLine::isFormedByStroke(CImage& oriImage, CHAINS chains) {
	DSCC_BYTE *p = oriImage.LockRawImg();
	int	  w = oriImage.GetWidth();
	int   h = oriImage.GetHeight();
	int	  wb = oriImage.GetLineSizeInBytes();
	int  bUpSideDown = oriImage.GetOrientation();
	int	  WIDTH = 10;
	int	  SPACE = 5;
	int   Proj, Proj2, i, j, k, WW, BB;
	DSCC_BYTE mask[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };
	WW = 0;		
	BB = 0;

	if (m_bIsHorLine) {
		for (i = chains.xs; i <= chains.xe; i++) {
			Proj = Proj2 = 0;
			j = (int)fYofChains(chains, i);
			j = min(h - 1, j);
			for (k = (int)max((double)0, j - chains.Width - SPACE); k > (int)max(j - chains.Width - WIDTH, (double)0); k--) {
				if (bUpSideDown) {
					if (h - 1 - k >= 0) {
						if (*(p + (h - 1 - k)*wb + i / 8)&mask[i % 8])//bug	
							Proj++;
					}
				} else {
					if (*(p + k*wb + i / 8)&mask[i % 8])
						Proj++;
				}
			}
			for (k = (int)min((double)h - 1, j + chains.Width + SPACE); k < (int)min(j + chains.Width + WIDTH, (double)h - 1); k++) {
				if (bUpSideDown) {
					if (h - 1 - k >= 0) {
						if (*(p + (h - 1 - k)*wb + i / 8)&mask[i % 8])
							Proj++;
					}
				} else {
					if (*(p + k*wb + i / 8)&mask[i % 8])
						Proj++;
				}
			}
			for (k = (int)max((double)0, j - (chains.Width + SPACE) / 2); k <= (int)min((double)h - 1, j + (chains.Width + SPACE) / 2); k++) {
				if (bUpSideDown) {
					if (*(p + (h - 1 - k)*wb + i / 8)&mask[i % 8])
						Proj2++;
				} else {
					if (*(p + k*wb + i / 8)&mask[i % 8])
						Proj2++;
				}
			}
			if (Proj == 0 && Proj2 == 0)		
				WW++;
			if (Proj > 0 && Proj2 > 0)		
				BB++;
		}
		oriImage.UnlockRawImg();
		if (3 * WW + 2 * BB < 2 * (chains.xe - chains.xs))
			return 0;
		else
			return 1;
	} else {
		for (j = chains.xs; j <= chains.xe; j++) {
			Proj = Proj2 = 0;
			i = (int)fYofChains(chains, j);
			i = min(w - 1, i);
			for (k = (int)max((double)0, i - chains.Width - SPACE); k>(int)max((double)0, i - chains.Width - WIDTH); k--)
				if (*(p + (h - 1 - j)*wb + k / 8)&mask[k % 8])
					Proj++;
			for (k = (int)min((double)w - 1, i + chains.Width + SPACE); k<(int)min((double)w - 1, i + chains.Width + WIDTH); k++)
				if (*(p + (h - 1 - j)*wb + k / 8)&mask[k % 8])
					Proj++;
			for (k = (int)max((double)0, i - (chains.Width + 1) / 2 - SPACE / 2); k <= (int)min((double)w - 1, i + (chains.Width + 1) / 2 + SPACE / 2); k++)
				if (*(p + (h - 1 - j)*wb + k / 8)&mask[k % 8])
					Proj2++;
			if (Proj == 0 && Proj2 == 0)		
				WW++;
			if (Proj >0 && Proj2 > 0)		
				BB++;
		}
		oriImage.UnlockRawImg();
		if (3 * WW + 2 * BB < 2 * (chains.xe - chains.xs))
			return 0;
		else
			return 1;
	}
	return 0;
}

/**********************************************************************************
/*	Name:		GetAverCharWidth
/*	Function:	Estimate the character width
/*	Parameter:	bLastStrip -- TRUE  Make the final decision no matter if there are
/*									enough connected components
/*							  FALSE Do not make the decision if lacking enough
/*									connected components
/*	return:		0  -- Succeed
/*				-1 -- Error
/*
/*********************************************************************************/
int NkDirLine::getAverCharWidth(int bLastStrip) {
	if (m_nCharWidth > 0)	
		return 0;
//	memset(m_charWidth, 0, sizeof(m_charWidth));
//	memset(m_charHeight, 0, sizeof(m_charHeight));
	m_charHeight.clear();
	m_charWidth.clear();
	m_pTree[m_nCurTree]->GetAllConnComps();
	int j = 0;
	for (; j<m_pTree[m_nCurTree]->m_nTotalConnComps; j++) {//Calculate histogram	
		DSCC_RECT rcBound = m_pTree[m_nCurTree]->m_pConnComp[j].rcBound;
		int  w = rcBound.right - rcBound.left;
		int  h = rcBound.bottom - rcBound.top;
		if (w >= 128 || h >= 128)	
			continue;
		if (w>15)	
			m_charWidth[w]++;
		if (h > 15)	
			m_charHeight[h]++;
	}
	int W[5], H[5], numW, numH, MaxW, MaxH;
	j = 125;		
	numW = 0;		
	MaxW = 125;
	while (j > 15) {//Find peaks in width histogram	
		if (m_charWidth[j]<10) {
			j--;		
			continue;
		}
		while (m_charWidth[j - 1] > m_charWidth[j] && j > 15)	
			j--;
		W[numW] = j;
		if (m_charWidth[W[numW]] > m_charWidth[MaxW])		
			MaxW = j;
		while (m_charWidth[j - 1] <= m_charWidth[j] && j > 15)	
			j--;
		numW++;
		if (numW >= 5)	
			break;
	}

	j = 125;		
	numH = 0;		
	MaxH = 125;
	while (j > 15) {//Find peaks in height histogram
		if (m_charHeight[j]<10) {
			j--;			
			continue;
		}
		while (m_charHeight[j - 1] > m_charHeight[j] && j > 15)	
			j--;
		H[numH] = j;
		if (m_charHeight[H[numH]] > m_charHeight[MaxH])		
			MaxH = j;
		while (m_charHeight[j - 1] <= m_charHeight[j] && j > 15)	
			j--;
		numH++;
		if (numH >= 5)	
			break;
	}

	if (m_nCurTree >= 2 && m_charWidth[MaxW] > 15 && m_charHeight[MaxH] > 15) {
		//Peak is high enough
		for (j = 0; j<numW; j++) {
			//Multi-Peak select the peak with large value
			if (m_charWidth[W[j]] > m_charWidth[MaxW] / 3)		
				break;
		}
		m_nCharWidth = W[j];
		for (j = 0; j<numH; j++) {
			if (m_charHeight[H[j]] > m_charHeight[MaxH] / 3)	
				break;
		}
		m_nCharHeight = H[j];
	} else if (bLastStrip) {
		//Peak is not high enough 
		if (m_charWidth[MaxW - 2] + m_charWidth[MaxW - 1] + m_charWidth[MaxW] + m_charWidth[MaxW + 1] + m_charWidth[MaxW + 2] > 30
			&& m_charHeight[MaxH - 2] + m_charHeight[MaxH - 1] + m_charHeight[MaxH] + m_charHeight[MaxH + 1] + m_charHeight[MaxH + 2] > 30) {
			m_nCharWidth = MaxW;
			m_nCharHeight = MaxH;
		} else {
			m_nCharWidth = 40;
			m_nCharHeight = 20;
		}
	}
	return 0;
}


