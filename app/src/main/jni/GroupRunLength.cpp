/*
Copyright (C) 2004	Yefeng Zheng

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You are free to use this program for non-commercial purpose.              
If you plan to use this code in commercial applications, 
you need additional licensing: 
please contact zhengyf@cfar.umd.edu
*/

////////////////////////////////////////////////////////////////////////////////////////////////
// File NAME:		GroupRunLength.cpp
// File Function:	1. Grouping vertical runlength belonging to a line
//					2. Creating polyline representation of a real line
//
//				Developed by: Yefeng Zheng
//			   First created: Feb. 2003
//			University of Maryland, College Park
///////////////////////////////////////////////////////////////////////////////////////////////////


#include "ImageObj.h"
#include "ImageTool.h"
#include "tools.h"
#include "ParalLine.h"
#include "GroupRunLength.h"
#include <math.h>
#include <algorithm>

#define		MIN_PIXEL	20

/******************************************************************************
/*	Name:		AdjustLines
/*	Function:	Adjust lines using the pixel level information
/*	Parameter:	Img				-- Image
/*				Line			-- Lines detected, they are adjust after calling this function
/*				nLine			-- The number of lines
/*				nMaxSearchRange -- Maximum search range in pixels
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int	AdjustLines( CImage &Img, LINE *Line, int nLine, int nMaxSearchRange )
{
	int		w = Img.GetWidth();
	int		h = Img.GetHeight();
	int		wb = Img.GetLineSizeInBytes( );
	DSCC_BYTE*	pImg = Img.LockRawImg( );
	COLRUNLENGTH *ColRun;
	ROWRUNLENGTH *RowRun;
	int			 nRun;

	for( int i=0; i<nLine; i++ )
	{
		int h = fabs( (float)(Line[i].EdPnt.y - Line[i].StPnt.y) );
		int w = fabs((float)(Line[i].EdPnt.x - Line[i].StPnt.x));
		if( h < w )
		{//Horizontal lines
			GetColRunLength( Img, Line[i], nMaxSearchRange, ColRun, nRun);
			AllocRunLength( Line[i], ColRun, nRun );
			LocalFilter( Line[i], ColRun, nRun );
			GetEndPoint( Line[i], ColRun, nRun );
			free( ColRun );
		}
		else if( h > w )
		{//Vertical lines
			GetRowRunLength( Img, Line[i], nMaxSearchRange, RowRun, nRun );
			AllocRunLength( Line[i], RowRun, nRun );
			LocalFilter( Line[i], RowRun, nRun );
			GetEndPoint( Line[i], RowRun, nRun );
			free( RowRun );
		}
	}
	Img.UnlockRawImg();
	return 0;
}

/******************************************************************************
/*	Name:		GetLineParam
/*	Function:	Get MMSE estimation of line parameters from the run length 
/*				composing the line. y = b*x + a
/*	Parameter:	ColRun	-- Column (vertical) run length
/*				nRun	-- Number of run length
/*				a		-- Parameter a
/*				b		-- Parameter b
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int GetLineParam( COLRUNLENGTH *ColRun, int nRun, double &a, double &b )
{
	double	SumX, SumY, SumXX, SumXY;
	int		N;
//Get MMSE approximate of a line
	N = 0;
	SumX = SumY = SumXX = SumXY = 0;
	int i = 0;
	for(; i<nRun; i++ )
	{
		if( ColRun[i].bUsed == FALSE )	continue;
		SumX += ColRun[i].x;
		SumY += (ColRun[i].ys+ColRun[i].ye)*0.5;
		N++;
	}
	if( N<=0 )	return 0;

	double	ax = SumX/N;
	double  ay = SumY/N;

	for( i=0; i<nRun; i++ )
	{
		if( ColRun[i].bUsed == FALSE )	continue;
		SumXX += (ColRun[i].x - ax)*(ColRun[i].x - ax);
		SumXY += (ColRun[i].x - ax)*( 0.5*(ColRun[i].ys + ColRun[i].ye ) - ay);
	}
	b = SumXY / SumXX;
	a = ay - b*ax;
	return 0;
}

/******************************************************************************
/*	Name:		GetLineParam
/*	Function:	Get MMSE estimation of line parameters from the run length 
/*				composing the line. x = b*y + a
/*	Parameter:	RowRun	-- Row (horizontal) run length
/*				nRun	-- Number of run length
/*				a		-- Parameter a
/*				b		-- Parameter b
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int GetLineParam( ROWRUNLENGTH *RowRun, int nRun, double &a, double &b )
{
	double	SumX, SumY, SumXY, SumYY;
	int		N;
//Get MMSE approximate of a line
	N = 0;
	SumX = SumY = SumYY = SumXY = 0;
	int i = 0;
	for(; i<nRun; i++ )
	{
		if( RowRun[i].bUsed == FALSE )	continue;
		SumX += (RowRun[i].xs+RowRun[i].xe)*0.5;
		SumY += RowRun[i].y;
		N++;
	}
	if( N<=0 )	return 0;

	double	ax = SumX/N;
	double  ay = SumY/N;

	for( i=0; i<nRun; i++ )
	{
		if( RowRun[i].bUsed == FALSE )	continue;
		SumYY += (RowRun[i].y - ay)*(RowRun[i].y - ay);
		SumXY += (RowRun[i].y - ay)*( 0.5*(RowRun[i].xs + RowRun[i].xe ) - ax);
	}
	b = SumXY / SumYY;
	a = ax - b*ay;
	return 0;
}

/******************************************************************************
/*	Name:		GetEndPoint
/*	Function:	Get two end points of a line.
/*	Parameter:	Line	-- A line
/*				ColRun	-- Column runlength
/*				nRun	-- Number of runlength
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int GetEndPoint( LINE &Line, COLRUNLENGTH *ColRun, int nRun )
{
	int nLeftCount=0;
	int nRightCount=0; 
	int	nLineRun = 0;

	//Test if the line is a virtual line
	int i, j;
	for( i=0; i<nRun; i++ )
	{
		if( ColRun[i].bUsed == FALSE )	continue;
		nLineRun ++;
	}
	if( nLineRun <= MIN_PIXEL )
		return -1;

	//Get MMSE approximate esimate of line parameters
	double a, b;
	GetLineParam( ColRun, nRun, a, b );
	double nSkewVar = fabs(atan(b) - atan2( (double)(Line.EdPnt.y-Line.StPnt.y), (double)(Line.EdPnt.x - Line.StPnt.x) ) ) * 180/PI;
	if( nSkewVar > 2.5 )	//Skew variance is too large
		return -1;
	//Search left end point
	for( i=0; i<nRun; i++ )
	{
		if( ColRun[i].bUsed == FALSE )	continue;
		nLeftCount++;
		if( nLeftCount >= 3 )
		{
			for(j=i; j>=0; j--)
			{
				if( ColRun[i].bUsed == FALSE )
					continue;
				if( abs(ColRun[j].x - ColRun[i].x)>6 )
					break;
			}
			j = max( 0, j );
			Line.StPnt.x = ColRun[j].x;
			Line.StPnt.y = (int)(a + b*ColRun[j].x );
			break;
		}
	}

//Search right end point
	for( i=nRun-1; i>=0; i-- )
	{
		if( ColRun[i].bUsed == FALSE )	continue;
		nRightCount++;
		if( nRightCount >= 3 )
		{
			for( j=i; j<nRun; j++ )
			{
				if( ColRun[j].bUsed == FALSE )	
					continue;
				if( abs(ColRun[j].x - ColRun[i].x ) > 6 )
					break;
			}
			j = min( nRun-1, j);
			Line.EdPnt.x = ColRun[j].x;
			Line.EdPnt.y = (int)(a + b*ColRun[j].x );
			break;
		}
	}
	return 0;
}

/******************************************************************************
/*	Name:		GetEndPoint
/*	Function:	Get two end points of a line.
/*	Parameter:	Line	-- A line
/*				RowRun	-- Column runlength
/*				nRun	-- Number of runlength
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int GetEndPoint( LINE &Line, ROWRUNLENGTH *RowRun, int nRun )
{
	int nLeftCount=0;
	int nRightCount=0; 
	int	nLineRun = 0;

	//Test if the line is a virtual line
	int i, j;
	for( i=0; i<nRun; i++ )
	{
		if( RowRun[i].bUsed == FALSE )	continue;
		nLineRun ++;
	}
	if( nLineRun <= MIN_PIXEL )
		return -1;

	//Get MMSE approximate esimate of line parameters
	double a, b;
	GetLineParam( RowRun, nRun, a, b );
	double nAngle = atan2( (double)(Line.EdPnt.x-Line.StPnt.x), (double)(Line.EdPnt.y - Line.StPnt.y) );
	double nSkewVar = fabs(atan(b) - nAngle ) * 180/PI;
	int  nLen = fabs((float)(Line.EdPnt.y - Line.StPnt.y));
	if( nLen > 300 && nSkewVar > 2.5 )//Skew variance is too large
		return -1;
	//Search left end point
	for( i=0; i<nRun; i++ )
	{
		if( RowRun[i].bUsed == FALSE )	continue;
		nLeftCount++;
		if( nLeftCount >= 3 )
		{
			for(j=i; j>=0; j--)
			{
				if( RowRun[i].bUsed == FALSE )
					continue;
				if( abs(RowRun[j].y - RowRun[i].y)>6 )
					break;
			}
			j = max( 0, j );
			Line.StPnt.y = RowRun[j].y;
			Line.StPnt.x = (int)(a + b*RowRun[j].y );
			break;
		}
	}

//Search right end point
	for( i=nRun-1; i>=0; i-- )
	{
		if( RowRun[i].bUsed == FALSE )	continue;
		nRightCount++;
		if( nRightCount >= 3 )
		{
			for( j=i; j<nRun; j++ )
			{
				if( RowRun[j].bUsed == FALSE )	
					continue;
				if( abs(RowRun[j].y - RowRun[i].y ) > 6 )
					break;
			}
			j = min( nRun-1, j);
			Line.EdPnt.y = RowRun[j].y;
			Line.EdPnt.x = (int)(a + b*RowRun[j].y );
			break;
		}
	}
	return 0;
}

/******************************************************************************
/*	Name:		GetColRunLength
/*	Function:	Extract column runlength
/*	Parameter:	Img		-- Image
/*				Line	-- A line
/*				nMaxSearchRange	-- Maximum search range above and below the line
/*				ColRun	-- Column runlength
/*				nRun	-- Number of runlength
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int	GetColRunLength( CImage &Img, LINE Line, int nMaxSearchRange, COLRUNLENGTH *&ColRun, int &nRun)
{
	int i, j;
	int		w = Img.GetWidth();
	int		h = Img.GetHeight();
	int		wb = Img.GetLineSizeInBytes();
	DSCC_BYTE	*pImg = Img.LockRawImg();
	DSCC_CPoint	StPnt = Line.StPnt;
	DSCC_CPoint	EdPnt = Line.EdPnt;

    DSCC_BYTE	mask[8] = {128,64,32,16,8,4,2,1} ;
    DSCC_BYTE*	tmp ;

	nRun = 0;
	int		nMaxRun = 100;
	ColRun = (COLRUNLENGTH *)malloc( sizeof(COLRUNLENGTH)*nMaxRun);
	if( ColRun == NULL )		return -1;
	for( i=max((long)0, StPnt.x); i<=min((long)(w-1), EdPnt.x); i++ )
	{
		int nByte = i/8;
		int nBit = i%8;
		int bTracked = FALSE;
		int y = StPnt.y + (i-StPnt.x)*(EdPnt.y-StPnt.y)/(EdPnt.x-StPnt.x);
		int ys = max(0, y-nMaxSearchRange );
		int ye = min(h-1, y+nMaxSearchRange );
		tmp = pImg + (h-1-ys)*wb + nByte;
		for( j=ys; j<=ye; j++ )
		{
			if( (*tmp & mask[nBit]) != 0 )//Black pixel
			{
				if( bTracked == FALSE )
				{
					ColRun[nRun].ys = j;
					bTracked = TRUE;
				}
			}
			else
			{//White pixel
				if( bTracked )
				{
					bTracked = FALSE;
					ColRun[nRun].ye = j-1;
					ColRun[nRun].x = i;
					nRun++;
					if( nRun >= nMaxRun )
					{
						nMaxRun += 100;
						ColRun = (COLRUNLENGTH*)realloc( ColRun, sizeof(COLRUNLENGTH)*nMaxRun);
					}
				}
			}
			tmp-=wb;
		}
		if( bTracked )
		{
			bTracked = FALSE;
			ColRun[nRun].ye = ye;
			ColRun[nRun].x = i;
			nRun++;
			if( nRun >= nMaxRun )
			{
				nMaxRun += 100;
				ColRun = (COLRUNLENGTH*)realloc( ColRun, sizeof(COLRUNLENGTH)*nMaxRun);
			}
		}
	}
	Img.UnlockRawImg();
    return 0 ;
}


/******************************************************************************
/*	Name:		GetRowRunLengthBit
/*	Function:	Extract row runlength
/*	Parameter:	Line -- Image data, 1 pixel per bit
/*				w -- Image width
/*				h -- Image height
/*				wb -- Image width in byte
/*				row -- Image row
/*				RunLength -- Runlength
/*				nRunLength -- Number of runlength
/*				bBlackConn -- TRUE   Black pixels connected component
/*							  FALSE  White pixels connected component
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int	GetRowRunLength( CImage &Img, LINE Line, int nMaxSearchRange, ROWRUNLENGTH *&RowRun, int &nRun)
{
    DSCC_BYTE mask[8] = {128,64,32,16,8,4,2,1} ;
	int i, j;
	int		w = Img.GetWidth();
	int		h = Img.GetHeight();
	int		wb = Img.GetLineSizeInBytes();
	DSCC_BYTE	*pImg = Img.LockRawImg();
	DSCC_CPoint	StPnt, EdPnt;
	if( Line.StPnt.y < Line.EdPnt.y )
	{
		StPnt = Line.StPnt;
		EdPnt = Line.EdPnt;
	}
	else
	{
		StPnt = Line.EdPnt;
		EdPnt = Line.StPnt;
	}

	int		nMaxRun = 100;
	RowRun = (ROWRUNLENGTH *)malloc( sizeof(ROWRUNLENGTH)*nMaxRun);
	if( RowRun == NULL )		return -1;

    int bInValley = FALSE;
	nRun = 0;
	for( i=max((long)0, StPnt.y); i<=min((long)(h-1), EdPnt.y); i++ )
	{
		bInValley = FALSE;
		int x = StPnt.x + (i-StPnt.y)*(EdPnt.x-StPnt.x)/(EdPnt.y-StPnt.y);
		for( j=max(0, x-nMaxSearchRange); j<=min(w-1, x+nMaxSearchRange); j++ )
		{
			DSCC_BYTE tmp = pImg[(h-1-i)*wb+j/8] & mask[j%8];
			if ( tmp )
			{
				if ( !bInValley )
				{
					RowRun[nRun].y = i;
					RowRun[nRun].xs = j;
					bInValley = TRUE ;
				}
			}
			else
			{
				if ( bInValley )
				{
					RowRun[nRun].xe = j-1;
					nRun ++;
					if( nRun >= nMaxRun )
					{
						nMaxRun += 100;
						RowRun = (ROWRUNLENGTH*)realloc( RowRun, sizeof(ROWRUNLENGTH)*nMaxRun);
					}

					bInValley = FALSE ;
				}
			}
		}
		if ( bInValley )
		{	
			RowRun[nRun].xe = j-1;
			nRun++;
			if( nRun >= nMaxRun )
			{
				nMaxRun += 100;
				RowRun = (ROWRUNLENGTH*)realloc( RowRun, sizeof(ROWRUNLENGTH)*nMaxRun);
			}
		}
	}
    return 0 ;
}

/******************************************************************************
/*	Name:		LocalFilter
/*	Function:	Using local filter to exclude some candidate column run length from 
/*				composing a line.
/*	Parameter:	Line	-- A line
/*				ColRun	-- Column runlength
/*				nRun	-- Number of runlength
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int LocalFilter( LINE Line, COLRUNLENGTH *ColRun, int nRun )
{
	int		i;
	int		DIST_TH = 3;

	//y = b*x + a
	double	a, b;
	GetLineParam( ColRun, nRun, a, b );
	for( i=0; i<nRun; i++ )
	{
		if( ColRun[i].bUsed == FALSE )	continue;

		int y = (int)(a + b*ColRun[i].x );
		int	dys = ColRun[i].ys - y;
		int dye = ColRun[i].ye - y;
		if( dys * dye < 0 )		continue;
		else if( min( abs(dys), abs(dye) ) > DIST_TH )
			ColRun[i].bUsed = FALSE;
	}
	return 0;
}

/******************************************************************************
/*	Name:		LocalFilter
/*	Function:	Using local filter to exclude some candidate row run length from 
/*				composing a line.
/*	Parameter:	Line	-- A line
/*				RowRun	-- Row runlength
/*				nRun	-- Number of runlength
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int LocalFilter( LINE Line, ROWRUNLENGTH *RowRun, int nRun )
{
	int		i;
	int		DIST_TH = 3;

	//x = b*y + a
	double	a, b;
	GetLineParam( RowRun, nRun, a, b );
	for( i=0; i<nRun; i++ )
	{
		if( RowRun[i].bUsed == FALSE )	continue;

		int x = (int)(a + b*RowRun[i].y );
		int	dxs = RowRun[i].xs - x;
		int dxe = RowRun[i].xe - x;
		if( dxs * dxe < 0 )		continue;
		else if( min( abs(dxs), abs(dxe) ) > DIST_TH )
			RowRun[i].bUsed = FALSE;
	}
	return 0;
}

/******************************************************************************
/*	Name:		AllocRunLength
/*	Function:	Label some column run length as candidates to compose a line
/*	Parameter:	Line	-- A line
/*				ColRun	-- Column runlength
/*				nRun	-- Number of runlength
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int AllocRunLength( LINE Line, COLRUNLENGTH *ColRun, int nRun )
{
	DSCC_CPoint	StPnt = Line.StPnt;
	DSCC_CPoint	EdPnt = Line.EdPnt;
	int nDist;

	int i;
	for( i=0; i<nRun; i++ )
		ColRun[i].bUsed = FALSE;

	i = 0;
	while( i<nRun )
	{
		int nCurrentX = ColRun[i].x;
		int	y = StPnt.y + (nCurrentX-StPnt.x)*(EdPnt.y-StPnt.y)/(EdPnt.x-StPnt.x);
		
		int j=i;
		int	nMinDist = 1000;
		int nMinDistIndex = -1;
		while( ColRun[j].x == nCurrentX )
		{
			int	dys = ColRun[j].ys - y;
			int dye = ColRun[j].ye - y;
			if( dys * dye < 0 )//Overlaping with the line on the vertical direction. It is a candidate.
				nDist = min(dys, dye);
			else//Otherwise find the candidate with minimum vertical distance to the line
				nDist = min( abs(dys), abs(dye) );

			if( nDist < nMinDist )
			{
				nMinDist = nDist;
				nMinDistIndex = j;
			}
			j++;
			if( j >= nRun )		break;
		}
		ColRun[nMinDistIndex].bUsed = TRUE;
		i = j;
	}
	return 0;
}

/******************************************************************************
/*	Name:		AllocLength
/*	Function:	Label some row run length as candidates to compose a line
/*	Parameter:	Line	-- A line
/*				ColRun	-- Column runlength
/*				nRun	-- Number of runlength
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int AllocRunLength( LINE Line, ROWRUNLENGTH *RowRun, int nRun )
{
	DSCC_CPoint	StPnt = Line.StPnt;
	DSCC_CPoint	EdPnt = Line.EdPnt;
	int nDist;

	int i;
	for( i=0; i<nRun; i++ )
		RowRun[i].bUsed = FALSE;

	i = 0;
	while( i<nRun )
	{
		int nCurrentY = RowRun[i].y;
		int	x = StPnt.x + (nCurrentY-StPnt.y)*(EdPnt.x-StPnt.x)/(EdPnt.y-StPnt.y);
		
		int j=i;
		int	nMinDist = 1000;
		int nMinDistIndex = -1;
		while( RowRun[j].y == nCurrentY )
		{
			int	dxs = RowRun[j].xs - x;
			int dxe = RowRun[j].xe - x;
			if( dxs * dxe < 0 )//Overlaping with the line on the horizontal direction. It is a candidate.
				nDist = min(dxs, dxe);
			else//Otherwise find the candidate with minimum horizontal distance to the line
				nDist = min( abs(dxs), abs(dxe) );

			if( nDist < nMinDist )
			{
				nMinDist = nDist;
				nMinDistIndex = j;
			}
			j++;
			if( j >= nRun )		break;
		}
		RowRun[nMinDistIndex].bUsed = TRUE;
		i = j;
	}
	return 0;
}