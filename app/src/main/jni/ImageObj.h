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
// File NAME:		ImageObj.h
// File Function:	Read and write image file
//
//				Developed by: Yefeng Zheng
//				First Created: June 2001
//			University of Maryland, College Park
/////////////////////////////////////////////////////////////////////////

#ifndef __IMAGEOBJ_H__
#define __IMAGEOBJ_H__
#include "convertMFC.h"

//Image type
enum { DIB_1BIT, DIB_4BIT, DIB_8BIT, DIB_16BIT, DIB_24BIT, DIB_32BIT, DIB_UNKNOWN };
//Palette type
enum{ WHITEISZERO, BLACKISZERO, RGB, PALETTE };

class CImage //: public CObject
{

//DECLARE_SERIAL(CImage)
public:
	CImage();
	~CImage();
	int FreeMem();	//Free memory

protected:
//    int	m_bImported;//If the image data ara allocate by other class.
						//If yes, do not free the image memory 
//	HGLOBAL m_hImg;		//Image handle
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
	int	m_bUpSideDown;//TRUE -- Upside down DIB format
	DSCC_BYTE	*m_pImg;		//Pointer to image data
	int		m_nWidth;		//Image width
	int		m_nHeight;		//Image height
	int		m_nWidthByte;	//Image width in bytes
	int		m_nBitCount;	//Bit count per pixel

	unsigned char lut[256];	//Test how many black pixels in one byte
public:
//////////////////////////////////////////////////////////////////////////////////////////////////
//	Initailize image
//////////////////////////////////////////////////////////////////////////////////////////////////
    //Initialize to void
	int Initialize() ;
// 	//Initialize from other CImage instance
//     int Initialize ( CImage &Image) ;
// 	//Initialize, do not allocate new memory, so do not release memory when destructing
//     int Initialize ( HGLOBAL hDIBImg ) ;
//     //Initialize from Bitmap
// 	int Initialize ( HDC hdc, CBitmap& bmp ) ;
// 	//Initialize, only allocate memory, do not copy image
//     int Initialize ( int nWidth, int nHeight, int nBitCount, int nPlanes ) ;
// 	//Initialize from image data
//     int Initialize ( BYTE *lpRawImg, int nWidth, int nHeight, int nImgType, int bUpsideDown = FALSE ) ;
// 	//Initialize from image line pointers
// 	int Initialize (unsigned char ** lpLine, int nWidth, int nHeight, int nImgType);
// 	//Initialize from image line pointers, upside down
// 	int InitializeUpSideDown ( BYTE *lpRawImg, int nWidth, int nHeight, int nImgType );
// 	//Initialize from image line pointers
// 	int Initialize ( BYTE *lpRawImg, int nWidth, int nHeight, int nWidthByte, int nBitCount, int bUpSideDown );
	//Initialize from image data info
	int Initialize (int nWidth, int nHeight, int nWidthByte, int nBitCount, int bUpSideDown);
// 	//Initialize from a specific image region
// 	int Initialize ( HGLOBAL hDIB, CRect rcRegion, int nImgType );
// 	//Initialize from a specific image region
// 	int Initialize ( LPBYTE *lpLine, int nWidth, int nHeight, CRect rcRegion, int nImgType );
 	//Initialize array lut[0-255], which calculate the number of black pixels in a byte
 	void InitBlackPixelsInByte();
	//Clear image
//	int	 Clear( );
/////////////////////////////////////////////////////////////////////////////////////////
//	Get image information
/////////////////////////////////////////////////////////////////////////////////////////
public:
	//Return lut
	DSCC_BYTE *GetBlackPixelsInByteMask();
	//Get m_bUpsideDown
	int GetOrientation();
	//Get image type
    int GetType() ;
	//Get image width
    int GetWidth() ;
	//Get image height
	int GetHeight() ;
	//Get the number of bytes to represent an image line
    int GetLineSizeInBytes() ;
	//Return image data handle in the memory
//	HGLOBAL GethImg() ;
	//Get black pixel ratio
    double GetBlackPixelRatio() ;
	//Retrieval the palette to see if the image is color
/*	int IsColor();*/
// protected:
// 	//Get the number of bytes to represent an image line
// 	int GetLineSizeInBytes(LPBITMAPINFO lpbi);
// 	//Get the palette size used
//     int GetPaletteSize ( LPBITMAPINFO lpbi ) ;

///////////////////////////////////////////////////////////////////////////////////
//	Read and write image
//////////////////////////////////////////////////////////////////////////////////
public:
	//Lock image in the memory, return pointer to the image data
    DSCC_BYTE *LockRawImg() ;
	//Release the lock
    int UnlockRawImg() ;
// 	//Read image from file
// 	int Read ( char* csFileName, int bInvert=TRUE ) ;
// 	//Write image to file
//     int Write ( CString csFileName ) ;
// protected:
// 	//Return raw image data pointer from BITMAPINFO structure
// 	BYTE *GetRawImgPtr ( LPBITMAPINFO lpbi ) ;
// 	//Get file extension to judge image type
// 	CString GetFileExtension ( CString FileName );

/////////////////////////////////////////////////////////////////////////////////
//	Others
////////////////////////////////////////////////////////////////////////////////
// public:
// 	//Test if need to invert image bits
// 	int NeedInvertBits();
// 	//Invert image bits
//     int InvertBits () ;
// 	//Test if need to invert image palette
// 	int NeedInvertPallete() ;
// 	//Invert image palette
//     int	InvertPallete() ;
	//Set image palette
//     int SetPalette(LPBITMAPINFO lpbi) ;
// 	//Set image palette
//     int SetPalette(RGBQUAD *pColors, int start, int end ) ;
// 	//Show full image to screen
//     int ShowFullImgDIB ( HDC hdc, int xd, int yd ) ;
// 	//Show full image to screen, stretch the image to fill specific region
// 	int ShowStretchImgDIB ( HDC hdc, int xd, int yd, int width, int height);
};

#endif  // __IMAGEOBJ_H__
