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
// File NAME:		DetectLine.cpp
// File Function:	Detect lines using the HMM model
//
//				Developed by: Yefeng Zheng
//			   First created: Aug. 2003
//			University of Maryland, College Park
///////////////////////////////////////////////////////////////////////////////////////////////////


#include "ProcForm.h"
#include "GetProcessFile.h"
#include "ImageObj.h"
#include "ImageTool.h"
#include "tools.h"
#include "ParalLine.h"
#include <cmath>
#include "DirLine.h"
#include "GroupRunLength.h"
#include <algorithm>


#define	MIN_PROBABILITY		1e-6

double	MIN_PROBABILITY_LOG	= log(MIN_PROBABILITY);

/*
	Name:		GetGaussProbLog
	Function:	Calculate the probability of a Gaussian distribution.
				The probability is converted to log form.
	Parameter:	x   -- Input point
			m   -- Mean of Gaussian distribution
				v	-- Variance of Gaussian distribution
	Return:		Logarithm of the probability
*/
double GetGaussProbLog( int x, double m, double v )
{
	double nProb = -(x-m)*(x-m)/(2*v*v) + log(1/v);
	return nProb;
}

/*
	Name:		GetGaussProbLogLogMin
	Function:	Calculate the probability of a Gaussian distribution.
				The probability is converted to log form. The probability is bounded
				by MIN_PROBABILITY_LOG
	Parameter:	x   -- Input point
				m   -- Mean of Gaussian distribution
				v	-- Variance of Gaussian distribution
	Return:		Logarithm of the probability
*/
double GetGaussProbLogMin( int x, double m, double v )
{
	double nProb = -(x-m)*(x-m)/(2*v*v) + log(1/v);
	return max( nProb, MIN_PROBABILITY_LOG );
}

/*
	Name:		BoundLineEndPoint
	Function:	Bound the end points of lines inside the image
    Parameter:	x   -- Input point
			m   -- Mean of Gaussian distribution
				v	-- Variance of Gaussian distribution
     Return:		0  -- Succeed				-1 -- Failed
*/
int BoundLineEndPoint( LINE *pLine, int nLine, int w, int h )
{
	for(int i=0; i<nLine; i++ )
	{
		pLine[i].StPnt.x = max((long)0, min(pLine[i].StPnt.x, (long)(w - 1)));
		pLine[i].EdPnt.x = max((long)0, min(pLine[i].EdPnt.x, (long)(w - 1)));
		pLine[i].StPnt.y = max((long)0, min(pLine[i].StPnt.y, (long)(h - 1)));
		pLine[i].EdPnt.y = max((long)0, min(pLine[i].EdPnt.y, (long)(h - 1)));
	}
	return 0;
}

