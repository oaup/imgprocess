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

/////////////////////////////////////////////////////////////////////////
// File Name:		FilterSkew.cpp
// File Function:	Used for parallel background line detection
//					1. DSCC filtering
//					2. Skew angle estimation based on lines
//
//				Developed by: Yefeng Zheng
//			   First Created: Feb. 2003
//			Universtiy of Maryland, College Park
//////////////////////////////////////////////////////////////////////////


#include "FormStructDef.h"
#include "DirLine.h"
#include "ImageTool.h"
#include "tools.h"
#include <math.h>
#include <algorithm>


#define	MAX_LINE_GAP	1000

/****************************************************************************************************
/*	Name:		DSCCFiltering
/*	Function:	Filter DSCC, only preserve those created by horizontal line segments
/*	Parameter:	Image		-- Original image
/*				FilteredImg -- Image after filtering
/*	Return:		0  -- Succeed
/*				-1 -- Failed
/***************************************************************************************************/
int CDirLine::DSCCFiltering( CImage &Image, CImage &FilteredImg, int bHorLine )
{
	FreeMem();
	m_rcBoundRange.left = 0;
	m_rcBoundRange.right = Image.GetWidth()-1;
	m_rcBoundRange.top = 0;
	m_rcBoundRange.bottom = Image.GetHeight()-1;

	m_nStrip = 1;
	m_pTree = (CConnTree**)malloc(sizeof(CConnTree*));

	m_pTree[0] = new CConnTree();
	m_pTree[0]->m_IsHorConn = bHorLine;
	m_bIsHorLine = bHorLine;
	m_nCurTree= 0;
	if ( BuildConnTree(Image, m_rcBoundRange) != 0 )
		return -1 ;

	if(	CalTree()!=0) return -1 ;
	FilterChain( );
	m_nChainStart[0] = m_nChain;
//	DumpChain( FilteredImg, Image.GetWidth(), Image.GetHeight() );
	return 0 ;
}

/****************************************************************************************************
/*	Name:		EstimateSkew
/*	Function:	Skew angle estimation based on lines
/*	Parameter:	Image	   -- Image
/*				nSkewAngle -- The estimated skew angle
/*	Return:		0  -- Succeed
/*				-1 -- Failed
/***************************************************************************************************/
int CDirLine::EstimateSkew( CImage &Image, double &nSkewAngle )
{
	FreeMem();
	m_rcBoundRange.left = 0;
	m_rcBoundRange.right = Image.GetWidth()-1;
	m_rcBoundRange.top = 0;
	m_rcBoundRange.bottom = Image.GetHeight()-1;

	m_nStrip = 1;
	m_pTree = (CConnTree**)malloc(sizeof(CConnTree*));

	m_pTree[0] = new CConnTree();
	m_pTree[0]->m_IsHorConn = TRUE;
	m_nCurTree= 0;
	if ( BuildConnTree(Image, m_rcBoundRange) != 0 )
		return -1 ;

	if(	CalTree()!=0) return -1 ;
	m_nChainStart[0] = m_nChain;
	MergeChains();

	int		i;
	int		MinLen = 20;
	double	nAngleHist[200];
	double	AngleDelta = PI/360;
	memset( nAngleHist,0, 200*sizeof(int) );
	for( i=0; i<m_nChains; i++ )
	{
		if( m_pChains[i].Num < MinLen )			continue;
		if( fabs( m_pChains[i].Angle ) > PI/4 )	continue;
		int	nIndex = (int)(m_pChains[i].Angle/AngleDelta) + 100;
		if( nIndex < 0 || nIndex > 199 )
			continue;
		nAngleHist[nIndex] += m_pChains[i].Num;
	}
//	CImageTool::DumpProject( "C:\\AngleHist.tif", nAngleHist, 200 );
	CImageTool::SmoothProject( nAngleHist, 200 );

	int		nMaxIndex=0;
	for( i=0; i<200; i++ )
		if( nAngleHist[i] > nAngleHist[nMaxIndex] )
			nMaxIndex = i;

	double	nAngle[10000];
	int		Num = 0;
	nSkewAngle = (nMaxIndex-100)*AngleDelta;
	for( i=0; i<m_nChains; i++ )
	{
		if( m_pChains[i].Num < MinLen )			continue;
		if( fabs( m_pChains[i].Angle ) > PI/4 )	continue;
		if( fabs( m_pChains[i].Angle - nSkewAngle ) <= AngleDelta && Num<1000 )
		{
			nAngle[Num] = m_pChains[i].Angle;
			Num++;
		}
	}
	if( Num <= 6 )//To few samples for estimation
		nSkewAngle = 0;
	else
		nSkewAngle = GetMidValue( nAngle, Num );
	return 0;
}

