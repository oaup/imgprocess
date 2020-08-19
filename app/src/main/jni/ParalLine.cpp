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
// File NAME:		ParalLine.cpp
// File Function:	Common functions to processing paralle lines
//
//				Developed by: Yefeng Zheng
//			   First created: Sept. 2002
//			University of Maryland, College Park
///////////////////////////////////////////////////////////////////////////////////////////////////


#include "ParalLine.h"
#include "math.h"
#include "tools.h"

#ifndef	PI
#define PI  3.1415926535
#endif

/****************************************************************************************************
/*	Name:		ReadDetectResult
/*	Function:	Read ground groundtruth
/*	Parameter:	sFileName -- Groundtruth database file name
/*				pLine -- Pointer to reference lines 
/*				nLine -- Number of reference lines
/*	Return:		0  -- Succeed
/*				-1 -- Failed
/***************************************************************************************************/
int ReadDetectResult(string sFileName, LINE *&pLine, int &nLine )
{
	FILE	*fp = fopen(sFileName.c_str(), "rb");
	if( fp == NULL)			return	-1; 
	fread(&nLine, sizeof(int), 1, fp);
	if( nLine < 0 || nLine > 1000 )
	{
		printf( "Number of lines is wrong!\n" );
		return -1;
	}
	pLine = (LINE*)malloc(nLine*sizeof(LINE));
	if(pLine == NULL)		return -1;
	for(int i=0; i<nLine; i++)
	{	
		fread( &pLine[i].StPnt, sizeof(DSCC_CPoint), 1, fp);
		fread( &pLine[i].EdPnt, sizeof(DSCC_CPoint), 1, fp);
		fread( &pLine[i].nType, sizeof(int), 1, fp);
	}
	fclose( fp );
	return 0;
}

/****************************************************************************************************
/*	Name:		WriteDetectResult
/*	Function:	Write detection result
/*	Parameter:	sFileName -- File name
/*				pLine -- Pointer to detected lines 
/*				nLine -- Number of detected lines
/*	Return:		0  -- Succeed
/*				-1 -- Failed
/***************************************************************************************************/
int WriteDetectResult(string sFileName, LINE *pLine, int nLine )
{	
	if(pLine == NULL )	return -1;
	FILE	*fp = fopen(sFileName.c_str(), "wb");
	if(fp == NULL)						return -1;
	//nType=0 not used to form cells;  nStyle=3 virtual line
	fwrite(&nLine, sizeof(int), 1, fp);
	for(int i=0; i<nLine; i++)
	{
		fwrite( &pLine[i].StPnt, sizeof(DSCC_CPoint), 1, fp);
		fwrite( &pLine[i].EdPnt, sizeof(DSCC_CPoint), 1, fp);
		fwrite( &pLine[i].nType, sizeof(int), 1, fp);
	}
	fclose(fp);
	return 0;
}

/*************************************************************************************
/*	Name:		GetLineAngle
/*	Function:	Calculate the angle of the line formed by two points
/*	Parameter:	Start -- Starting point
/*				End -- Ending point
/*	Return:		Angle of the line
/**************************************************************************************/
double GetLineAngle ( DSCC_POINT Start, DSCC_POINT End )
{
    if ( End.x != Start.x )
    {
        double atg = atan ( ((double)(Start.y-End.y)) / ((double)(End.x-Start.x)) ) ;
        if ( End.x > Start.x )
            return atg ;
        else
        {
            if ( End.y < Start.y )
                return atg+PI ;
            else
                return atg-PI ;
        }
    }
    else
    {
        if ( End.y < Start.y )
            return PI/2 ;
        else if ( End.y > Start.y )
            return -PI/2 ;
        else
            return 0 ;
    }
}

/****************************************************************************************************
/*	Name:		GetLineSkew
/*	Function:	Get average line skew angle
/*	Parameter:	pLine -- Detected lines 
/*				nLine -- Number of detected lines
/*				nSkew -- Average skew angle
/*	Return:		0  -- Succeed
/*				-1 -- Failed
/***************************************************************************************************/
int	GetLineSkew( LINE *pLine, int nLine, double &nSkew )
{
	nSkew = 0;
	int nNum = 0;
	for(int i=0; i<nLine; i++ )
	{
		if( pLine[i].nType == 1 )
		{
			nSkew += GetLineAngle( pLine[i].StPnt, pLine[i].EdPnt );
			nNum++;
		}
	}
	nSkew /= nNum;
	return 0;
}

