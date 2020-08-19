//////////////////////////////////////////////////////////////////////////
// File Name:		ReadWriteHMM.h
// File Function:	Prototypes of the functions related to HMM model 
//					reading and writing
//
//				Developed by: Yefeng Zheng
//				First Created: June 2003
//			University of Maryland, College Park
//					All Right Reserved
///////////////////////////////////////////////////////////////////////////

#ifndef	__READ_WRITE_HMM_H__
#define	__READ_WRITE_HMM_H__
#include "convertMFC.h"
#include <string.h>
#define	MAXLINEGAP		50	//Maximum variation of line gaps

struct LINE_GAP
{
	int		nMeanGap;			//Mean of the gaps
	int		nMinGap;			//Minimal the gaps
	int		nMaxGap;			//Maximal the gaps
	double	nProb[MAXLINEGAP];	//Probability of line gap
};

struct CROSS
{
	int		nStLine;	// For a horizontal line, nLine1 is the vertical line that crosses it at left
						// For a vertical line, nLine1 is the horizontal line that crosses it at top
	DSCC_CPoint	StOffset;	// Offset of the left or top end point to the crossing point
	int		nEdLine;	// For a horizontal line, nLine1 is the vertical line that crosses it at right
						// For a vertical line, nLine1 is the horizontal line that crosses it at bottom
	DSCC_CPoint	EdOffset;	// Offset of the right or bottom end point to the crossing point
};

struct HMM_MODEL
{
	int		nHorLine;		//Number of horizontal lines each image
	int		nVerLine;		//Number of vertical lines each image
	double	*HLineLenMean;	//Horizontal line projection mean value
	double	*HLineLenVar;	//Horizontal line projection variance
	double  HTextLenMean;	//Horizonal text projection mean value
	double	HTextLenVar;	//Horizontal text projection variance
	LINE_GAP *HLineGapDist; //Line gap distribution

	double	*VLineLenMean;	//Vertical line projection mean value
	double	*VLineLenVar;	//Vertical line projection variance
	double	VTextLenMean;	//Vertical text projection mean value
	double	VTextLenVar;	//Vertical text projection variance
	LINE_GAP *VLineGapDist;	//Line gap distribution

	CROSS	*HCross;		//Crossing of starting and ending points of horizontal lines
	CROSS	*VCross;		//Crossing of starting and ending points of vertical lines
};

int AllocHMMModel( HMM_MODEL &Model);
int FreeHMMModel( HMM_MODEL &Model );

#endif //__READ_WRITE_HMM_H__