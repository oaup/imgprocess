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
// File Name:		DirLine.h
// File Function:	1. Interface for CDirLine class
//					2. Define some inline methods to speedup the program
//
//				Developed by: Yefeng Zheng
//				First Created: June 2001
//			University of Maryland, College Park
//////////////////////////////////////////////////////////////////////////

#ifndef __DIRLINE_H__
#define __DIRLINE_H__

#include "convertMFC.h"
#include <iostream>
#include "ConnTree.h"
#include "ImageObj.h"
#include "FormStructDef.h"
#include "tools.h"
#include "math.h"

using namespace std;

class CDirLine //: public CObject
{
//DECLARE_SERIAL(CDirLine)
public :
	double	m_nSkewAngle;

    int	m_bIsHorLine;   //Horizontal or Veritical line
	DSCC_RECT	m_rcBoundRange; //Bound region of the image analysed

    int			m_nLine ;//Line number
    FORMLINE	*m_pLine ;//Pointer to Line info
    int		m_bParamsSet;
	PARAMETER   m_Param; //Parameters 

public :
    int		m_nChain;//Chain number
    CHAIN	*m_pChain;//Pointer to Chain info
    int		m_nChains;//Chains number
    CHAINS	*m_pChains;//Pointer to Chains info
protected:
//Variables used to split a image into several strips to speed up the algorithm
	int		m_nStrip; //Strip number of the image devided
	int		m_nCurTree;//Current strip under analysis
    CConnTree** m_pTree;//Each strip analysis by a CConnTree instance
	int		m_nOldChain;//Number of CHAINs have been extracted before the current strip analysed
	int		m_nChainStart[MAXSTRIP];//First CHAIN of each strip

public :
    CDirLine() ;
    ~CDirLine() ;
    int		FreeMem(); //Free memory

///////////////////////////////////////////////////////////////////////////////////
//	Methods to create Directional Single Connected Chain
///////////////////////////////////////////////////////////////////////////////////
	//Set parameters
    int		SetDetectParams ( int bIsHorLine, PARAMETER &Param ) ;
	//Set default parameters
	int		SetDefaultDetectParams ();
    //Extract runlengths
	int		BuildConnTree ( CImage& Img, DSCC_RECT rRange ) ;
    //Get pixels on a horizontal image row
	int		AquireHorLineData ( DSCC_BYTE *p, int w, int h, int n, int start, int end, int *buf, int ImgType, int Channel=0, int bUpSideDown = TRUE ) ;
    //Get pixels on a vertical image column
	int		AquireVerLineData ( DSCC_BYTE *p, int w, int h, int n, int start, int end, int *buf, int ImgType, int Channel=0, int bUpSideDown = TRUE ) ;
    //Extract column runlength
	int		GetColumnRunLength ( DSCC_BYTE *p, int w, int h, int column, int start, int end, LineValley *valley, int bUpSideDown ) ;
    //Extract row runlength
	int		GetRowRunLength ( DSCC_BYTE *p, int w, int h, int row, int start, int end, LineValley *valley, int bUpSideDown ) ;
    //Extract runlength on gray image
	int		ValleyDetect ( int *p, int start, int end, LineValley *valley, int *s, int Depth, int MaxLen ) ;
	//Column runlength smoothing
	int		ColRunLenSmooth(DSCC_BYTE *p, int w, int h, int col, LineValley *valley, int& Valleys);
	//Row runlength smoothing
	int		RowRunLenSmooth(DSCC_BYTE *p, int w, int h, int row, LineValley *valley, int& Valleys);	
	//Create DSCC
    int		CalTree() ;

///////////////////////////////////////////////////////////////////////////////////
//	Methods to extract CHAINS from CHAIN
///////////////////////////////////////////////////////////////////////////////////
	//Calculate black pixels between a CHAIN to a CHAINS
	int		PixelsBetween ( CHAINS &Chains, CHAIN &Chain, int &MaxWidth );
	//Merge CHAINs on the left side
    int		LeftMerge ( INTCHAIN *pTailXTbl, int *pTailXIndex, int &SeedChains ) ;
    //Merge CHAINs on the right side
	int		RightMerge ( INTCHAIN *pHeadXTbl, int *pHeadXIndex, int &SeedChains ) ;
	//Merge CHAINs to get CHAINSes
    int		MergeChains() ;
	//Merge two CHAINSes
	int		MergeChains ( CHAINS &Chains1, CHAINS &Chains2 ) ;
	//Sort CHAIN head, speedup searching
    int		SortChainHead ( INTCHAIN *pTbl, int *pIndex ) ;
    //Sort CHAIN tail, speedup searching
	int		SortChainTail ( INTCHAIN *pTbl, int *pIndex ) ;
    //Sort CHAIN by length, speedup searching
	int		SortChainLen ( int MaxLen, INTCHAIN *pTbl, int *pIndex ) ;
    //Add a CHAIN to a CHAINS
	int		AddChain ( int &nChains, int nChain ) ;
	//Delete redudent CHAINS
    int		DeleteChains ( int nDelChains ) ;
    //Calculate the distance of two CHAINs
	double	ChainDistance ( int nChains, int nChain, double Max ) ;
    //Calculate the distance of two CHAINs
	double	ChainDistance ( CHAINS &Chains1, CHAINS &Chains2, double Max ) ;
    //Initialize a CHAINS
	int		InitChains ( CHAINS &Chains, CHAIN &Chain, int nChain ) ;
	//Test if two CHAINs are connected
    int	IsConnected ( int c1, int c2 ) ;
	//Test if two CHAINs are connected within a gap
	int		IsConnected2(int c1, int c2, int&width, int&gap);

