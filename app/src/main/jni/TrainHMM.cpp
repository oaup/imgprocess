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
// File NAME:		TrainHMM.cpp
// File Function:	Get parameters for HMM model
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
#include <math.h>
#include "DirLine.h"
#include "ReadWriteHMM.h"
#include <algorithm>


int	GAP_VAR = 9;	// Variance of line gaps. Used to train an HMM model with one sample

//Temporary variables used to calculate the parameters of HMM
double  **HLineGapStat;	//Horizontal line gaps
double	**VLineGapStat;	//Vertical line gaps
int		**HLineProj;	//Projection of horizontal lines
int		**VLineProj;	//Projection of vertical lines
int		**HTextProj;	//Horizontal projection of text
int		**VTextProj;	//Vertical projection of text
int		*nImgWidth;		//Image withs
int		*nImgHeight;	//Image heights 

/*
	Name:		AllocMem
	Function:	Allocate memory
	Parameter:	nImg	 -- Number of images
				nHorLine -- Number of horizontal lines
				nVerLine -- Number of vertical lines
	Return:		0  -- Succeed
*/
int AllocMem( int nImg, int nHorLine, int nVerLine )
{
	int i;
	if( nHorLine > 0 )
	{
		HLineGapStat = (double**)malloc( sizeof(double*)*nImg );
		for( i=0; i<nImg; i++ )
			HLineGapStat[i] = (double*)malloc( sizeof(double)*nHorLine );
		HLineProj = (int**)malloc( sizeof(int*)*nImg );
		HTextProj = (int**)malloc( sizeof(int*)*nImg );
		for( i=0; i<nImg; i++ )
			HLineProj[i] = (int*)malloc(sizeof(int)*nHorLine);
	}
	if( nVerLine > 0 )
	{
		VLineGapStat = (double**)malloc( sizeof(double*)*nImg );
		for( i=0; i<nImg; i++ )
			VLineGapStat[i] = (double*)malloc( sizeof(double)*nVerLine );
		VTextProj = (int**)malloc( sizeof(int*)*nImg );
		VLineProj = (int**)malloc( sizeof(int*)*nImg );
		for( i=0; i<nImg; i++ )
			VLineProj[i] = (int*)malloc(sizeof(int)*nVerLine);
	}

	nImgWidth = (int*)malloc( sizeof(int)*nImg );
	nImgHeight = (int*)malloc( sizeof(int)*nImg );

	return 0;
}

/*
	Name:		FreeMem
	Function:	Free memory
	Parameter:	nImg	 -- Number of images
				nHorLine -- Number of horizontal lines
				nVerLine -- Number of vertical lines
	Return:		0  -- Succeed
*/
int FreeMem( int nImg, int nHorLine, int nVerLine )
{
	int i;
	free( nImgWidth );
	free( nImgHeight );
	if( nHorLine > 0 )
	{
		for( i=0; i<nImg; i++ )
		{
			free( HLineGapStat[i] );
			free( HLineProj[i] );
			free( HTextProj[i] );
		}

		free( HLineGapStat );
		free( HLineProj );
		free( HTextProj );
	}
	
	if( nVerLine > 0 )
	{
		for( i=0; i<nImg; i++ )
		{
			free( VLineProj[i] );
			free( VLineGapStat[i] );
			free( VTextProj[i] );
		}
		free( VLineGapStat );
		free( VLineProj );
		free( VTextProj );
	}	
	return 0;
}

/*
	Name:		OutputHMM
	Function:	Output HMM models
	Parameter:	fnHMM -- File name of HMM model
				Model -- HMM model
	Return:		0  -- Succeed
*/
int OutputHMM( char *fnHMM, HMM_MODEL &Model )
{
	FILE *fp = fopen( fnHMM, "wt" );
	if( fp == NULL )
	{
		printf( "Can not fopen %s to write!\n", fnHMM );
		return -1;
	}
	
	int i, j;
	int nHorLine = Model.nHorLine;
	int nVerLine = Model.nVerLine;

	//Output horizontal HMM model
	if( nHorLine > 0 )
	{
		fprintf( fp, "Horizontal Lines=%d\n", nHorLine );
		//Projection mean and variance
		for( i=0; i<nHorLine; i++ )
			fprintf( fp, "Line %d Mean=%f\tVar=%f\n", i, Model.HLineLenMean[i], Model.HLineLenVar[i] );
		fprintf( fp, "Text Mean=%f\tVar=%f\n", Model.HTextLenMean, Model.HTextLenVar );

		//Line gap distribution
		fprintf( fp, "Line gap distribution\n" );
		for( i=0; i<nHorLine-1; i++ )
		{
			fprintf( fp, "Gap %d\n", i );
			fprintf( fp, "\tMean=%5d  ", (int)Model.HLineGapDist[i].nMeanGap );

			int nMin, nMax;
			for( nMin=0; nMin<MAXLINEGAP; nMin++ )
				if( Model.HLineGapDist[i].nProb[nMin] != 0 )
					break;
			for( nMax=MAXLINEGAP-1; nMax>=0; nMax-- )
				if( Model.HLineGapDist[i].nProb[nMax] != 0 )
					break;
			fprintf( fp, "Min=%5d  Max=%5d\n", nMin-MAXLINEGAP/2, nMax-MAXLINEGAP/2 );
			fprintf( fp, "\t" );
			for( j=nMin; j<=nMax; j++ )
				fprintf( fp, "%5d", (int)Model.HLineGapDist[i].nProb[j] );
			fprintf( fp, "\n" );
		}
	}
	
	//Output vertical HMM model
	if( nVerLine > 0 )
	{
		fprintf( fp, "Vertical Lines=%d\n", nVerLine );
		//Projection mean and variance
		for( i=0; i<nVerLine; i++ )
			fprintf( fp, "Line %d Mean=%f\tVar=%f\n", i, Model.VLineLenMean[i], Model.VLineLenVar[i] );
		fprintf( fp, "Text Mean=%f\tVar=%f\n", Model.VTextLenMean, Model.VTextLenVar );

		//Line gap distribution
		fprintf( fp, "Line gap distribution\n" );
		for( i=0; i<nVerLine-1; i++ )
		{
			fprintf( fp, "Gap %d\n", i );
			fprintf( fp, "\tMean=%5d  ", (int)Model.VLineGapDist[i].nMeanGap );

			int nMin, nMax;
			for( nMin=0; nMin<MAXLINEGAP; nMin++ )
				if( Model.VLineGapDist[i].nProb[nMin] != 0 )
					break;
			for( nMax=MAXLINEGAP-1; nMax>=0; nMax-- )
				if( Model.VLineGapDist[i].nProb[nMax] != 0 )
					break;
			fprintf( fp, "Min=%5d  Max=%5d\n", nMin-MAXLINEGAP/2, nMax-MAXLINEGAP/2 );
			fprintf( fp, "\t" );
			for( j=nMin; j<=nMax; j++ )
				fprintf( fp, "%5d", (int)Model.VLineGapDist[i].nProb[j] );
			fprintf( fp, "\n" );
		}
	}

	//Output crossing information
	if( nHorLine > 0 && nVerLine > 0 )
	{
		fprintf( fp, "Crossing of horizontal lines:\n" );
		for( i=0; i<nHorLine; i++ )
		{
			fprintf( fp, "%d\t%d\n", Model.HCross[i].nStLine, Model.HCross[i].nEdLine );
			fprintf( fp, "\t(%d %d)\n", Model.HCross[i].StOffset.x, Model.HCross[i].StOffset.y );
			fprintf( fp, "\t(%d %d)\n", Model.HCross[i].EdOffset.x, Model.HCross[i].EdOffset.y );
		}
		fprintf( fp, "Crossing of vertical lines:\n" );
		for( i=0; i<nVerLine; i++ )
		{
			fprintf( fp, "%d\t%d\n", Model.VCross[i].nStLine, Model.VCross[i].nEdLine );
			fprintf( fp, "\t(%d %d)\n", Model.VCross[i].StOffset.x, Model.VCross[i].StOffset.y );
			fprintf( fp, "\t(%d %d)\n", Model.VCross[i].EdOffset.x, Model.VCross[i].EdOffset.y );
		}
	}
	fclose( fp );
	return 0;
}

