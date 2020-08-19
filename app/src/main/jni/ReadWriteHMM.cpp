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
#include "ReadWriteHMM.h"
#include <math.h>
#include <algorithm>
#include <string.h>


#ifndef	eps
#define	eps		1.0e-16			//Prevent dividing by zero or log(0)
#endif

#define	MAX_CHAR	10000

/****************************************************************************************************
/*	Name:		AllocHMMModel
/*	Function:	Allocate memory for the HMM model
/*	Parameter:	Model	-- HMM model
/*	Return:		0  -- Succeed
/***************************************************************************************************/
int AllocHMMModel( HMM_MODEL &Model)
{
	int nHorLine = Model.nHorLine;
	if( nHorLine > 0 )
	{
		Model.HLineLenMean	= (double*)malloc( sizeof(double)*nHorLine );
		Model.HLineLenVar	= (double*)malloc( sizeof(double)*nHorLine );
		Model.HLineGapDist	= (LINE_GAP*)malloc( sizeof(LINE_GAP)*nHorLine );
		Model.HCross		= (CROSS*)malloc( sizeof(CROSS)*nHorLine );
	}
	else
	{
		Model.HLineLenMean	= NULL;
		Model.HLineLenVar	= NULL;
		Model.HLineGapDist	= NULL;
		Model.HCross		= NULL;
	}

	int nVerLine = Model.nVerLine;
	if( nVerLine > 0 )
	{
		Model.VLineLenMean	= (double*)malloc( sizeof(double)*nVerLine );
		Model.VLineLenVar	= (double*)malloc( sizeof(double)*nVerLine );
		Model.VLineGapDist	= (LINE_GAP*)malloc( sizeof(LINE_GAP)*nVerLine );
		Model.VCross		= (CROSS*)malloc( sizeof(CROSS)*nVerLine );
	}
	else
	{
		Model.VLineLenMean	= NULL;
		Model.VLineLenVar	= NULL;
		Model.VLineGapDist	= NULL;
		Model.VCross		= NULL;
	}

	return 0;
}

/****************************************************************************************************
/*	Name:		FreeHMMModel
/*	Function:	Free memory allocate for the HMM model
/*	Parameter:	Model	-- HMM model
/*	Return:		0  -- Succeed
/***************************************************************************************************/
int FreeHMMModel( HMM_MODEL &Model )
{
	if( Model.HLineLenMean != NULL )
	{
		free( Model.HLineLenMean );
		Model.HLineLenMean = NULL;
	}
	if( Model.HLineLenVar != NULL )
	{
		free( Model.HLineLenVar );
		Model.HLineLenVar = NULL;
	}
	if( Model.HLineGapDist != NULL )
	{
		free( Model.HLineGapDist );
		Model.HLineGapDist = NULL;
	}
	if( Model.HCross != NULL )
	{
		free( Model.HCross );
		Model.HCross = NULL;
	}

	if( Model.VLineLenMean != NULL )
	{
		free( Model.VLineLenMean );
		Model.VLineLenMean = NULL;
	}
	if( Model.VLineLenVar != NULL )
	{
		free( Model.VLineLenVar );
		Model.VLineLenVar = NULL;
	}
	if( Model.VLineGapDist != NULL )
	{
		free( Model.VLineGapDist );
		Model.VLineGapDist = NULL;
	}
	if( Model.VCross != NULL )
	{
		free( Model.VCross );
		Model.VCross = NULL;
	}

	return 0;
}

