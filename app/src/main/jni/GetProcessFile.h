//////////////////////////////////////////////////////////////////////////
// File Name:		GetProcessFile.h
// File Function:	Prototypes of functions related to searching all files
//					under a folder.
//
//				Developed by: Yefeng Zheng
//				First Created: June 2002
//			University of Maryland, College Park
//					All Right Reserved
///////////////////////////////////////////////////////////////////////////

#ifndef __GET_PROCESS_FILE__
#define	__GET_PROCESS_FILE__
#include "convertMFC.h"
//Variables to get processing file list
extern	int	bFileList;			//Process all files in the file list
extern	FILE	*fpFileList;		//File pointer to file list
extern	char    fnFile[MAX_PATH];	//File name, input from console
//extern	WIN32_FIND_DATA  FindFileData;	//For searching files
//extern	HANDLE	hd;					//For searching files
extern	int	bBatch;				//Batch mode

//int GetFirstFile( char *fn );
//int GetNextFile( char *fn );

#endif //__GET_PROCESS_FILE__