/*
	Name:		GetObservStat
	Function:	Get statistics of the observatoin of HMM, which is horizontal or vertical projection
	Parameter:	Img				-- Image
				pLine			-- Lines
				pMerge			-- Whether a line should be merged with its previous one
				nLine			-- Number of lines
				LineProj		-- Line projection
				TextProj		-- Projection of text
				bHorLine		-- Horizontal projection or vertical projection
	Return:		0  -- Succeed
*/
int GetObservStat( CImage &Img, LINE *pLine, int *pMerge, int nLine, int *LineProj, int *TextProj, int bHorLine )
{
	double	*Proj;
	int		nProj;
	if( bHorLine )
		CImageTool::DoHorProject( Img, Proj, nProj );
	else
		CImageTool::DoVerProject( Img, Proj, nProj );
	CImageTool::SmoothProject( Proj, nProj );

	int nPos;
	int i, j;
	i=0;
	while( i<nLine )
	{
		int nStart	= i;
		int nEnd	= i;
		while( nEnd<nLine )
		{
			if( pMerge[nEnd] == 0 )
				break;
			nEnd++;
		}

		nPos = 0;
		if( bHorLine )
		{
			for( j=nStart; j<=nEnd; j++ )
				nPos += pLine[i].StPnt.y + pLine[i].EdPnt.y;
			nPos /= (nEnd-nStart+1)*2;
		}
		else
		{
			for( j=nStart; j<=nEnd; j++ )
				nPos += pLine[i].StPnt.x + pLine[i].EdPnt.x;
			nPos /= (nEnd-nStart+1)*2;
		}
		int nMaxPeak = nPos;
		for( j=max(0, nPos-POSVAR); j<=min(nProj-1, nPos+POSVAR); j++ )
			if( abs(Proj[j]) > abs(Proj[nMaxPeak]) )
				nMaxPeak = j;
		
		for( j=nStart; j<=nEnd; j++ )
			LineProj[j] = abs(Proj[nMaxPeak]);
		//Negate the projection of lines
		Proj[nMaxPeak] = -abs(Proj[nMaxPeak]);

		i = nEnd+1;
	}

	for( i=0; i<nProj; i++ )
		TextProj[i] = Proj[i];

	free( Proj );
	return 0;
}

/*
	Name:		RegualizeLineGapEstimate
	Function:	Regualize the line gap estimate. Two regualizations are done:
				1. Making the distribution symmetric
				2. Replacing zeros with a small value
	Parameter:	Prob		-- Raw estimates
				nProb		-- Size of the array of Prob
	Return:		Half of the range
*/
int RegualizeLineGapEstimate( double *Prob, int nProb )
{
	int i;
	// Making the distribution symmetric
	double	nMin = 1e+100;
	for( i=1; i<=nProb/2; i++ )
	{
		double	nAver = Prob[i] + Prob[nProb-i];
		Prob[i] = Prob[nProb-i] = nAver;
		if( nAver > 0 )
			nMin = min( nMin, nAver );
	}

	// Replacing zeros with a small value
	for( i=1; i<=nProb/2; i++ )
		if( Prob[i] != 0 )
			break;
//	int nBound = i-max(3, 0.1*(nProb/2-i));	//Extend the boundary a little bit
	int nBound = i-0.1*(nProb/2-i);			//Extend the boundary a little bit
	nBound = max( nBound, 0 );
	for( i=nBound; i<=nProb-nBound; i++ )
		Prob[i] = max( Prob[i], nMin );
	return nProb/2-nBound;
}

