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
// File NAME:		ProcForm.cpp
// File Function:	Prototypes of some functions
//
//				Developed by: Yefeng Zheng
//			   First created: Aug. 2003
//			University of Maryland, College Park
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef	__PROC_FORM_H__
#define __PROC_FORM_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ReadWriteHMM.h"
#include "ParalLine.h"

#define	HOR_LINE	1	// Only train the HMM model for horizontal lines
#define	VER_LINE	2	// Only train the HMM model for vertical lines
#define	ALL_LINE	3	// Train two HMM models, one for horizontal lines and the other for vertical lines

#define	OPER_IDLE		0	// No operation
#define	OPER_TRAIN		1	// Train HMM models
#define	OPER_DETECT		2	// Line detection
#define	OPER_FORM_ID	3	// Form identification followed by line detection

#define	POSVAR			3	// Presicion of groundtruthing. Used to train HMM model only.

int		TrainHMM( char *fnHMM, int nLineOption, int bFilter );
//int		DetectLine( char *fnHMM, int bFilter );
int		FormIdentify( char *fnFormIdConfig, int bFilter );
int		DecodeHMM( double *Proj, int nProj, int nLine, int *pLinePos, double &TotalProb, double *LineLenMean, double *LineLenVar, double TextLenMean, double TextLenVar, LINE_GAP LineGapProb[]);
int		ReadHMM( char *fnHMM, HMM_MODEL &Model );
double	GetGaussProbLog( int x, double m, double v );
int		BoundLineEndPoint( LINE *pLine, int nLine, int w, int h );
int		CalEndPoint( LINE *HorLine, int nHorLine, LINE *VerLine, int nVerLine, CROSS *HCross, CROSS *VCross );
int		CalSearchRange( HMM_MODEL &Model );

#endif // __PROC_FORM_H__