/****************************************************************************************************
/*	Name:		GetLineGap
/*	Function:	Get average gap between neighboring lines
/*	Parameter:	pLine -- Detected lines 
/*				nLine -- Number of detected lines
/*				nLineGap -- Average line gap between neighboring lines
/*	Return:		0  -- Succeed
/*				-1 -- Failed
/***************************************************************************************************/
int GetLineGap( LINE *pLine, int nLine, double &nLineGap )
{
	double nLeftLineGap = 0;
	double nRightLineGap = 0;
	int		Num = 0;

	for( int i=0; i<nLine-1; i++ )
	{
		if( pLine[i].nType == 0 || pLine[i+1].nType == 0 )		break;
		nLeftLineGap += pLine[i+1].StPnt.y - pLine[i].StPnt.y;
		nRightLineGap += pLine[i+1].EdPnt.y - pLine[i].EdPnt.y;
		Num ++;
	}
	if( Num == 0 )	return -1;
	nLeftLineGap /= Num;
	nRightLineGap /= Num;
	if( abs( nLeftLineGap-nRightLineGap ) >= 4 )
		printf( "Warning: lines are not parallel!" );
	nLineGap = (nLeftLineGap + nRightLineGap)/2;
	return 0;
}

/****************************************************************************************************
/*	Name:		RemoveVirtualLine
/*	Function:	Remove virtual lines
/*	Parameter:	pLine -- Detected lines 
/*				nLine -- Number of detected lines
/*	Return:		0  -- Succeed
/*				-1 -- Failed
/***************************************************************************************************/
int	RemoveVirtualLine( LINE *pLine, int &nLine )
{
	int i=0;
	while( i<nLine )
	{
		if( pLine[i].nType == 0 )
		{
			for( int j=i; j<nLine-1; j++ )
				pLine[j] = pLine[j+1];
			nLine--;
		}
		else 
			i++;
	}
	return 0;
}

/****************************************************************************************************
/*	Name:		RemoveBorderLine
/*	Function:	Remove border lines
/*	Parameter:	pLine -- Detected lines 
/*				nLine -- Number of detected lines
/*	Return:		0  -- Succeed
/*				-1 -- Failed
/***************************************************************************************************/
int	RemoveBorderLine( LINE *pLine, int &nLine )
{
	//Search for the first solid line
	int		nFirstLine =0;
	while( nFirstLine<nLine )
	{
		if( pLine[nFirstLine].nType == 0)	nFirstLine++;
		else		break;
	}
	//Remove virtual lines
	for( int j=nFirstLine; j<nLine; j++ )
		pLine[j-nFirstLine] = pLine[j];
	nLine -= nFirstLine;

	//Search for the last solid line
	int		nLastLine = nLine-1;
	while( nLastLine >= 0 )
	{
		if( pLine[nLastLine].nType == 0 )	nLastLine--;
		else			break;
	}
	nLine = nLastLine+1;
	return 0;
}

/****************************************************************************************************
/*	Name:		AdjustLinePosition
/*	Function:	Adjust starting and ending points of lines, extending them to image borders
/*	Parameter:	w	  -- Image width
/*				h     -- Image height
/*				pLine -- Detected lines 
/*				nLine -- Number of detected lines
/*	Return:		0  -- Succeed
/*				-1 -- Failed
/***************************************************************************************************/
int	AdjustLinePosition( int w, int	h, LINE *pLine, int nLine )
{
	int	i;
	DSCC_CPoint	LeftTopCorner, RightTopCorner, LeftBottomCorner, RightBottomCorner;
	LeftTopCorner = DSCC_CPoint(0, 0);
	RightTopCorner = DSCC_CPoint(w-1, 0);
	LeftBottomCorner = DSCC_CPoint(0, h-1);
	RightBottomCorner = DSCC_CPoint(w-1, h-1);
	for( i=0; i<nLine; i++ )
	{
		DSCC_CPoint	CrossPnt = GetCrossPoint( pLine[i].StPnt, pLine[i].EdPnt, LeftTopCorner, LeftBottomCorner );
		if( CrossPnt.y < 0 )
		{
			CrossPnt = GetCrossPoint( pLine[i].StPnt, pLine[i].EdPnt, LeftTopCorner, RightTopCorner );
			pLine[i].nType = 0;
		}
		else if( CrossPnt.y >= h )
		{
			CrossPnt = GetCrossPoint( pLine[i].StPnt, pLine[i].EdPnt, LeftBottomCorner, RightBottomCorner );
			pLine[i].nType = 0;
		}
		pLine[i].StPnt = CrossPnt;
		
		CrossPnt = GetCrossPoint( pLine[i].StPnt, pLine[i].EdPnt, RightTopCorner, RightBottomCorner );
		if( CrossPnt.y < 0 )
		{
			CrossPnt = GetCrossPoint( pLine[i].StPnt, pLine[i].EdPnt, LeftTopCorner, RightTopCorner );
			pLine[i].nType = 0;
		}
		else if( CrossPnt.y >= h )
		{
			CrossPnt = GetCrossPoint( pLine[i].StPnt, pLine[i].EdPnt, LeftBottomCorner, RightBottomCorner );
			pLine[i].nType = 0;
		}
		pLine[i].EdPnt = CrossPnt;
	}
	return 0;
}