/*
	Name:		DecodeHMM
	Function:	Using DTW to search optimal state sequence of HMM model
	Parameter:	Proj	-- Horizontal projection (Quantized)
				nProj   -- Number of the projection
				nLine	-- Number of lines
				pLinePos	-- Decoded line positions
				TotalProb	-- Probability to observe such sequence given the model
				LineLenMean	-- Mean of line projection
			    LineLenVar	-- Variance of line projection
				TextLenMean	-- Mean of text projection
				TextLenVar	-- Variance of text projection
				LineGapProb	-- Line gap distribution
	Return:		0  -- Succeed				-1 -- Failed
*/
int DecodeHMM( double *Proj, int nProj, int nLine, int *pLinePos, double &TotalProb,
			double *LineLenMean, double *LineLenVar,
			double TextLenMean, double TextLenVar, LINE_GAP LineGapProb[])
{
	int		i, j, k, m;
	// Calculate the minimum span of lines
	int		nMinProj = 0;
	for( i=0; i<nLine-1; i++ )
		nMinProj += LineGapProb[i].nMinGap;
	if( nMinProj > nProj )
		return -1;	
	
	//Alloc memory
	//Decoding result
	int		***LineList = (int***)malloc( sizeof(int**)*nProj );
	for( i=0; i<nProj; i++ )
	{
		LineList[i] = (int**)malloc( sizeof(int*)*nLine );
		for( j=0; j<nLine; j++ )
		{
			LineList[i][j] = (int*)malloc( sizeof(int)*nLine );
			for( k=0; k<nLine; k++ )
				LineList[i][j][k] = -1;
		}
	}
	//Probability table during decoding
	double  **Prob = (double**)malloc( sizeof(double*)*nProj );
	for( i=0; i<nProj; i++ )
	{
		Prob[i] = (double*)malloc( sizeof(double)*nLine );
		for( j=0; j<nLine; j++ )
			Prob[i][j] = -1e+10;
	}


	//Initialization by decoding the first line position
	double	nNewProb;
	double	nAllTextProb = 0;
	for( i=0; i<nProj-nMinProj; i++ )
	{
		LineList[i][0][0] = i;
		Prob[i][0] = nAllTextProb + GetGaussProbLogMin( Proj[i], LineLenMean[0], LineLenVar[0] );
		nAllTextProb += GetGaussProbLogMin( Proj[i], TextLenMean, TextLenVar );
	}

	// Begin decoding
	for( i=0; i<nProj; i++ )
	{
		for( j=0; j<nLine-1; j++ )
		{
			if( LineList[i][j][j] == -1 )
				continue;
			if( i+LineGapProb[j].nMinGap >= nProj )
				continue;

			//Decode some observation as text
			nAllTextProb = 0;
			for( k=i+1; k<i+LineGapProb[j].nMinGap; k++ )
				nAllTextProb += GetGaussProbLogMin( Proj[k], TextLenMean, TextLenVar );

			for( k=max(0, i+LineGapProb[j].nMinGap); k<=min(nProj-1, i+LineGapProb[j].nMaxGap); k++ )
			{
				int nGap = k-i-LineGapProb[j].nMinGap;
				if( nGap < 0 || nGap >= LineGapProb[j].nMaxGap-LineGapProb[j].nMinGap+1 )
				{
					printf( "error!\n" );		return -1;
				}
				nNewProb = Prob[i][j] + nAllTextProb + LineGapProb[j].nProb[nGap] + GetGaussProbLogMin( Proj[k], LineLenMean[j+1], LineLenVar[j+1] );
				if( nNewProb > Prob[k][j+1] )//Update decoding result of k
				{
					Prob[k][j+1] = nNewProb;
					for( m=0; m<=j; m++ )
						LineList[k][j+1][m] = LineList[i][j][m];
					LineList[k][j+1][j+1] = k;
				}
				nAllTextProb += GetGaussProbLogMin( Proj[k], TextLenMean, TextLenVar );
			}
		}

		//Decoding the following observation as text
		if( LineList[i][nLine-1][nLine-1] != -1 )
		{
			nAllTextProb = 0;
			for( k=i+1; k<nProj; k++ )
				nAllTextProb += GetGaussProbLogMin( Proj[k], TextLenMean, TextLenVar );
			nNewProb = nAllTextProb + Prob[i][nLine-1];
			if( nNewProb > Prob[nProj-1][nLine-1] )
			{//Update the final decoding result
				Prob[nProj-1][nLine-1] = nNewProb;
				for( m=0; m<nLine; m++ )
					LineList[nProj-1][nLine-1][m] = LineList[i][nLine-1][m];
			}
		}
	}
	//Output the decoding results
	TotalProb = Prob[nProj-1][nLine-1];
	for( i=0; i<nLine; i++ )
		pLinePos[i] = LineList[nProj-1][nLine-1][i];

	//Free memory
	for( i=0; i<nProj; i++ )
	{
		for( j=0; j<nLine; j++ )
			free( LineList[i][j] );
		free( LineList[i] );
		free( Prob[i] );
	}
	free( LineList );
	free( Prob );
	return 0;
}

