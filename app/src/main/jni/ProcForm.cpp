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
// File Function:	Model based line detection for batch form processing
//
//				Developed by: Yefeng Zheng
//			   First created: Aug. 2003
//			University of Maryland, College Park
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ProcForm.h"
#include "GetProcessFile.h"
#include "Tools.h"


//CWinApp theApp;

extern int GAP_VAR;

/*****************************************************************************
/*	Name:		Usage
/*	Function:	Print help
/*	Parameter:	NULL
/*	Return:		NULL
/*****************************************************************************/
void Usage( )
{
	printf( "Usage: ProcForm [-h] [-t] [-i] [-d] HMM_file img_file\n" );
	printf( "       -t  -- Train an HMM model\n" );
	printf( "       -d  -- Line detection\n" );
	printf( "       -i  -- Form identification followed by line detection\n" );
	printf( "              with the automatically selected model.\n" );
	printf( " HMM_file  -- HMM model file or a list file of all HMM model files\n" );
	printf( " img_file  -- Image file, such as 1.tiff, *.tiff, etc.\n" );
	printf( "       -h  -- Help\n" );
	return;
}

/*****************************************************************************
/*	Name:		_tmain
/*	Function:	Main function of the program
/*	Parameter:	
/*	Return:		
/*****************************************************************************/
//int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
//{
//	int		nOperation = OPER_IDLE;
//	int		nLineOption = ALL_LINE;
//	char	fnHMM[MAX_PATH];
//	char	fnFormIdConfig[MAX_PATH];
//
//	strcpy( fnHMM, "" );
//	strcpy( fnFormIdConfig, "" );
//	int	bFilter = TRUE;
//
////Parse command line
//	int i = 1;
//	for(;i<argc;i++)
//	{
//		if(argv[i][0] != '-') break;
//		++i;
//		switch(tolower(argv[i-1][1]))
//		{
//		case 'g':
//			GAP_VAR = atoi( argv[i] );
//			break;
//		case 't':
//			if( nOperation != OPER_IDLE )
//			{
//				printf( "Wrong operation!\n" );
//				Usage();
//			}
//			nOperation = OPER_TRAIN;
//			i--;
//			break;
//		case 'd':
//			if( nOperation != OPER_IDLE )
//			{
//				printf( "Wrong operation!\n" );
//				Usage();
//			}
//			nOperation = OPER_DETECT;
//			i--;
//			break;
//		case 'i':
//			if( nOperation != OPER_IDLE )
//			{
//				printf( "Wrong operation!\n" );
//				Usage();
//			}
//			nOperation = OPER_FORM_ID;
//			i--;
//			break;
//		case 'l':
//			bFileList = TRUE;
//			i--;
//			break;
//		case '?':
//			Usage();
//			return 0;
//		default:
//			printf( "Input parameters error!\n" );
//			Usage();
//			return -1;
//		}
//	}
//	if( i != argc-2 )
//	{
//		printf( "Input parameters error!\n" );
//		Usage();
//		return -1;
//	}
//	if( nOperation == OPER_IDLE )
//	{
//		printf( "Please input operation type!\n" );
//		Usage();
//	}
//	if( nOperation == OPER_TRAIN || nOperation == OPER_DETECT )
//		strcpy( fnHMM, argv[i] );
//	if( nOperation == OPER_FORM_ID )
//		strcpy( fnFormIdConfig, argv[i] );
//	strcpy( fnFile, argv[i+1] );
//
//	// Processing
//	switch(nOperation)
//	{
//	case OPER_TRAIN:
//		// Train the HMM model
//		TrainHMM( fnHMM, nLineOption, bFilter );
//		break;
//	case OPER_DETECT:
//		//Detect lines
//		DetectLine( fnHMM, bFilter );
//		break;
//	case OPER_FORM_ID:
//		//Form identification
//		FormIdentify( fnFormIdConfig, bFilter );
//		break;
//	}
//	return 0;	
//}


