//////////////////////////////////////////////////////////////////////////
// File Name:		ParalLine.h
// File Function:	Common functions to processing paralle lines
//
//				Developed by: Yefeng Zheng
//				First Created: June 2002
//			University of Maryland, College Park
//					All Right Reserved
///////////////////////////////////////////////////////////////////////////

#ifndef __READ_WRITE_LINE_DETECT_H__
#define	__READ_WRITE_LINE_DETECT_H__
#include <iostream>
#include "convertMFC.h"

using namespace std;

struct LINE{
	DSCC_CPoint	StPnt;
	DSCC_CPoint	EdPnt;
	int		nType;
};

int ReadDetectResult(string sFileName, LINE *&pLine, int &nLine );
int WriteDetectResult(string sFileName, LINE *pLine, int nLine );
int	GetLineSkew( LINE *pLine, int nLine, double &nSkew );
int GetLineGap( LINE *pLine, int nLine, double &nLineGap );
int	RemoveVirtualLine( LINE *pLine, int &nLine );
int	RemoveBorderLine( LINE *pLine, int &nLine );
int	AdjustLinePosition( int w, int	h, LINE *pLine, int nLine );
int	GetHorLine( LINE *pLine, int nLine, LINE *&HorLine, int &nHorLine );
int GetVerLine( LINE *pLine, int nLine, LINE *&VerLine, int &nVerLine );

#endif//__READ_WRITE_LINE_DETECT_H__