/*
	Name:		CalEndPoint
	Function:	Using crossing information to calculate the ending points of lines
	Parameter:	HorLine		-- Horizontal lines
				nHorLine	-- Number of horizontal lines
				VerLine		-- Vertical lines
				nVerLine	-- Number of vertical lines
				HCross		-- Crossing information of horizontal lines
				VCross		-- Crossing information of vertical lines
	Return:		0  -- Succeed
*/
int CalEndPoint( LINE *HorLine, int nHorLine, LINE *VerLine, int nVerLine, CROSS *HCross, CROSS *VCross )
{
	DSCC_CPoint Pnt;
	int	i, nCross;
	for( i=0; i<nHorLine; i++ )
	{
		if( HCross[i].nStLine >= 0 && HCross[i].nStLine < nVerLine )
		{
			nCross			= HCross[i].nStLine;
			Pnt				= GetCrossPoint( HorLine[i].StPnt, HorLine[i].EdPnt, VerLine[nCross].StPnt, VerLine[nCross].EdPnt );
			HorLine[i].StPnt= Pnt + HCross[i].StOffset;
		}
		if( HCross[i].nEdLine >= 0 && HCross[i].nEdLine < nVerLine )
		{
			nCross			= HCross[i].nEdLine;
			Pnt				= GetCrossPoint( HorLine[i].StPnt, HorLine[i].EdPnt, VerLine[nCross].StPnt, VerLine[nCross].EdPnt );
			HorLine[i].EdPnt= Pnt + HCross[i].EdOffset;
		}
	}

	for( i=0; i<nVerLine; i++ )
	{
		if( VCross[i].nStLine >= 0 && VCross[i].nStLine < nHorLine )
		{
			nCross			= VCross[i].nStLine;
			Pnt				= GetCrossPoint( VerLine[i].StPnt, VerLine[i].EdPnt, HorLine[nCross].StPnt, HorLine[nCross].EdPnt );
			VerLine[i].StPnt= Pnt + VCross[i].StOffset;
		}
		if( VCross[i].nEdLine >= 0 && VCross[i].nEdLine < nHorLine )
		{
			nCross			= VCross[i].nEdLine;
			Pnt				= GetCrossPoint( VerLine[i].StPnt, VerLine[i].EdPnt, HorLine[nCross].StPnt, HorLine[nCross].EdPnt );
			VerLine[i].EdPnt= Pnt + VCross[i].EdOffset;
		}
	}
	return 0;
}