/****************************************************************************
/*	Name:		FilterChain
/*	Function:	Filter chains not like to be a line segment
/*	Parameter:	void
/*	return:		0 -- Succeed
/*				1 -- Error
/***************************************************************************/
int CDirLine::FilterChain( )
{
	int		i, j, k;
	int		w, h;
	double	Height;
	double	MaxHeight = 5;
	for( i=0, k=0; i<m_nChain; i++ )
	{
		int	bRemove = FALSE;
		w = m_pChain[i].rcBound.right - m_pChain[i].rcBound.left + 1;
		h = m_pChain[i].rcBound.bottom - m_pChain[i].rcBound.top + 1;
		double	vw = m_pChain[i].SXX/m_pChain[i].NumPixel - pow( m_pChain[i].SX/m_pChain[i].NumPixel, 2 );
		vw = sqrt( vw );
		double	vh = m_pChain[i].SYY/m_pChain[i].NumPixel - pow( m_pChain[i].SY/m_pChain[i].NumPixel, 2 );
		vh = max((double)1, sqrt( vh ));
		if( vw >= 3*vh )
			bRemove = FALSE;
		else
			if( w >= 3*h )
			bRemove = FALSE;
		else
			if( h >= 2*MaxHeight && h >= w )
			bRemove = TRUE;
		else
		{
			Height = 0;
			int		pNode = m_pChain[i].pHead ;
			DSCC_CPoint	StPnt = DSCC_CPoint( m_pChain[i].xs, m_pChain[i].fYs );
			DSCC_CPoint	EdPnt = DSCC_CPoint( m_pChain[i].xe, m_pChain[i].fYe );
			LineValley v;

			for ( j=0 ; j<m_pChain[i].Len ; j++ )
			{
				v = m_pTree[m_nCurTree]->m_pNode[pNode].v ;
				int x = v.x & 0x1fffffff ;
				
				Height = max( Height, GetDistance( DSCC_CPoint( x, v.yvs), StPnt, EdPnt ) );
				Height = max( Height, GetDistance( DSCC_CPoint( x, v.yve), StPnt, EdPnt ) );
				
				pNode = m_pTree[m_nCurTree]->m_pNode[pNode].pRight ;
				if(pNode < 0)	break;
			}

			if( Height >= MaxHeight )
				bRemove = TRUE;
		}
		if( bRemove == FALSE )
		{
			m_pChain[k] = m_pChain[i];
			k++;
		}
	}
	m_nChain = k;
	return 0;
}

/****************************************************************************
/*	Name:		RefineSkewEstimation
/*	Function:	Refine skew estimation
/*	Parameter:	Img			-- Image
/*				nSkewAngle	-- Skew angle
/*				HPorj		-- Skewed horizontal projection
/*				nHProj		-- Number of projection
/*	return:		0 -- Succeed
/*				1 -- Error
/***************************************************************************/
int CDirLine::RefineSkewEstimation( CImage &Img, double &nSkewAngle, double *&HProj, int &nHProj)
{
	double	nAngle;
	double	nDeltaAngle = PI/180/5;
	
	int		w = Img.GetWidth();
	int		h = Img.GetHeight();

	int		nMax = 0;
	double	Sum=0, nMaxSum = 0;
	int		nLeftStart, nRightStart, nLeftEnd, nRightEnd;

	double	*Proj, *tmpProj;
	int		nProj;
	Proj = (double*)malloc( sizeof(double)*2*h);
	tmpProj = (double*)malloc( sizeof(double)*2*h);

	HProj = (double*)malloc( sizeof(double)*2*h);
	memset( HProj,0, sizeof(double)*2*h);

	for( int i=-8; i<=8; i++ )
	{
		nAngle = nSkewAngle + i*nDeltaAngle;
		int	dh = (int)( w*tan(nAngle) );
		if( nAngle >= 0 )
		{
			nLeftStart = 1;
			nRightStart = -dh+1;
		}
		else
		{
			nLeftStart = dh+1;
			nRightStart = 1;
		}
		nLeftEnd = abs(dh) + h + nLeftStart;
		nRightEnd = abs(dh) + h + nRightStart;

		int	xs, ys, xe, ye;
		DSCC_CPoint	CrossPnt;
		for(nProj=0; nLeftStart<nLeftEnd; nLeftStart++, nRightStart++, nProj++)
		{
			if( nLeftStart < 0 || nRightStart < 0 )
			{
				GetCrossPoint( 0, nLeftStart, w-1, nRightStart, 0, 0, w-1, 0, CrossPnt );
				if( nLeftStart < 0 )
				{
					xs = CrossPnt.x;
					ys = CrossPnt.y;
					xe = w-1;
					ye = nRightStart;
				}
				else
				{
					xs = 0;
					ys = nLeftStart;
					xe = CrossPnt.x;
					ye = CrossPnt.y;
				}
			}
			else if( nLeftStart >= h || nRightStart >= h )
			{
				GetCrossPoint( 0, nLeftStart, w-1, nRightStart, 0, h-1, w-1, h-1, CrossPnt );
				if( nLeftStart >= h )
				{
					xs = CrossPnt.x;
					ys = CrossPnt.y;
					xe = w-1;
					ye = nRightStart;
				}
				else
				{
					xs = 0;
					ys = nLeftStart;
					xe = CrossPnt.x;
					ye = CrossPnt.y;
				}
			}
			else
			{
				xs = 0;
				ys = nLeftStart;
				xe = w-1;
				ye = nRightStart;
			}
			Proj[nProj] = CImageTool::PixelsOnHorLine( Img, xs, ys, xe, ye );
		}

		CImageTool::SmoothProject( Proj, nProj );

		memcpy( tmpProj, Proj, sizeof(double)*nProj );
		SortData( tmpProj, nProj, FALSE );
		Sum = 0;
		for( int j=0; j<min(50, nProj); j++ )
			Sum += tmpProj[j];
		if( Sum > nMaxSum ||
			(Sum == nMaxSum && abs(i) < abs(nMax) ) )
		{
			nMaxSum = Sum;
			nMax = i;
			memcpy( HProj, Proj, sizeof(double)*nProj );
			nHProj = nProj;
		}
	}
	nSkewAngle += nMax*nDeltaAngle;
	m_nSkewAngle = nSkewAngle;
	free( tmpProj );
	free( Proj );
	return 0;
}