/*
	Name:		CalHMMModel
	Function:	Calculate parameters of HMM model
	Parameter:	nImg		-- Number of images
				Model		-- HMM Model
	Return:		0  -- Succeed
*/
int CalHMMModel( int nImg, HMM_MODEL &Model )
{
	int i, j;
	int nHorLine = Model.nHorLine;
	int nVerLine = Model.nVerLine;
	int	nDist;

	if( nHorLine > 0 )
	{
		for( j=0; j<nHorLine-1; j++ )
		{
			Model.HLineGapDist[j].nMeanGap = 0;
			//Mean of horizontal line gap
			for( i=0; i<nImg; i++ )
				Model.HLineGapDist[j].nMeanGap += HLineGapStat[i][j];
			Model.HLineGapDist[j].nMeanGap = floor( 0.5 + 1.0*Model.HLineGapDist[j].nMeanGap / nImg);

			//Distribution of horizontal line gap
			memset( Model.HLineGapDist[j].nProb,0, sizeof(double)*MAXLINEGAP );
			for( i=0; i<nImg; i++ )
			{
				nDist = floor( 0.5 + HLineGapStat[i][j] - Model.HLineGapDist[j].nMeanGap + MAXLINEGAP/2);
				if( nDist < 0 || nDist >= MAXLINEGAP )
				{
					printf( "line gap error\n" );
					exit(-1);
				}
				Model.HLineGapDist[j].nProb[nDist]++;
			}
			int nHalfRange = RegualizeLineGapEstimate( Model.HLineGapDist[j].nProb, MAXLINEGAP );
			Model.HLineGapDist[j].nMinGap = Model.HLineGapDist[j].nMeanGap - nHalfRange;
			Model.HLineGapDist[j].nMaxGap = Model.HLineGapDist[j].nMeanGap + nHalfRange;
		}

		for( j=0; j<nHorLine; j++ )
		{
			//Mean of horizontal line projection
			Model.HLineLenMean[j] = 0;
			for( i=0; i<nImg; i++ )
				Model.HLineLenMean[j] += HLineProj[i][j];
			Model.HLineLenMean[j] /= nImg;

			//Variance of horizontal line projection
			Model.HLineLenVar[j] = 0;
			for( i=0; i<nImg; i++ )
				Model.HLineLenVar[j] += (HLineProj[i][j] - Model.HLineLenMean[j]) * (HLineProj[i][j] - Model.HLineLenMean[j]);
			Model.HLineLenVar[j] = sqrt( 1.0*Model.HLineLenVar[j]/nImg );
		}

		//Mean of the horizontal projection of text
		Model.HTextLenMean = Model.HTextLenVar = 0;
		int		nHText = 0;
		for( i=0; i<nImg; i++ )
		{
			for( j=0; j<nImgHeight[i]; j++ )
			{
				if( HTextProj[i][j] >= 0 )
				{
					Model.HTextLenMean += HTextProj[i][j];
					nHText++;
				}
			}
		}
		Model.HTextLenMean /= nHText;

		//Variance of the horizontal projection of text
		for( i=0; i<nImg; i++ )
		{
			for( j=0; j<nImgHeight[i]; j++ )
			{
				if( HTextProj[i][j] >= 0 )
					Model.HTextLenVar += (HTextProj[i][j] - Model.HTextLenMean) * (HTextProj[i][j] - Model.HTextLenMean);
			}
		}
		Model.HTextLenVar = sqrt( 1.0*Model.HTextLenVar/nHText );
	}

	if( nVerLine > 0 )
	{
		for( j=0; j<nVerLine; j++ )
		{
			Model.VLineGapDist[j].nMeanGap = 0;
			//Mean of vertical line gaps
			for( i=0; i<nImg; i++ )
				Model.VLineGapDist[j].nMeanGap += VLineGapStat[i][j];
			Model.VLineGapDist[j].nMeanGap = floor( 0.5 + 1.0*Model.VLineGapDist[j].nMeanGap/nImg );

			//Distribution of vertical line gaps
			memset( Model.VLineGapDist[j].nProb,0, sizeof(double)*MAXLINEGAP );
			for( i=0; i<nImg; i++ )
			{
				nDist = floor( 0.5 + VLineGapStat[i][j] - Model.VLineGapDist[j].nMeanGap + MAXLINEGAP/2);
				if( nDist < 0 || nDist >= MAXLINEGAP )
				{
					printf( "line gap error\n" );
					exit(-1);
				}
				Model.VLineGapDist[j].nProb[nDist] ++;
			}
			int nHalfRange = RegualizeLineGapEstimate( Model.VLineGapDist[j].nProb, MAXLINEGAP );
			Model.VLineGapDist[j].nMinGap = Model.VLineGapDist[j].nMeanGap - nHalfRange;
			Model.VLineGapDist[j].nMaxGap = Model.VLineGapDist[j].nMeanGap + nHalfRange;
		}
		
		for( j=0; j<nVerLine; j++ )
		{
			//Mean of vertical line projection
			Model.VLineLenMean[j] = 0;
			for( i=0; i<nImg; i++ )
				Model.VLineLenMean[j] += VLineProj[i][j];
			Model.VLineLenMean[j] /= nImg;

			//Variance of vertical line projection
			Model.VLineLenVar[j] = 0;
			for( i=0; i<nImg; i++ )
				Model.VLineLenVar[j] += (VLineProj[i][j] - Model.VLineLenMean[j]) * (VLineProj[i][j] - Model.VLineLenMean[j]);
			Model.VLineLenVar[j] = sqrt( 1.0*Model.VLineLenVar[j]/nImg );
		}

		//Mean of the vertical projection of text
		Model.VTextLenMean = 0;
		int	nVText = 0;
		for( i=0; i<nImg; i++ )
		{
			for( j=0; j<nImgWidth[i]; j++ )
			{
				if( VTextProj[i] >= (int*)0 )
				{
					Model.VTextLenMean += VTextProj[i][j];
					nVText++;
				}
			}
		}
		Model.VTextLenMean /= nVText;

		//Variance of the vertical projection of text
		Model.VTextLenVar = 0;
		for( i=0; i<nImg; i++ )
		{
			for( j=0; j<nImgWidth[i]; j++ )
			{
				if( VTextProj[i] >= (int*)0 )
					Model.VTextLenVar += (VTextProj[i][j] - Model.VTextLenMean) * (VTextProj[i][j] - Model.VTextLenMean);
			}
		}
		Model.VTextLenVar = sqrt( 1.0*Model.VTextLenVar/nVText );
	}
	
	return 0;
}

