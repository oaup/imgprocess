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
// File NAME:		FormID.cpp
// File Function:	Form identification using the HMM model
//
//				Developed by: Yefeng Zheng
//			   First created: Aug. 2004
//			University of Maryland, College Park
///////////////////////////////////////////////////////////////////////////////////////////////////


#include <math.h>
#include <float.h>
#include "ProcForm.h"
#include "GetProcessFile.h"
#include "ImageObj.h"
#include "ImageTool.h"
#include "Tools.h"
#include "ParalLine.h"
#include "DirLine.h"
#include "GroupRunLength.h"



/****************************************************************************************************
/*	Name:		CalLineProb
/*	Function:	Calculate the probablity of one decoding path, only considering projections of lines
/*	Parameter:	Proj		-- Projections
/*				nProj		-- Number of projections
/*				pLinePos	-- Decoded line positions
/*				nLine		-- Number of lines
/*				LineLenMean	-- Mean of the projections of lines
/*				LineLenVar	-- Variance of the projections of lines
/*	Return:		Logarithm of the probability
/***************************************************************************************************/
double CalLineProb( double *Proj, int nProj, int *pLinePos, int nLine, double *LineLenMean, double *LineLenVar, LINE_GAP *LineGap )
{
	double	nTotalProb = 0;
	int		nDist;
	int		nCountLine = 0;
	for( int i=0; i<nLine; i++ )
	{
		if( LineGap[i].nMeanGap == 0 )	
			continue;	// Merged lines are only counted once!
		nTotalProb += GetGaussProbLog( Proj[pLinePos[i]], LineLenMean[i], LineLenVar[i] );
		if( i < nLine-1 )
		{
			nDist		= pLinePos[i+1] - pLinePos[i];
			if( nDist < LineGap[i].nMinGap || nDist > LineGap[i].nMaxGap )
				printf( "Error!" );
			else
			{
				nDist		-= LineGap[i].nMinGap;
				nTotalProb	+= LineGap[i].nProb[nDist];
			}
		}
		nCountLine++;
	}
	return	nTotalProb/nCountLine;
}

/****************************************************************************************************
/*	Name:		ReadHMMList
/*	Function:	Read a list of HMM models. The HMM model files should be in the same directory
/*				of the configuration file, if directory is indicated in the config file
/*	Parameter:	fnFormIdConfig	-- A list file for all HMM models
/*				pModel			-- HMM models read from files
/*				pFormID			-- Form ID (strings)
/*				nModel			-- Number of HMM models
/*	Return:		0  -- Succeed
/***************************************************************************************************/
int ReadHMMList( char *fnFormIdConfig, HMM_MODEL *&pModel, char **&pFormID, int &nModel )
{
	FILE *fp = fopen( fnFormIdConfig, "rt" );
	if( fp == NULL )
	{
		printf( "Can not open %s to read!", fnFormIdConfig );
		return -1;
	}

	// Get the number of HMM models
	char	buf[MAX_PATH];
	nModel = 0;
	while( !feof( fp ) )
	{
		if( fgets( buf, MAX_PATH, fp ) == NULL )
			break;
		if( strstr( buf, "hmm" ) == NULL )
			break;
		nModel ++;
	}
	rewind( fp );
	if( nModel == 0 )
	{
		fclose( fp );
		return -1;
	}

	// Allocate memory
	pModel = new HMM_MODEL[nModel];
	pFormID = (char**)malloc( sizeof(char*)*nModel );
	int i = 0;
	for( ; i<nModel; i++ )
		pFormID[i] = new char[MAX_PATH];

	// Read HMM models
	char	fnHMM[MAX_PATH];
	string	csDir = GetPathName( fnFormIdConfig );
	for( i=0; i<nModel; i++ )
	{
		fgets( buf, MAX_PATH, fp );

		// Remove the character RETURN at the end of the string
		buf[strlen(buf)-1] = '\0';
		if( GetPathName( buf ) == ".\\" )
			sprintf(fnHMM, "%s\\%s", (const char*)csDir.c_str(), buf);
		else
			strcpy( fnHMM, buf );
		if( ReadHMM( fnHMM, pModel[i] ) != 0 )
		{
			printf( "Read HMM model %s failed!", buf );
			return -1;
		}
		char *p = strchr( buf, '.' );
		*p = '\0';
		p = strchr( buf, '\\' );
		if( p == NULL )
			p = buf;
		else
			p++;
		strcpy( pFormID[i], buf );
	}
	fclose( fp );
	return 0;
}

