//////////////////////////////////////////////////////////////////////////
// File Name:		GroupRunLength.h
// File Function:	Prototypes of functions which determine the end points
//					of a line.
//
//				Developed by: Yefeng Zheng
//				First Created: June 2002
//			University of Maryland, College Park
//					All Right Reserved
///////////////////////////////////////////////////////////////////////////

#ifndef	__GROUP_RUN_LENGTH_H__
#define __GROUP_RUN_LENGTH_H__

#include "ImageObj.h"
#include "ParalLine.h"

#define		REMOVE_OUTSIDE		0  //Outside of any character
#define		UNREMOVE_OUTSIDE    1  //Outside of any character and not used to compose the line
#define		REMOVE_INSIDE		2  //Inside a character, and removalable
#define		UNREMOVE_INSIDE		3  //Inside a character, and un-removalable

struct COLRUNLENGTH
{
	int		x;
	int		ys;
	int		ye;
	int	bUsed;
	int		nCC;
	int		nRemoveMode; //Take value of REMOVE_OUTSIDE, UNREMOVE_OUTSIDE, REMOVE_INSIDE, UNREMOVE_INSIDE
};

struct ROWRUNLENGTH
{
	int		y;
	int		xs;
	int		xe;
	int	bUsed;
	int		nCC;
	int		nRemoveMode;
};

int	GetColRunLength( CImage &Img, LINE Line, int nMaxSearchRange, COLRUNLENGTH *&ColRun, int &nRun);
int	GetRowRunLength( CImage &Img, LINE Line, int nMaxSearchRange, ROWRUNLENGTH *&RowRun, int &nRun);

int LocalFilter( LINE Line, COLRUNLENGTH *ColRun, int nRun );
int LocalFilter( LINE Line, ROWRUNLENGTH *RowRun, int nRun );

int AllocRunLength( LINE Line, COLRUNLENGTH *ColRun, int nRun );
int AllocRunLength( LINE Line, ROWRUNLENGTH *RowRun, int nRun );

int DumpRunLength( COLRUNLENGTH *ColRun, int nRun, CImage &ImgOut );
int	DumpLineImage( CImage &Img, LINE &Line, int nMaxSearchRange, CImage &ImgOut );

int GetEndPoint( LINE &Line,  COLRUNLENGTH *ColRun, int nRun );
int GetEndPoint( LINE &Line,  ROWRUNLENGTH *RowRun, int nRun );

int GetLineParam( COLRUNLENGTH *ColRun, int nRun, double &a, double &b );
int GetLineParam( ROWRUNLENGTH *RowRun, int nRun, double &a, double &b );

int	AdjustLines( CImage &Img, LINE *Line, int nLine, int nMaxSearchRange );

#endif // __GROUP_RUN_LENGTH_H__