/****************************************************************************************************
/*	Name:		ReadHMM
/*	Function:	Read HMM model
/*	Parameter:	fpHMM		-- File for HMM model
/*				nLine		-- Number of lines
/*				LineLenMean	-- Mean of line projection
/*				LineLenVar	-- Variance of line projection
/*				TextLenMean	-- Mean of the projection of text
/*				TextLenVar	-- Variance of the projection of text
/*				LineGapProb -- Line gap distribution
/*	Return:		0  -- Succeed
/***************************************************************************************************/
int ReadHMM( FILE *fpHMM, int nLine, double *LineLenMean, double *LineLenVar,
			double &TextLenMean, double &TextLenVar, LINE_GAP LineGapProb[] )
{
	int		i, j;
	char	buf[MAX_CHAR];
	char	*p;
	//Read mean and variance of line projection
	for( i=0; i<nLine; i++ )
	{
		fgets( buf, MAX_CHAR, fpHMM );
		p = strstr( buf, "Mean=" );
		if( p == NULL )		
			return -1;
		sscanf(p, "Mean=%lf", &LineLenMean[i] );
		p = strstr( buf, "Var=" );
		if( p == NULL )		
			return -1;
		sscanf(p, "Var=%lf", &LineLenVar[i] );
	}

	//Read mean and variance of text projection
	fgets( buf, MAX_CHAR, fpHMM );
	p = strstr( buf, "Mean=" );
	if( p == NULL )		return -1;
	sscanf(p, "Mean=%lf", &TextLenMean );
	p = strstr( buf, "Var=" );
	if( p == NULL )		return -1;
	sscanf(p, "Var=%lf", &TextLenVar );

	//Read line gap distribution
	fgets( buf, MAX_CHAR, fpHMM );
	for( i=0; i<nLine-1; i++ )
	{
		fgets( buf, MAX_CHAR, fpHMM );
		fgets( buf, MAX_CHAR, fpHMM );
		int nMean, nMin, nMax;
		p = strstr( buf, "Mean" );
		if( p == NULL )		
			return -1;
		sscanf( p, "Mean=%d", &nMean );
		if( p == NULL )		
			return -1;
		p = strstr( buf, "Min" );
		sscanf( p, "Min=%d", &nMin );
		if( p == NULL )		
			return -1;
		p = strstr( buf, "Max" );
		sscanf( p, "Max=%d", &nMax );
		LineGapProb[i].nMeanGap	= nMean;
		LineGapProb[i].nMinGap	= nMean+nMin;
		LineGapProb[i].nMaxGap	= nMean+nMax;
		memset( LineGapProb[i].nProb,0, sizeof(double)*MAXLINEGAP );

		double	nSumProb = 0;
		for( j=0; j<=nMax-nMin; j++ )
		{
			fscanf( fpHMM, "%lf", &LineGapProb[i].nProb[j] );
			nSumProb += LineGapProb[i].nProb[j];
		}
		//Normalization to get the probability distribution
		//And convert it to logarithm
		for( j=0; j<=LineGapProb[i].nMaxGap - LineGapProb[i].nMinGap; j++ )
			LineGapProb[i].nProb[j] = log( max(eps, LineGapProb[i].nProb[j]/nSumProb) );

		fgets( buf, MAX_CHAR, fpHMM );
	}
	return 0;
}