/****************************************************************************************************
/*	Name:		FormIdentify
/*	Function:	Form identification using HMM models
/*	Parameter:	fnFormIdConfig	-- A list file for all HMM models
/*				bFilter			-- Performing text filtering or not
/*	Return:		0  -- Succeed
/***************************************************************************************************/
// int	FormIdentify( char *fnFormIdConfig, int bFilter )
// {
// 	int i;
// 
// 	//Read HMM models
// 	HMM_MODEL	*pModel;
// 	char		**pFormID;
// 	int			nModel;
// 	if( ReadHMMList( fnFormIdConfig, pModel, pFormID, nModel ) != 0 )
// 		return -1;
// 
// 	// Get the maximum number of lines, and allocate memory
// 	int nMaxHorLine = 0;
// 	int nMaxVerLine = 0;
// 	for( i=0; i<nModel; i++ )
// 	{
// 		nMaxHorLine = max( nMaxHorLine, pModel[i].nHorLine );
// 		nMaxVerLine = max( nMaxVerLine, pModel[i].nVerLine );
// 	}
// 	if( nMaxHorLine == 0 && nMaxVerLine == 0 )
// 		return -1;
// 
// 	int		*pHorLinePos		= NULL;
// 	int		*pHorLinePosBest	= NULL;
// 	if( nMaxHorLine > 0 )
// 	{
// 		pHorLinePos		= new int[nMaxHorLine];
// 		pHorLinePosBest	= new int[nMaxHorLine];
// 	}
// 
// 	int	*pVerLinePos		= NULL;
// 	int	*pVerLinePosBest	= NULL;
// 	if( nMaxVerLine > 0 )
// 	{
// 		pVerLinePos		= new int[nMaxVerLine];
// 		pVerLinePosBest	= new int[nMaxVerLine];
// 	}
// 	LINE	*pLine		= new LINE[nMaxHorLine+nMaxVerLine];
// 
// 	// Calculate the searching range
// 	int *pSearchRange	= NULL;
// 	pSearchRange		= new int[nModel];
// 	for( i=0; i<nModel; i++ )
// 		pSearchRange[i] = CalSearchRange( pModel[i] );
// 
// 	// Open a log file
// 	char	fnLog[MAX_PATH];
// 	strcpy( fnLog, "FormID.log" );	
// 	FILE *fpLog = fopen( fnLog, "wt" );
// 	fclose( fpLog );
// 
// 	// Process all files
// 	char fnImg[MAX_PATH];
// 	GetFirstFile( fnImg );
// 	double	*pTotalProb = (double*)malloc( sizeof(double)*nModel );
// 
// 	while( 1 )
// 	{
// 		printf( "Processing %s ...\n", GetFileName( fnImg ) );
// 	
// 		CImage Img;
// 		if( Img.Read( fnImg ) != 0 )
// 		{
// 			printf( "Read image: %s failed!\n", fnImg );
// 			return -1;
// 		}
// 		int w = Img.GetWidth();
// 		int h = Img.GetHeight();
// 
// 		double	*HProj, *VProj;
// 		int		nHProj, nVProj;
// 		CDirLine	DirLine;
// 		CImage		FilteredImg;
// 
// 		//Horizontal projection
// 		if( bFilter )
// 		{
// 			DirLine.DSCCFiltering( Img, FilteredImg, TRUE );
// 			CImageTool::DoHorProject( FilteredImg, HProj, nHProj );
// //			FilteredImg.Write( "C:\\FilterHor.tif" );
// 		}
// 		else
// 			CImageTool::DoHorProject( Img, HProj, nHProj );
// 		CImageTool::SmoothProject( HProj, nHProj );
// //		CImageTool::DumpProject( "C:\\HorProj.tif", HProj, nHProj );
// 
// 		//Vertical projection
// 		if( bFilter )
// 		{
// 			DirLine.DSCCFiltering(Img, FilteredImg, FALSE );
// 			CImageTool::DoVerProject( FilteredImg, VProj, nVProj );
// //			FilteredImg.Write( "C:\\FilterVer.tif" );
// 		}
// 		else
// 			CImageTool::DoVerProject( Img, VProj, nVProj );
// 		CImageTool::SmoothProject( VProj, nVProj );
// //		CImageTool::DumpProject( "C:\\VerProj.tif", VProj, nVProj, FALSE );
// 
// 		// Form identification
// 		double	nHProb, nVProb;
// 		double	nMaxProb	= -1e+20;
// 		int		nID			= -1;
// 		for( i=0; i<nModel; i++ )
// 		{
// 			if( pModel[i].nHorLine > 0 )
// 			{
// 				if( DecodeHMM( HProj, nHProj, pModel[i].nHorLine, pHorLinePos, nHProb, pModel[i].HLineLenMean, 
// 								pModel[i].HLineLenVar, pModel[i].HTextLenMean, pModel[i].HTextLenVar, pModel[i].HLineGapDist ) == 0 )
// 					nHProb = CalLineProb( HProj, nHProj, pHorLinePos, pModel[i].nHorLine, pModel[i].HLineLenMean, pModel[i].HLineLenVar, pModel[i].HLineGapDist);
// 				else
// 					nHProb = -1e+10;
// 
// 			}
// 			else
// 				nHProb = 0;
// 
// 			if( pModel[i].nVerLine > 0 )
// 			{
// 				if( DecodeHMM( VProj, nVProj, pModel[i].nVerLine, pVerLinePos, nVProb, pModel[i].VLineLenMean, 
// 								pModel[i].VLineLenVar, pModel[i].VTextLenMean, pModel[i].VTextLenVar, pModel[i].VLineGapDist ) == 0 )
// 					nVProb = CalLineProb( VProj, nVProj, pVerLinePos, pModel[i].nVerLine, pModel[i].VLineLenMean, pModel[i].VLineLenVar, pModel[i].VLineGapDist );
// 				else
// 					nVProb = -1e+10;
// 			}
// 			else
// 				nVProb = 0;
// 
// 			if( nHProb + nVProb > nMaxProb )
// 			{
// 				nMaxProb = nHProb + nVProb;
// 				nID = i;
// 				if( pHorLinePos != NULL )
// 					memcpy( pHorLinePosBest, pHorLinePos, sizeof(int)*pModel[i].nHorLine );
// 				if( pVerLinePos != NULL )
// 					memcpy( pVerLinePosBest, pVerLinePos, sizeof(int)*pModel[i].nVerLine );
// 			}
// 			pTotalProb[i] = nHProb + nVProb;
// 		}
// 		fpLog = fopen( fnLog, "at" );
// 		fprintf( fpLog, "%s\t\t%s\t", (LPCSTR)GetFileName(fnImg), pFormID[nID] );
// /*		for( i=0; i<nModel; i++ )
// 			fprintf( fpLog, "\t%6.3e", pTotalProb[i] );
// */		fprintf( fpLog, "\n" );
// 		fclose( fpLog );
// 	
// /////////////////////////////////////////////////////////////////////////////////////////
// 		//Get the detected lines
// 		int	nLine = 0;
// 		for( i=0; i<pModel[nID].nHorLine; i++ )
// 		{
// 			pLine[nLine].StPnt.x = 0;
// 			pLine[nLine].EdPnt.x = w-1;
// 			pLine[nLine].StPnt.y = pLine[nLine].EdPnt.y = pHorLinePosBest[i];
// 			nLine ++;
// 		}
// 		for( i=0; i<pModel[nID].nVerLine; i++ )
// 		{
// 			pLine[nLine].StPnt.y = 0;
// 			pLine[nLine].EdPnt.y = h-1;
// 			pLine[nLine].StPnt.x = pLine[nLine].EdPnt.x = pVerLinePosBest[i];
// 			nLine++;
// 		}
// 		//Calculate the ending points using the crossing information
// 		CalEndPoint( &pLine[0], pModel[nID].nHorLine, &pLine[pModel[nID].nHorLine], 
// 					 pModel[nID].nVerLine, pModel[nID].HCross, pModel[nID].VCross );
// 		BoundLineEndPoint( pLine, nLine, w, h );
// 		
// 		//Adjust lines using the pixel level information
// 		AdjustLines( Img, pLine, nLine, pSearchRange[nID] );
// 
// 		//Calculate the ending points using the crossing information
// 		//Call this function twice to refine the detection accuracy
// 		CalEndPoint( &pLine[0], pModel[nID].nHorLine, &pLine[pModel[nID].nHorLine], 
// 					pModel[nID].nVerLine, pModel[nID].HCross, pModel[nID].VCross );
// 		BoundLineEndPoint( pLine, nLine, w, h );
// 
// 		for(i=0; i<nLine; i++)
// 			pLine[i].nType = 1;
// 
// 		//Output line detection result
// 		char	fnFDR[MAX_PATH];
// 		ChangeFileExt( fnFDR, fnImg, "fdr" );
// 		WriteDetectResult( fnFDR, pLine, pModel[nID].nHorLine+pModel[nID].nVerLine );
// 
// 		free( HProj );
// 		free( VProj );
// 		if( GetNextFile( fnImg ) != 0 )
// 			break;
// 	}
// 
// 	// Free memory
// 	if( pHorLinePos != NULL )
// 		delete pHorLinePos;
// 	if( pHorLinePosBest != NULL )
// 		delete pHorLinePosBest;
// 	if( pVerLinePos != NULL )
// 		delete pVerLinePos;
// 	if( pVerLinePosBest != NULL )
// 		delete pVerLinePosBest;
// 	if( pLine != NULL )
// 		delete pLine;
// 	if( pSearchRange != NULL )
// 		delete pSearchRange;
// 
// 	for( i=0; i<nModel; i++ )
// 	{
// 		FreeHMMModel( pModel[i] );
// 		free( pFormID[i] );
// 	}
// 	free( pModel );
// 	free( pFormID );
// 	free( pTotalProb );
// 	return 0;
// }