/*
	Name:		GetCrossRelation
	Function:	Get crossing relationship between horizontal and vertical lines.
				This crossing relationship is used to determined the starting and ending points of
				detected lines.
	Parameter:	HorLine		-- Horizontal lines
			nHorLine	-- Number of horizontal lines
				VerLine		-- Vertical lines
				nVerLine	-- Number of vertical lines
			HCross		-- Crossing information of horizontal lines
				VCross		-- Crossing information of vertical lines
	Return:		0  -- Succeed
*/
int	GetCrossRelation( LINE *HorLine, int nHorLine, LINE *VerLine, int nVerLine, CROSS *HCross, CROSS *VCross )
{
	int i, j;
	double	nDist, D1, D2;
	int		TH = 20;
	DSCC_CPoint	StOffset, EdOffset, CrossPnt;
	// Get crossing information for horizontal lines
	StOffset.y = EdOffset.y = 0;
	for( i=0; i<nHorLine; i++ )
	{
		double	nStMin = 1e+10, nEdMin = 1e+10;
		int		nStMinIndex, nEdMinIndex;
		for( j=0; j<nVerLine; j++ )
		{
			CrossPnt	= GetCrossPoint( HorLine[i].StPnt, VerLine[j].StPnt, VerLine[j].EdPnt );
			nDist		= GetDistance( HorLine[i].StPnt, CrossPnt );
			D1			= CrossPnt.y - VerLine[j].StPnt.y;
			D2			= CrossPnt.y - VerLine[j].EdPnt.y;
			if( D1*D2 > 0 )
				nDist	+= min( fabs(D1), fabs(D2) );

			if( nDist < nStMin )
			{
				nStMin		= nDist;
				nStMinIndex = j;
				StOffset.x	= HorLine[i].StPnt.x - CrossPnt.x;
			}
			CrossPnt	= GetCrossPoint( HorLine[i].EdPnt, VerLine[j].StPnt, VerLine[j].EdPnt );
			nDist		= GetDistance( HorLine[i].EdPnt, CrossPnt );
			D1			= CrossPnt.y - VerLine[j].StPnt.y;
			D2			= CrossPnt.y - VerLine[j].EdPnt.y;
			if( D1*D2 > 0 )
				nDist	+= min( fabs(D1), fabs(D2) );

			if( nDist < nEdMin )
			{
				nEdMin		= nDist;
				nEdMinIndex = j;
				EdOffset.x	= HorLine[i].EdPnt.x - CrossPnt.x;
			}
		}

		HCross[i].nStLine	= nStMinIndex;
		if( nStMin < TH )
			HCross[i].StOffset	= DSCC_CPoint(0,0);
		else
			HCross[i].StOffset	= StOffset;

		HCross[i].nEdLine = nEdMinIndex;
		if( nEdMin < TH )
			HCross[i].EdOffset	= DSCC_CPoint(0,0);
		else
			HCross[i].EdOffset	= EdOffset;
	}

	// Get crossing information for vertical lines
	StOffset.x = EdOffset.x = 0;
	for( i=0; i<nVerLine; i++ )
	{
		double	nStMin = 1e+10, nEdMin = 1e+10;
		int		nStMinIndex, nEdMinIndex;
		for( j=0; j<nHorLine; j++ )
		{
			CrossPnt	= GetCrossPoint( VerLine[i].StPnt, HorLine[j].StPnt, HorLine[j].EdPnt );
			nDist		= GetDistance( VerLine[i].StPnt, CrossPnt );
			D1			= CrossPnt.x - HorLine[j].StPnt.x;
			D2			= CrossPnt.x - HorLine[j].EdPnt.x;
			if( D1*D2 > 0 )
				nDist	+= min( fabs(D1), fabs(D2) );

			if( nDist < nStMin )
			{
				nStMin		= nDist;
				nStMinIndex = j;
				StOffset.y	= VerLine[i].StPnt.y - CrossPnt.y;
			}
			CrossPnt	= GetCrossPoint( VerLine[i].EdPnt, HorLine[j].StPnt, HorLine[j].EdPnt );
			nDist		= GetDistance( VerLine[i].EdPnt, CrossPnt );
			D1			= CrossPnt.x - HorLine[j].StPnt.x;
			D2			= CrossPnt.x - HorLine[j].EdPnt.x;
			if( D1*D2 > 0 )
				nDist	+= min( fabs(D1), fabs(D2) );

			if( nDist < nEdMin )
			{
				nEdMin		= nDist;
				nEdMinIndex = j;
				EdOffset.y	= VerLine[i].EdPnt.y - CrossPnt.y;
			}
		}
		VCross[i].nStLine = nStMinIndex;
		if( nStMin < TH )
			VCross[i].StOffset	= DSCC_CPoint(0, 0);
		else
			VCross[i].StOffset	= StOffset;

		VCross[i].nEdLine = nEdMinIndex;
		if( nEdMin < TH )
			VCross[i].EdOffset	= DSCC_CPoint(0, 0);
		else
			VCross[i].EdOffset	= EdOffset;
	}
	return 0;
}