	//Merge CHAINs to get CHAINSes
	int		MergeChains( double nSkewAngle );
	//Merge CHAINs on the left side
    int		LeftMerge ( INTCHAIN *pTailXTbl, int *pTailXIndex, int &SeedChains, double nSkewAngle  ) ;
    //Merge CHAINs on the right side
	int		RightMerge ( INTCHAIN *pHeadXTbl, int *pHeadXIndex, int &SeedChains, double nSkewAngle  ) ;
    //Get Form Line from CHAINS 
	int		ChainsToFORMLINE ( FORMLINE &Line, CHAINS &Chains ) ;

///////////////////////////////////////////////////////////////////////////////////
//	Methods to dump debug informations to files
///////////////////////////////////////////////////////////////////////////////////
	//Dump runlength to image file
	int		DumpNodes ( string FileName, int w, int h );
	//Dump connected component to image file
	int		DumpConnComp(string FileName, int w, int h);
	//Dump CHAIN to image file
	int		DumpChain ( string FileName, int w, int h);
	//Dump CHAIN to image file
	int		DumpChain ( CImage &Img, int w, int h);
	//Dump CHAIN to text file
	int		DumpChainData(string FileName);
	//Dump CHAINS to image file
	int		DumpChains ( string FileName, int w, int h);
	//Dump line of CHAINS to image file
	int		DumpChainsLine ( string FileName, int w, int h);
	//Dump CHAINS to text file
	int		DumpChainsData ( string  FileName);
    //Dump LINE to image file
	int		DumpLine ( string FileName, int w, int h ) ;
	//Dump LINE to text file
	int		DumpLineData(string FileName);

///////////////////////////////////////////////////////////////////////////////////
//	Methods to get statistical property of DSCC and lines
///////////////////////////////////////////////////////////////////////////////////
	//Find a CHAIN in which strip
	int		InWhichTree(int nChain);
	//Find a CHAIN in which CHAINS
    int		InWhichChains ( int nChain ) ;
	//Estimate average character width
	int		GetAverCharWidth(int	bLastStrip);
	//Get y ordinate using CHAIN statistical property
	double	fYofChain(CHAIN &Chain, double x);
	//Get y ordinate using CHAINS statistical property
	double	fYofChains(CHAINS &Chains, double x);
	//Get y ordinate using Line statistical property
    double	fYofLine ( FORMLINE &Line, double x ) ;
    //Calculate CHAIN quality
	int		ChainStatics( CHAIN &Chain ) ;
    //Calculate CHAINS quality
	double	ChainsQuality ( CHAINS &Chains ) ;

////////////////////////////////////////////////////////////////////////////////////
//	Detect parallel dash lines
////////////////////////////////////////////////////////////////////////////////////
	//Filter CHAINs unlikely to be a line segment
	int		FilterChain( );
	//Estimate skew angle
	int		EstimateSkew( CImage &Img, double &nSkewAngle );
	//Refine skew angle estimation
	int		RefineSkewEstimation( CImage &Img, double &nSkewAngle, double *&HProj, int &nHProj);
	int		DSCCFiltering( CImage &Img, CImage &FilteredImg, int bHorLine );

};

/*******************************************************************
*Name:		fYofChain
*Function:	Get y ordinate using CHAIN statistical property
*			
********************************************************************/
inline double CDirLine::fYofChain ( CHAIN &Chain, double x )
{
	double tmp, a, b ;
    tmp = Chain.SumX*Chain.SumX - Chain.Num*Chain.SumXX ;
    if ( fabs(tmp) > 1e-8 )
    {
        a = Chain.SumXY*Chain.SumX - Chain.SumY*Chain.SumXX ;
        b = Chain.SumX*Chain.SumY - Chain.Num*Chain.SumXY ;
        return (a + b*x)/tmp;
    }
    else if( Chain.Num != 0 )
        return Chain.SumY/Chain.Num ;
	else
		return Chain.SumY;
}

/*******************************************************************
*Name:		fYofChains
*Function:	Get y ordinate using CHAINS statistical property
*			
********************************************************************/
inline double CDirLine::fYofChains ( CHAINS &Chains, double x )
{
	double tmp, a, b ;
    tmp = Chains.SumX*Chains.SumX - Chains.Num*Chains.SumXX ;
    if ( fabs(tmp) > 1e-8 )  
	{
        a = Chains.SumXY*Chains.SumX - Chains.SumY*Chains.SumXX ;
        b = Chains.SumX*Chains.SumY - Chains.Num*Chains.SumXY ;
        return (a + b*x)/tmp ;
    }
    else if( Chains.Num != 0 )
        return Chains.SumY/Chains.Num ;
	else
		return Chains.SumY;
}

/*******************************************************************
*Name:		fYofLine
*Function:	Get y ordinate using Form Frame Line statistical property
*			
********************************************************************/
inline double CDirLine::fYofLine ( FORMLINE &Line, double x )
{
	if( Line.nIndex >= 0 )
		return fYofChains ( m_pChains[Line.nIndex], x);
	else
	{
		if( m_bIsHorLine )
		{
			if( Line.StPnt.x == Line.EdPnt.x )
				return 1.0*Line.StPnt.y;
			else
				return Line.StPnt.y+1.0*(Line.EdPnt.y-Line.StPnt.y)*(x-Line.StPnt.x)/(Line.EdPnt.x-Line.StPnt.x);
		}
		else
		{
			if( Line.StPnt.y == Line.EdPnt.y )
				return 1.0*Line.StPnt.x;
			else
				return Line.StPnt.x+1.0*(Line.EdPnt.x-Line.StPnt.x)*(x-Line.StPnt.y)/(Line.EdPnt.y-Line.StPnt.y);
		}
	}
}
#endif //__DIRLINE_H__