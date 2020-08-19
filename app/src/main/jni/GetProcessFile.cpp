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
// File NAME:		GetProcessFile.cpp
// File Function:	Get processing files in file list or under the specified directory.
//
//				Developed by: Yefeng Zheng
//			   First created: June, 2002
//			University of Maryland, College Park
///////////////////////////////////////////////////////////////////////////////////////////////////


#include "tools.h"


int	bFileList=FALSE;	//Process all files in the file list
FILE	*fpFileList;		//File pointer to file list
char    fnFile[MAX_PATH];	//File name, input from console

//WIN32_FIND_DATAA  FindFileData;	//For searching files
//HANDLE		hd = NULL;			//For searching files

int	bBatch;				//Batch mode

/*****************************************************************************
/*	Name:		GetFirstFile
/*	Function:	Get first file, support three modes
/*				- single file
/*				- *.file
/*				- file list
/*	Parameter:	fn -- First feature file
/*	Return:		0 -- Succeed
/*****************************************************************************/
// int GetFirstFile( char *fn )
// {
// 	if( bFileList ) //Process files listed in the filelist
// 	{
// 		bBatch = TRUE;
// 		fpFileList = fopen( fnFile, "rt" );
// 		if( fpFileList == NULL )
// 		{
// 			printf( "Can not read the file list: %s\n", fnFile );
// 			return -1;
// 		}
// 		char tmp[MAX_PATH];
// 		while( !feof( fpFileList ) )
// 		{
// 			fgets( tmp, 256, fpFileList );
// 			if( tmp[0] != '#' )
// 				break;
// 		}
// 		if( tmp[0] == '#' )	return -1;
// 		if( tmp[strlen(tmp)-1] == '\n' )
// 			tmp[strlen(tmp)-1] = '\0';
// 		sprintf( fn, "%s", tmp );
// 	}
// 	else if( strchr( fnFile, '*' ) != NULL )//Process all files in the folder
// 	{
// 		bBatch = TRUE;
// 		char csSearchName[MAX_PATH];
// 		sprintf( csSearchName, fnFile );
// 		hd = FindFirstFileA(csSearchName, &FindFileData);
// 		if( hd == INVALID_HANDLE_VALUE )	
// 			return -1;
// 		sprintf( fn, "%s\\%s", (LPCSTR)GetPathName( fnFile ), FindFileData.cFileName );
// 	}
// 	else//Process single file
// 	{
// 		bBatch = FALSE;
// 		sprintf( fn, "%s", fnFile );
// 	}
// 	return 0;
// }


/*****************************************************************************
/*	Name:		GetNextFile
/*	Function:	Get first file, support three modes
/*				- single file
/*				- *.file
/*				- file list
/*	Parameter:	fn -- Next feature file
/*	Return:		0 -- Succeed
/*****************************************************************************/
// int GetNextFile( char *fn )
// {
// 	if( bFileList )
// 	{
// 		char tmp[MAX_PATH];
// 		strcpy( tmp, "" );
// 		while( !feof( fpFileList ) )
// 		{
// 			if( fgets( tmp, 256, fpFileList ) == NULL )
// 				break;
// 			if( tmp[0] != '#' )
// 				break;
// 		}
// 		if( strcmp(tmp, "" ) == 0 )	
// 		{
// 			fclose( fpFileList );
// 			fpFileList = NULL;
// 			return -1;
// 		}
// 		if( tmp[strlen(tmp)-1] == '\n' )
// 			tmp[strlen(tmp)-1] = '\0';
// 		sprintf( fn, "%s", tmp );	
// 	}
// 	else
// 	{
// 		if( bBatch )
// 		{
// 			if( FindNextFileA(hd, &FindFileData) != TRUE )
// 			{
// 				FindClose( hd );
// 				return -1;
// 			}
// 			sprintf( fn, "%s\\%s", (LPCSTR)GetPathName( fnFile ), FindFileData.cFileName );
// 		}
// 		else
// 			return -1;
// 	}
// 	return 0;
// }