/*
	Name:		GetLineNumber
	Function:	Get the number of horizontal and vertical lines.
				This line numbers are used to allocate memory.
	Parameter:	fnFDR		-- Ground truth file name
				nOption		-- Get statistics of horizontal or/and vertical HMM parameters
				nHorLine	-- Number of horizontal lines
				nVerLine	-- Number of vertical lines
				HCross		-- Crossing information of horizontal lines to vertical lines
				VCross		-- Crossing information of vertical lines to horizontal lines
	Return:		0  -- Succeed
*/
int GetLineNumber( char *fnFDR, int nOption, int &nHorLine, int &nVerLine, CROSS *&HCross, CROSS *&VCross )
{
	LINE *pLine, *HorLine=NULL, *VerLine=NULL;
	int  nLine;
	ReadDetectResult( fnFDR, pLine, nLine );
	switch( nOption )
	{
	case HOR_LINE:
		GetHorLine( pLine, nLine, HorLine, nHorLine );
		break;
	case VER_LINE:
		GetVerLine( pLine, nLine, VerLine, nVerLine );
		break;
	case ALL_LINE:
		GetHorLine( pLine, nLine, HorLine, nHorLine );
		GetVerLine( pLine, nLine, VerLine, nVerLine );
		// Get crossing relationship between horizontal and vertical lines
		if( nHorLine > 0 && nVerLine > 0 )
		{
			HCross = (CROSS*)malloc( sizeof(CROSS)*nHorLine );
			VCross = (CROSS*)malloc( sizeof(CROSS)*nVerLine );
			GetCrossRelation( HorLine, nHorLine, VerLine, nVerLine, HCross, VCross );
		}
		break;
	};
	if( HorLine != NULL )
		free( HorLine );
	if( VerLine != NULL )
		free( VerLine );
	free( pLine );
	return 0;
}

/*
	Name:		GetLineGapStat
	Function:	Get statistics of line gaps
	Parameter:	HorLine			-- Horizontal lines
				pHorMerge		-- Line merging flags
				nHorLine		-- Number of horizontal lines
				VerLine			-- Vertical lines
			pVerMerge		-- Line mergeing flags
				nVerLine		-- Number of vertical lines
				HLineGapStat	-- Line gaps between horizontal lines
				VLineGapStat    -- Line gaps between vertical lines
	Return:		0  -- Succeed
*/
int GetLineGapStat( LINE *HorLine, int *pHorMerge, int nHorLine, LINE *VerLine, int *pVerMerge, int nVerLine, double *HLineGapStat, double *VLineGapStat )
{
	int i, j;
	int nPrevPos, nPos;
	int nPrevLine;
	int nStart, nEnd;
	// Get line gap statistics of horizontal lines
	if( HorLine != NULL )
	{
		i=0;
		nPrevLine = -1;
		while( i<nHorLine )
		{
			nStart	= i;
			nEnd	= i;
			if( pHorMerge != NULL )
			{
				while( nEnd<nHorLine )
				{
					if( pHorMerge[nEnd] == 0 )
						break;
					nEnd++;
				}
			}
			nPos = 0;
			for( j=nStart; j<=nEnd; j++ )
				nPos += HorLine[j].StPnt.y + HorLine[j].EdPnt.y;
			nPos /= (nEnd-nStart+1)*2;

			if( nPrevLine != -1 )
				HLineGapStat[nPrevLine] = nPos-nPrevPos;
			for( j=nPrevLine+1; j<=nEnd-1; j++ )
				HLineGapStat[j] = 0;

			i			= nEnd+1;
			nPrevPos	= nPos;
			nPrevLine	= nEnd;
		}
	}

	// Get line gap statistics of vertical lines
	if( VerLine != NULL )
	{
		i=0;
		nPrevLine = -1;
		while( i<nVerLine )
		{
			nStart	= i;
			nEnd	= i;
			if( pVerMerge != NULL )
			{
				while( nEnd<nVerLine )
				{
					if( pVerMerge[nEnd] == 0 )
						break;
					nEnd++;
				}
			}
			nPos = 0;
			for( j=nStart; j<=nEnd; j++ )
				nPos += VerLine[j].StPnt.x + VerLine[j].EdPnt.x;
			nPos /= (nEnd-nStart+1)*2;

			if( nPrevLine != -1 )
				VLineGapStat[nPrevLine] = nPos-nPrevPos;
			for( j=nPrevLine+1; j<=nEnd-1;j++ )
				VLineGapStat[j] = 0;

			i			= nEnd+1;
			nPrevPos	= nPos;
			nPrevLine	= nEnd;
		}
	}

	return 0;
}

/*
	Name:		GetLineGapStat
	Function:	Get statistics of line gaps
	Parameter:	fnFDR		-- Ground truth file name
			nImg		-- Current image serial number
				nOption		-- Get statistics of horizontal or/and vertical HMM parameters
				nHorLine	-- Number of horizontal lines
				nVerLine	-- Number of vertical lines
	Return:		0  -- Succeed
*/
int GetLineGapStat( char *fnFDR, int nImg, int nOption, int nHorLine, int nVerLine )
{
	LINE *pLine;
	int nLine;
	if( ReadDetectResult(fnFDR, pLine, nLine ) != 0 )
	{
		printf( "Read line detection result failed!\n", fnFDR );
		return -1;
	}
		
	LINE *HorLine = NULL, *VerLine = NULL;
	int  nCurHorLine, nCurVerLine;

	switch( nOption )
	{
	case HOR_LINE:
		//Get statistics of horizontal HMM
		GetHorLine( pLine, nLine, HorLine, nCurHorLine );
		if( nHorLine != nCurHorLine )
		{
			printf( "%s has different number of lines with others!\n", fnFDR );
			exit(-1);
		}
		GetLineGapStat( HorLine, NULL, nHorLine, NULL, NULL, 0, HLineGapStat[nImg], NULL);
		break;
	case VER_LINE:
		//Get statistics of vertical HMM
		GetVerLine( pLine, nLine, VerLine, nCurVerLine );
		if( nVerLine != nCurVerLine )
		{
			printf( "%s has different number of lines with others!\n", fnFDR );
			exit(-1);
		}
		GetLineGapStat( NULL, NULL, 0, VerLine, NULL, nVerLine, NULL, VLineGapStat[nImg] );
		break;
	case ALL_LINE:
		//Get statistics of both horizontal and vertical HMM
		GetHorLine( pLine, nLine, HorLine, nCurHorLine );
		GetVerLine( pLine, nLine, VerLine, nCurVerLine );
		if( nHorLine != nCurHorLine || nVerLine != nCurVerLine )
		{
			printf( "%s has different number of lines with others!\n", fnFDR );
			exit(-1);
		}
		GetLineGapStat( HorLine, NULL, nHorLine, VerLine, NULL, nVerLine, HLineGapStat[nImg], VLineGapStat[nImg] );
		break;
	};

	//Free memory
	if( HorLine != NULL )
		free( HorLine );
	if( VerLine != NULL )
		free( VerLine );
	free( pLine );

	return 0;
}