/****************************************************************************************************
/*	Name:		ReadHMM
/*	Function:	Read HMM model
/*	Parameter:	fpHMM		-- File for HMM model
/*				Model		-- HMM model
/*	Return:		0  -- Succeed
/***************************************************************************************************/
int ReadHMM( char *fnHMM, HMM_MODEL &Model )
{
	Model.nHorLine = Model.nVerLine = 0;
	char	buf[MAX_CHAR];
	FILE *fpHMM = fopen( fnHMM, "rt" );
	if( fpHMM == NULL )
	{
		printf( "Can not open %s to read HMM model!\n" );
		return -1;
	}

	char* subBuf1, *subBuf2;
	// Get the horizontl and vertical line numbers
	while( !feof( fpHMM ) )
	{
		if( fgets( buf, MAX_CHAR, fpHMM ) == NULL )
			break;

		int subStrNum1 = strlen("Horizontal Lines = ");
		int subStrNum2 = strlen("Vertical Lines=");
		subBuf1 = NULL;
		subBuf2 = NULL;
		if (sizeof(buf)>=subStrNum1)
		{
			strncpy(subBuf1, buf, subStrNum1);
			if (strcmp(subBuf1, "Horizontal Lines=")==0)
			{
				sscanf(buf, "Horizontal Lines=%d", &Model.nHorLine);
			}
		}
		if (sizeof(buf)>=subStrNum2)
		{
			strncpy(subBuf2, buf, subStrNum2);
			if (strcmp(subBuf2, "Vertical Lines=")==0)
			{
				sscanf(buf, "Vertical Lines=%d", &Model.nVerLine);
			}
		}
		
// 		if( strnicmp( buf, "Horizontal Lines=", strlen( "Horizontal Lines=" ) ) == 0 )
// 			sscanf( buf, "Horizontal Lines=%d", &Model.nHorLine );
// 		if( strnicmp( buf, "Vertical Lines=", strlen( "Vertical Lines=" ) ) == 0 )
// 			sscanf( buf, "Vertical Lines=%d", &Model.nVerLine );
	}
	rewind( fpHMM );

	//Allocate memory for HMM model
	AllocHMMModel( Model );

	//Read twice since two models may in the same file
	int i = 0;
	for( ; i<2; i++ )
	{
		if( fgets( buf, MAX_CHAR, fpHMM ) == NULL )
			break;
		int nHorLine = 0;
		int nVerLine = 0;
		sscanf( buf, "Horizontal Lines=%d", &nHorLine );
		if( nHorLine > 0 )
			if( ReadHMM( fpHMM, Model.nHorLine, Model.HLineLenMean, Model.HLineLenVar, Model.HTextLenMean, Model.HTextLenVar, Model.HLineGapDist ) != 0 )
				return -1;
		sscanf( buf, "Vertical Lines=%d", &nVerLine );
		if( nVerLine > 0 )
			if( ReadHMM( fpHMM, Model.nVerLine, Model.VLineLenMean, Model.VLineLenVar, Model.VTextLenMean, Model.VTextLenVar, Model.VLineGapDist ) != 0 )
				return -1;
	}

	//Initialization
	for( i=0; i<Model.nHorLine; i++ )
		Model.HCross[i].nStLine = Model.HCross[i].nEdLine = -1;
	for( i=0; i<Model.nVerLine; i++ )
		Model.VCross[i].nStLine = Model.VCross[i].nEdLine = -1;

	//Read crossing information
	if( Model.nHorLine && Model.nVerLine )
	{
		fgets( buf, MAX_CHAR, fpHMM );
		if( strcmp( buf, "Crossing of horizontal lines:\n" ) == 0 )
		{
			for( i=0; i<Model.nHorLine; i++ )
			{
				fgets( buf, MAX_CHAR, fpHMM );
				sscanf( buf, "%d%d", &Model.HCross[i].nStLine, &Model.HCross[i].nEdLine );
				
				fgets( buf, MAX_CHAR, fpHMM );
				char *p = strchr( buf, '(');
				if( p == NULL )	return -1;
				sscanf( p+1, "%d%d", &Model.HCross[i].StOffset.x, &Model.HCross[i].StOffset.y );

				fgets( buf, MAX_CHAR, fpHMM );
				p = strchr( buf, '(');
				if( p == NULL )	return -1;
				sscanf( p+1, "%d%d", &Model.HCross[i].EdOffset.x, &Model.HCross[i].EdOffset.y );	
			}
		}
		fgets( buf, MAX_CHAR, fpHMM );
		if( strcmp( buf, "Crossing of vertical lines:\n" ) == 0 )
		{
			for( i=0; i<Model.nVerLine; i++ )
			{
				fgets( buf, MAX_CHAR, fpHMM );
				sscanf( buf, "%d%d", &Model.VCross[i].nStLine, &Model.VCross[i].nEdLine );
				
				fgets( buf, MAX_CHAR, fpHMM );
				char *p = strchr( buf, '(');
				if( p == NULL )	return -1;
				sscanf( p+1, "%d%d", &Model.VCross[i].StOffset.x, &Model.VCross[i].StOffset.y );

				fgets( buf, MAX_CHAR, fpHMM );
				p = strchr( buf, '(');
				if( p == NULL )	return -1;
				sscanf( p+1, "%d%d", &Model.VCross[i].EdOffset.x, &Model.VCross[i].EdOffset.y );	
			}
		}
	}
	fclose( fpHMM );
	return 0;
}