/****************************************************************************************************
/*	Name:		GetHorLine
/*	Function:	Get horizontal lines
/*	Parameter:	pLine		-- All lines
/*				nLine		-- Number of lines
/*				HorLine		-- Horizontal lines
/*				nHorLine	-- Number of horizontal lines
/*	Return:		0  -- Succeed
/***************************************************************************************************/
int	GetHorLine( LINE *pLine, int nLine, LINE *&HorLine, int &nHorLine )
{
	HorLine = (LINE*)malloc( sizeof(LINE)*nLine );
	//Get all horizontal lines
	int i = 0;
	nHorLine = 0;
	for( ; i<nLine; i++ )
	{
		if( fabs((double)(pLine[i].EdPnt.y-pLine[i].StPnt.y)) < fabs((double)(pLine[i].EdPnt.x-pLine[i].StPnt.x )) )
		{ //Horizontal line
			HorLine[nHorLine] = pLine[i];
			nHorLine++;
		}
	}

	//Sort horizontal lines from top to bottom
	for( i=0; i<nHorLine; i++ )
	{
		int nMin = i;
		for( int j=i+1; j<nHorLine; j++ )
		{
			if( HorLine[j].StPnt.y + HorLine[j].EdPnt.y < 
				HorLine[nMin].StPnt.y + HorLine[nMin].EdPnt.y )
				nMin = j;
		}
		if( nMin != i )
		{
			LINE tmp = HorLine[i];
			HorLine[i] = HorLine[nMin];
			HorLine[nMin] = tmp;
		}
	}
	return 0;
}

/****************************************************************************************************
/*	Name:		GetVerLine
/*	Function:	Get vertical lines
/*	Parameter:	pLine		-- All lines
/*				nLine		-- Number of lines
/*				VerLine		-- Verizontal lines
/*				nVerLine	-- Number of verizontal lines
/*	Return:		0  -- Succeed
/***************************************************************************************************/
int GetVerLine( LINE *pLine, int nLine, LINE *&VerLine, int &nVerLine )
{
	//Get all vertical lines
	VerLine = (LINE*)malloc( sizeof(LINE)*nLine );
	nVerLine = 0;
	int i = 0;
	for( ; i<nLine; i++ )
	{
		if( fabs((double)(pLine[i].EdPnt.y-pLine[i].StPnt.y)) > fabs((double)(pLine[i].EdPnt.x-pLine[i].StPnt.x )) )
		{	//Vertical line
			VerLine[nVerLine] = pLine[i];
			nVerLine++;
		}
	}

	//Sort vertical line from left to right
	for( i=0; i<nVerLine; i++ )
	{
		int nMin = i;
		for( int j=i+1; j<nVerLine; j++ )
		{
			if( VerLine[j].StPnt.x + VerLine[j].EdPnt.x <
				VerLine[nMin].StPnt.x + VerLine[nMin].EdPnt.x)
				nMin = j;
		}
		if( nMin != i )
		{
			LINE tmp = VerLine[i];
			VerLine[i] = VerLine[nMin];
			VerLine[nMin] = tmp;
		}
	}
	return 0;
}