/*
	Name:		SetLineMergeFlag
	Function:	Merge lines if they lie on the same line
	Parameter:	nImg		-- Number of training images
				pHorMerge	-- Array indicating whether a horizontal line should be merged with its previous line
								1 -- merge          0 -- do not merge
				nHorLine    -- Number of horizontal lines
        	pVerMerge	-- Array indicating whether a vertical line should be merged with its previous line
								1 -- merge          0 -- do not merge
				nVerLine    -- Number of vertical lines
Return:		0  -- Succeed
*/
int	SetLineMergeFlag( int nImg, int *pHorMerge, int nHorLine, int *pVerMerge, int nVerLine )
{
	int i, j;
	int	TH_GAP = 6;

	//Processing horizontal lines
	memset( pHorMerge,0, sizeof(int)*nHorLine );
	if( pHorMerge != NULL )
	{
		for( j=0; j<nHorLine-1; j++ )
		{
			double nMeanGap = 0;
			//Mean of horizontal line gap
			for( i=0; i<nImg; i++ )
				nMeanGap += HLineGapStat[i][j];
			nMeanGap = floor( 0.5 + nMeanGap / nImg);
			if( nMeanGap <= TH_GAP )
				pHorMerge[j]	= 1;
		}
	}

	//Processing vertical lines
	memset( pVerMerge,0, sizeof(int)*nVerLine );
	if( pVerMerge != NULL )
	{
		for( j=0; j<nVerLine-1; j++ )
		{
			double nMeanGap = 0;
			//Mean of horizontal line gap
			for( i=0; i<nImg; i++ )
				nMeanGap += VLineGapStat[i][j];
			nMeanGap = floor( 0.5 + nMeanGap / nImg);
			if( nMeanGap <= TH_GAP )
				pVerMerge[j]	= 1;
		}
	}
	return 0;
}

/*
	Name:		GetStat
	Function:	Get statistics of projections and line gaps to calculate HMM parameters.
	Parameter:	fnImg		-- Image file name
				fnFDR		-- Ground truth file name
				nImg		-- Current image serial number
				nOption		-- Get statistics of horizontal or/and vertical HMM parameters
				bFilter		-- TRUE  -- Perform text filtering before projection	FALSE -- Do not perform text filtering
				pHorMerge	-- Whether a horizontal line should be merged with its previous one
				nHorLine	-- Number of horizontal lines
				nVerMerge	-- Whether a vertical line should be merged with its previous one
				nHorLine	-- Number of vertical lines
	Return:		0  -- Succeed
/***************************************************************************************************/
// int GetStat( char *fnImg, char *fnFDR, int nImg, int nOption, int bFilter, int *pHorMerge, int nHorLine, int *pVerMerge, int nVerLine )
// {
// 	CImage Img;
// 	if( Img.Read( fnImg ) != 0 )
// 	{
// 		printf( "Can not open image: %s to read!\n" );
// 		return -1;
// 	}
// 	nImgWidth[nImg] = Img.GetWidth();
// 	nImgHeight[nImg] = Img.GetHeight();
// 
// 	LINE *pLine;
// 	int nLine;
// 	if( ReadDetectResult(fnFDR, pLine, nLine ) != 0 )
// 	{
// 		printf( "Read line detection result failed!\n", fnFDR );
// 		return -1;
// 	}
// 		
// 	LINE *HorLine = NULL, *VerLine = NULL;
// 	int  nCurHorLine, nCurVerLine;
// 	CDirLine	DirLine;
// 	CImage		FilteredImg;
// 
// 	switch( nOption )
// 	{
// 	case HOR_LINE:
// 		//Get statistics of horizontal HMM
// 		GetHorLine( pLine, nLine, HorLine, nCurHorLine );
// 		if( nHorLine != nCurHorLine )
// 		{
// 			printf( "Image %s has different number of lines with others!\n", fnImg );
// 			exit(-1);
// 		}
// 		GetLineGapStat( HorLine, pHorMerge, nHorLine, NULL, NULL, 0, HLineGapStat[nImg], NULL);
// 
// 		HTextProj[nImg] = (int*)malloc( sizeof(int)*Img.GetHeight() );
// 		if( bFilter )
// 		{
// 			DirLine.DSCCFiltering( Img, FilteredImg, TRUE );
// 			GetObservStat( FilteredImg, HorLine, pHorMerge, nHorLine, HLineProj[nImg], HTextProj[nImg], TRUE );
// 		}
// 		else
// 			GetObservStat( Img, HorLine, pHorMerge, nHorLine, HLineProj[nImg], HTextProj[nImg], TRUE );
// 		break;
// 	case VER_LINE:
// 		//Get statistics of vertical HMM
// 		GetVerLine( pLine, nLine, VerLine, nCurVerLine );
// 		if( nVerLine != nCurVerLine )
// 		{
// 			printf( "Image %s has different number of lines with others!\n", fnImg );
// 			exit(-1);
// 		}
// 		GetLineGapStat( NULL, NULL, 0, VerLine, pVerMerge, nVerLine, NULL, VLineGapStat[nImg] );
// 
// 		VTextProj[nImg] = (int*)malloc( sizeof(int)*Img.GetWidth() );
// 		if( bFilter )
// 		{
// 			DirLine.DSCCFiltering( Img, FilteredImg, FALSE );
// 			GetObservStat( FilteredImg, VerLine, pVerMerge, nVerLine, VLineProj[nImg], VTextProj[nImg], FALSE );
// 		}
// 		else
// 			GetObservStat( Img, VerLine, pVerMerge, nVerLine, VLineProj[nImg], VTextProj[nImg], FALSE );
// 		break;
// 	case ALL_LINE:
// 		//Get statistics of both horizontal and vertical HMM
// 		GetHorLine( pLine, nLine, HorLine, nCurHorLine );
// 		GetVerLine( pLine, nLine, VerLine, nCurVerLine );
// 		if( nHorLine != nCurHorLine || nVerLine != nCurVerLine )
// 		{
// 			printf( "Image %s has different number of lines with others!\n", fnImg );
// 			exit(-1);
// 		}
// 		GetLineGapStat( HorLine, pHorMerge, nHorLine, VerLine, pVerMerge, nVerLine, HLineGapStat[nImg], VLineGapStat[nImg] );
// 
// 		//Filter
// 		HTextProj[nImg] = (int*)malloc( sizeof(int)*Img.GetHeight() );
// 		if( bFilter )
// 		{
// 			DirLine.DSCCFiltering( Img, FilteredImg, TRUE );
// 			GetObservStat( FilteredImg, HorLine, pHorMerge, nHorLine, HLineProj[nImg], HTextProj[nImg], TRUE );
// 		}
// 		else
// 			GetObservStat( Img, HorLine, pHorMerge, nHorLine, HLineProj[nImg], HTextProj[nImg], TRUE );
// 		//Filter
// 		VTextProj[nImg] = (int*)malloc( sizeof(int)*Img.GetWidth() );
// 		if( bFilter )
// 		{
// 			DirLine.DSCCFiltering( Img, FilteredImg, FALSE );
// 			GetObservStat( FilteredImg, VerLine, pVerMerge, nVerLine, VLineProj[nImg], VTextProj[nImg], FALSE );
// 		}
// 		else
// 			GetObservStat( Img, VerLine, pVerMerge, nVerLine, VLineProj[nImg], VTextProj[nImg], FALSE );
// 		break;
// 	};
// 
// 	//Free memory
// 	if( HorLine != NULL )
// 		free( HorLine );
// 	if( VerLine != NULL )
// 		free( VerLine );
// 	free( pLine );
// 
// /*	char fnFiltered[MAX_PATH];
// 	sprintf( fnFiltered, "C:\\%s", GetFileName( fnImg ) );
// 	FilteredImg.Write( fnFiltered );
// */	return 0;
// }