/*
	Name:		CalSearchRange
	Function:	Calculate the searching range
	Parameter:	Model -- HMM model
	Return:		Searching range
*/
int CalSearchRange( HMM_MODEL &Model )
{
	int nSearchRange = 0;
	int nCount = 0;
	int i = 0;
	for( ; i<Model.nHorLine-1; i++ )
	{
		int nRange = Model.HLineGapDist[i].nMaxGap - Model.HLineGapDist[i].nMinGap;
		if( nRange > 0 )
		{
			nSearchRange += nRange;
			nCount ++;
		}
	}
	for( i=0; i<Model.nVerLine-1; i++ )
	{
		int nRange = Model.VLineGapDist[i].nMaxGap - Model.VLineGapDist[i].nMinGap;
		if( nRange > 0 )
		{
			nSearchRange += nRange;
			nCount ++;
		}
	}
	if( nCount == 0 )	
		return 10;
	else
		return nSearchRange / (2*nCount);
}
/*
	Name:		DetectLine
	Function:	Detect lines using HMM model
	Parameter:	fnHMM	-- File name of HMM model
				bFilter -- Performing text filtering or not
	Return:		0  -- Succeed
*/
// int	DetectLine( char *fnHMM, int bFilter )
// {
// 	int i;
// 
// 	// Read HMM model
// 	HMM_MODEL	Model;
// 	if( ReadHMM( fnHMM, Model ) != 0 )
// 	{
// 		printf( "Read HMM model:%s failed!\n", fnHMM );
// 		return -1;
// 	}
// 	int nHorLine = Model.nHorLine;
// 	int nVerLine = Model.nVerLine;
// 
// 	int		*pLinePos	= new int[max( nHorLine, nVerLine )];
// 	LINE	*pLine		= new LINE[nHorLine+nVerLine];
// 
// 	// Calculate searching range to adjust line end points
// 	int	nSearchRange = CalSearchRange( Model );
// 
// 	// Processing all images
// 	char fnImg[MAX_PATH];
// 	char fnFDR[MAX_PATH];
// 	GetFirstFile( fnImg );
// 
// 	CImage Img;
// 	CDirLine	DirLine;
// 	CImage		FilteredImg;
// 	while( 1 )
// 	{
// 		printf( "Processing %s ...\n", GetFileName( fnImg ) );
// 	
// 		// Read image
// 		if( Img.Read( fnImg ) != 0 )
// 		{
// 			printf( "Read image: %s failed!\n", fnImg );
// 			return -1;
// 		}
// 		int w = Img.GetWidth();
// 		int h = Img.GetHeight();
// 
// 		double	*Proj;
// 		int		nProj;
// 		double	nTotalProb;
// 		int		nLine		= 0;
// 
// 		if( nHorLine > 0 )
// 		{
// 			//Horizontal projection
// 			if( bFilter )
// 			{
// 				DirLine.DSCCFiltering( Img, FilteredImg, TRUE );
// //				FilteredImg.Write( "C:\\HorFilter.tif" );
// 				CImageTool::DoHorProject( FilteredImg, Proj, nProj );
// 			}
// 			else
// 				CImageTool::DoHorProject( Img, Proj, nProj );
// 			CImageTool::SmoothProject( Proj, nProj );
// //			CImageTool::DumpProject( "C:\\HorProj.tif", Proj, nProj );
// 
// 			//Decoding HMM
// 			if( DecodeHMM( Proj, nProj, nHorLine, pLinePos, nTotalProb, Model.HLineLenMean, Model.HLineLenVar, Model.HTextLenMean, Model.HTextLenVar, Model.HLineGapDist ) != 0 )
// 			{
// 				printf( "\tDecoding failed!\n" );
// 				continue;
// 			}
// 	
// 			//Get the detected lines
// 			for( i=0; i<nHorLine; i++ )
// 			{
// 				pLine[nLine].StPnt.x = 0;
// 				pLine[nLine].EdPnt.x = w-1;
// 				pLine[nLine].StPnt.y = pLine[nLine].EdPnt.y = pLinePos[i];
// 				nLine ++;
// 			}
// 			free( Proj );
// 		}
// 		if( nVerLine > 0 )
// 		{
// 			//Vertical projection
// 			if( bFilter )
// 			{
// 				DirLine.DSCCFiltering(Img, FilteredImg, FALSE );
// //				FilteredImg.Write( "C:\\VerFilter.tif" );
// 				CImageTool::DoVerProject( FilteredImg, Proj, nProj );
// 			}
// 			else
// 				CImageTool::DoVerProject( Img, Proj, nProj );
// 			CImageTool::SmoothProject( Proj, nProj );
// //			CImageTool::DumpProject( "C:\\VerProj.tif", Proj, nProj, FALSE );
// 
// 			//Decoding HMM
// 			if( DecodeHMM( Proj, nProj, nVerLine, pLinePos, nTotalProb, Model.VLineLenMean, Model.VLineLenVar, Model.VTextLenMean, Model.VTextLenVar, Model.VLineGapDist ) != 0 )
// 			{
// 				printf( "\tDecoding failed!\n" );
// 				continue;
// 			}
// 
// 			//Get the detected lines
// 			for( i=0; i<nVerLine; i++ )
// 			{
// 				pLine[nLine].StPnt.y = 0;
// 				pLine[nLine].EdPnt.y = h-1;
// 				pLine[nLine].StPnt.x = pLine[nLine].EdPnt.x = pLinePos[i];
// 				nLine++;
// 			}
// 			free( Proj );
// 		}
// 
// 		//Calculate the ending points using the crossing information
// 		CalEndPoint( &pLine[0], nHorLine, &pLine[nHorLine], nVerLine, Model.HCross, Model.VCross );
// 		BoundLineEndPoint( pLine, nLine, w, h );
// 		
// 		//Adjust lines using the pixel level information
// 		AdjustLines( Img, pLine, nLine, nSearchRange );
// 
// 		//Calculate the ending points using the crossing information
// 		//Call this function twice to refine the detection accuracy
// 		CalEndPoint( &pLine[0], nHorLine, &pLine[nHorLine], nVerLine, Model.HCross, Model.VCross );
// 		BoundLineEndPoint( pLine, nLine, w, h );
// 
// 		for(i=0; i<nLine; i++)
// 			pLine[i].nType = 1;
// 
// 		//Output line detection result
// 		ChangeFileExt( fnFDR, fnImg, "fdr" );
// 		WriteDetectResult( fnFDR, pLine, nHorLine+nVerLine );
// 
// 		if( GetNextFile( fnImg ) != 0 )
// 			break;
// 	}
// 
// 	// Free memory
// 	delete	pLinePos;
// 	delete	pLine;
// 	FreeHMMModel( Model );
// 	return 0;
// }