/*
	Name:		TrainHMM1Sample
	Function:	Train HMM models with one training sample
	Parameter:	fnHMM		-- HMM model file name
				nLineOption -- Horizontal HMM or/and vertical HMM
				bFilter     -- TRUE  -- Text filtering before projection
							   FALSE -- Do not need text filtering
	Return:		0  -- Succeed
*/
// int TrainHMM1Sample( char *fnHMM, int nLineOption, int bFilter )
// {
// 	HMM_MODEL	Model;
// 	char	fnFDR[MAX_PATH];
// 	char	fnImg[MAX_PATH];
// 
// 	//Get the number of images for training
// 	if( GetFirstFile( fnImg ) != 0 )
// 		return -1;
// 
// 	ChangeFileExt( fnFDR, fnImg, "fdr" );
// 	if( FileExist( fnFDR ) == FALSE )
// 	{
// 		printf( "Groundtruth of image %s does not exist", fnImg );
// 		exit( -1 );
// 	}
// 
// 	//Get the number of horizontal and vertical lines
// 	CROSS *HCross = NULL;
// 	CROSS *VCross = NULL;
// 	GetLineNumber( fnFDR, nLineOption, Model.nHorLine, Model.nVerLine, HCross, VCross );
// 
// 	//Allocate memory for the HMM model
// 	AllocHMMModel( Model );
// 	if( Model.nHorLine > 0 && Model.nVerLine > 0 )
// 	{
// 		memcpy(	Model.HCross, HCross, sizeof(CROSS)*Model.nHorLine );
// 		memcpy( Model.VCross, VCross, sizeof(CROSS)*Model.nVerLine );
// 	}
// 
// 	//Allocate memory
// 	AllocMem( 1, Model.nHorLine, Model.nVerLine );
// 
// 	// Get line gap statistics
// 	GetLineGapStat( fnFDR, 0, nLineOption, Model.nHorLine, Model.nVerLine );
// 
// 	// Merge lines
// 	int *pHorMerge = NULL;
// 	if( Model.nHorLine > 0 )
// 		pHorMerge = new int[Model.nHorLine];
// 	int *pVerMerge = NULL;
// 	if( Model.nVerLine > 0 )
// 		pVerMerge = new int[Model.nVerLine];
// 	SetLineMergeFlag( 1, pHorMerge, Model.nHorLine, pVerMerge, Model.nVerLine );
// 
// 	//Get statistics
// 	GetStat( fnImg, fnFDR, 0, nLineOption, bFilter, pHorMerge, Model.nHorLine, pVerMerge, Model.nVerLine );
// 
// 	//Calculate parameters of HMM
// 	CalHMMModel( 1, Model );
// 
// 	//Set variance of line projection
// 	int i, j;
// 	for( i=0; i<Model.nHorLine; i++ )
// 		Model.HLineLenVar[i] = 0.2*Model.HLineLenMean[i];
// 
// 	for( i=0; i<Model.nVerLine; i++ )
// 		Model.VLineLenVar[i] = 0.2*Model.VLineLenMean[i];
// 
// 	//Set line gap distribution
// 	for( i=0; i<Model.nHorLine-1; i++ )
// 	{
// 		ZeroMemory( Model.HLineGapDist[i].nProb, sizeof(double)*MAXLINEGAP );
// 		if( pHorMerge[i] == 1 )
// 			Model.HLineGapDist[i].nProb[MAXLINEGAP/2] = 1;
// 		else
// 		{
// 			for( j=-GAP_VAR; j<=GAP_VAR; j++ ) 
// 				Model.HLineGapDist[i].nProb[j+MAXLINEGAP/2] = GAP_VAR + 1 - fabs((float)j);
// 		}
// 	}
// 
// 	for( i=0; i<Model.nVerLine-1; i++ )
// 	{
// 		ZeroMemory( Model.VLineGapDist[i].nProb, sizeof(double)*MAXLINEGAP );
// 		if( pVerMerge[i] == 1 )
// 			Model.VLineGapDist[i].nProb[MAXLINEGAP/2] = 1;
// 		else
// 		{
// 			for( j=-GAP_VAR; j<=GAP_VAR; j++)
// 				Model.VLineGapDist[i].nProb[j+MAXLINEGAP/2] = GAP_VAR + 1 - fabs((float)j);
// 		}
// 	}
// 
// 	//Output HMM model
// 	OutputHMM( fnHMM, Model );
// 
// 	// Free HMM model
// 	FreeHMMModel( Model );
// 	//Free memory
// 	if( pHorMerge != NULL )
// 		delete pHorMerge;
// 	if( pVerMerge != NULL )
// 		delete pVerMerge;
// 	FreeMem( 1, Model.nHorLine, Model.nVerLine);
// 	if( HCross != NULL )
// 		free( HCross );
// 	if( VCross != NULL )
// 		free( VCross );
// 	return 0;
// }

/*
	Name:		TrainHMM
	Function:	Train HMM models
	Parameter:	fnHMM		-- HMM model file name
				nLineOption -- Horizontal HMM or/and vertical HMM
			bFilter     -- TRUE  -- Text filtering before projection
							   FALSE -- Do not need text filtering
	Return:		0  -- Succeed
*/
//  int TrainHMM( char *fnHMM, int nLineOption, int bFilter )
// {
// 	HMM_MODEL	Model;
// 	char	fnFDR[MAX_PATH];
// 	char	fnImg[MAX_PATH];
// 
// 	//Get the number of images for training
// 	int		nImg = 0;
// 	if( GetFirstFile( fnImg ) != 0 )
// 		return -1;
// 
// 	while( 1 )
// 	{ 
// 		nImg++;
// 		ChangeFileExt( fnFDR, fnImg, "fdr" );
// 		if( FileExist( fnFDR ) == FALSE )
// 		{
// 			printf( "Groundtruth of image %s does not exist", fnImg );
// 			exit( -1 );
// 		}
// 		//Get next file
// 		if( GetNextFile( fnImg ) != 0 )
// 			break;
// 	}
// 
// 	// Training with one sample
// 	if( nImg == 1 )
// 		return TrainHMM1Sample( fnHMM, nLineOption, bFilter );
// 
// 	// Get the first image
// 	if( GetFirstFile( fnImg ) != 0 )
// 		return -1;
// 
// 	//Get the number of horizontal and vertical lines
// 	ChangeFileExt( fnFDR, fnImg, "fdr" );
// 	CROSS *HCross = NULL;
// 	CROSS *VCross = NULL;
// 	GetLineNumber( fnFDR, nLineOption, Model.nHorLine, Model.nVerLine, HCross, VCross );
// 
// 	// Allocate memory for the HMM model
// 	AllocHMMModel( Model );
// 	if( Model.nHorLine > 0 && Model.nVerLine > 0 )
// 	{
// 		memcpy(	Model.HCross, HCross, sizeof(CROSS)*Model.nHorLine );
// 		memcpy( Model.VCross, VCross, sizeof(CROSS)*Model.nVerLine );
// 	}
// 
// 	//Allocate memory
// 	AllocMem( nImg, Model.nHorLine, Model.nVerLine );
// 
// 	//Get line gap statistics
// 	nImg = 0;
// 	while( 1 )
// 	{ 
// 		ChangeFileExt( fnFDR, fnImg, "fdr" );
// 		// Get line gap statistics
// 		GetLineGapStat( fnFDR, nImg, nLineOption, Model.nHorLine, Model.nVerLine );
// 
// 		nImg++;
// 		//Get next file
// 		if( GetNextFile( fnImg ) != 0 )
// 			break;
// 	}
// 	// Merge lines
// 	int *pHorMerge = NULL;
// 	if( Model.nHorLine > 0 )
// 		pHorMerge = new int[Model.nHorLine];
// 	int *pVerMerge = NULL;
// 	if( Model.nVerLine > 0 )
// 		pVerMerge = new int[Model.nVerLine];
// 	SetLineMergeFlag( nImg, pHorMerge, Model.nHorLine, pVerMerge, Model.nVerLine );
// 
// 	// Get projection statistics
// 	nImg = 0;
// 	if( GetFirstFile( fnImg ) != 0 )
// 		return -1;
// 	while( 1 )
// 	{ 
// 		printf( "Processing %s...\n", GetFileName(fnImg) );
// 		ChangeFileExt( fnFDR, fnImg, "fdr" );
// 		//Get statistics of projections
// 		GetStat( fnImg, fnFDR, nImg, nLineOption, bFilter, pHorMerge, Model.nHorLine, pVerMerge, Model.nVerLine );
// 
// 		nImg++;
// 		//Get next file
// 		if( GetNextFile( fnImg ) != 0 )
// 			break;
// 	}
// 
// 	//Calculate parameters of HMM
// 	CalHMMModel( nImg, Model );
// 
// 	//Output HMM model
// 	OutputHMM( fnHMM, Model );
// 
// 	// Free HMM model
// 	FreeHMMModel( Model );
// 	//Free memory
// 	if( pHorMerge != NULL )
// 		delete pHorMerge;
// 	if( pVerMerge != NULL )
// 		delete pVerMerge;
// 	FreeMem( nImg, Model.nHorLine, Model.nVerLine);
// 	if( HCross != NULL )
// 		free( HCross );
// 	if( VCross != NULL )
// 		free( VCross );
// 
// 	return 0;
// }