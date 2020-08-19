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

/////////////////////////////////////////////////////////////////
// File Name:		ImageTool.h
// File Function:	Interface for CImageTool class
//
//				Developed by: Yefeng Zheng
//				First Created: June 2001
//			University of Maryland, College Park
//////////////////////////////////////////////////////////////////////
#ifndef __IMAGE_TOOL_H__
#define	__IMAGE_TOOL_H__

#include "convertMFC.h"
#include "ImageObj.h"

class CImageTool //: public CObject  
{
public:
	CImageTool();
	virtual ~CImageTool();
//////////////////////////////////////////////////////////////////////////////////
//	Methods for copying Image
//////////////////////////////////////////////////////////////////////////////////
	//Copy an image block 
	static int CopyBlock ( DSCC_BYTE *ps, int xs, int ys, int wInBytesS, DSCC_BYTE *pd, int xd, int yd, int wInBytesD, int w, int h, int Type, int bUpSideDown );
	//Copy image with more parameters for adjusting
	static int CopyImage ( CImage &ImgS, int xs, int ys, CImage &ImgD, int xd, int yd, int w, int h);
	//Copy image
//	static int CopyImage ( CImage &ImgS, int xs, int ys, int w, int h, CImage &ImgD);
	// Copy binary block with byte allign to speed up copy image speed
//	static int CopyBinaryImageByteAllign ( CImage &ImgS, int xs, int ys, int w, int h, CImage &ImgD );
	
//////////////////////////////////////////////////////////////////////////////////
//	Methods for cutting Image
//////////////////////////////////////////////////////////////////////////////////
	//Brush the top part of a block to white
	static int CutImageTop ( DSCC_BYTE *pImg, int wb, int h, DSCC_POINT Left, DSCC_POINT Right, int Type );
	//Brush the bottom part of a block to white
	static int CutImageBottom ( DSCC_BYTE *pImg, int wb, int h, DSCC_POINT Left, DSCC_POINT Right, int Type );
	//Brush the left part of a block to white
	static int CutImageLeft ( DSCC_BYTE *pImg, int wb, int h, DSCC_POINT Top, DSCC_POINT Bottom, int Type );
	//Brush the right part of a block to white
	static int CutImageRight ( DSCC_BYTE *pImg, int wb, int h, DSCC_POINT Top, DSCC_POINT Bottom, int Type );
	//Cut a block image out of the whole image
//	static int CutImage ( CImage &ImgS, POINT TopL, POINT TopR, POINT BtmL, POINT BtmR, CImage &ImgD );

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Methods for eraseing pixels
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Set a vertical line to the indicated gray level
	static int SetVerLineToGrayscale ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int h, DSCC_BYTE Gray, int bUpSideDown );
	//Set a horizontal line to the indicated gray level
	static int SetHorLineToGrayscale ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int w, DSCC_BYTE Gray, int bUpSideDown );
	//Set a vertical line to the indicated color
	static int SetVerLineToColor ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int h, DSCC_COLORREF Color, int bUpSideDown );
	//Set a horizontal line to the indicated color
	static int SetHorLineToColor ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int w, DSCC_COLORREF Color, int bUpSideDown );
	//Set a vertical line to black
	static int SetVerLineToBlack ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int h, int bUpSideDown );
	//Set a vertical line to white
	static int SetVerLineToWhite ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int h, int bUpSideDown );
	//Set a horizontal line to black
	static int SetHorLineToBlack ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int w, int bUpSideDown );
	//Set a horizontal line to white. Her p point to the whole image
	static int SetHorLineToWhite ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int w, int bUpSideDown );
	//Set a horizontal line to white. Here p point to the image line
	static int SetHorLineToWhite ( DSCC_BYTE *p, int x, int y, int w );
	//Set a line to specific value for color/gray/binary images
	static int SetLine ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int Len, int bIsHorLine, int ImageType, DSCC_BYTE Gray, int bUpSideDown );
	//Draw a line on an image, used for debug
	static int DrawLine ( CImage &Img, DSCC_POINT s, DSCC_POINT e );
	//Draw a rectangle on an image, used for debug
	static int DrawRect ( CImage &Img, DSCC_RECT rc );
	//Earse black strip of a binary image
//	static int EraseBlackStrip( CImage &Img );
	//Earse black strip of a binary image, and save the processed result
//	static int EraseBlackStrip ( char* sFnImage );
	//Dilate image used for line detection on edge images	//Yefeng Zheng 02/23/2003
//	static int Dialate ( CImage &Img, CImage &DImg );

/////////////////////////////////////////////////////////////////////////////////////
//	Methods for testing white/black 
/////////////////////////////////////////////////////////////////////////////////////	
	//Test if the pixels on a horizontal line are all white
	static int AllVerWhite ( DSCC_BYTE *p, int wb, int h, DSCC_POINT s, DSCC_POINT e);
	//Test if the pixels on a vertial line are all white
	static int AllHorWhite ( DSCC_BYTE *p, int wb, int h, int x, int y, int w );
	//Test if the pixels on a horizontal/vertical line are all white
	static int AllWhite ( DSCC_BYTE *p, int wb, int h, DSCC_POINT s, DSCC_POINT e );
	//Test if the pixels on a line are all white in a gray level image
	static int GrayAllWhite ( DSCC_BYTE *p, int wb, int h, DSCC_POINT s, DSCC_POINT e, DSCC_BYTE Gray );
	//Test if the pixels on a line are all white in a color image
	static int ColorAllWhite ( DSCC_BYTE *p, int wb, int h, DSCC_POINT s, DSCC_POINT e, int Channel, DSCC_BYTE Gray );

	
/////////////////////////////////////////////////////////////////////////////////////////////////////
//	Skew correction
/////////////////////////////////////////////////////////////////////////////////////////////////////
	//Copy pixels, written in assemble language to accelate, called by BlockCopyBi
//	static int BlockCopyBi0 ( DSCC_BYTE *ImgS, int xs, int ys, int wInBytesS, DSCC_BYTE *ImgD, int xd, int yd, int wInBytesD, int w, int h );
	//Copy pixels, written in assemble language to accelate, used for skew correction of binary images
//	static int BlockCopyBi ( DSCC_BYTE *ImgS, int xs, int ys, int wInBytesS, DSCC_BYTE *ImgD, int xd, int yd, int wInBytesD, int w, int h );
	//Skew correction for binary image
//	static CImage * BinaryImageSkewCorrection ( CImage* _SrcImg, double Angle );
	//Skew correction for binary image, another interface
//	static int	BinaryImageSkewCorrection ( CImage &_SrcImg, CImage &_DstImg, double Angle );
	//Skew correction then cut extra border
//	static int	SkewCorrectionCut( CImage &Img, double dSkewAngle, CImage &CorrectedImg );
	//Skew correction then cut extra border
//	static int	SkewCorrectionCut( CImage &Img, double dSkewAngle );
	//Skew correction for color image 
//	static CImage *ColorImageSkewCorrection ( CImage* _SrcImg, double Angle );
	//Rotate binary image, called by Zoom()
//	static int	Rotate ( DSCC_BYTE *ImgS, int SrcXs, int SrcXe, int SrcWdInBytes, int h, DSCC_BYTE *ImgD, int DstXs, int DstWdInBytes );
	//Upside down
	static int UpsideDown( DSCC_BYTE *ImgS, int w, int h, DSCC_BYTE *&ImgD );
/////////////////////////////////////////////////////////////////////////////////////////////////////
//	Other tools
/////////////////////////////////////////////////////////////////////////////////////////////////////
	//Shift image
	static int	ShiftImage(DSCC_BYTE* pByteS, int w, int h, int wb, DSCC_BYTE* pByte, int RowShift, int ColShift);
	//Get the bounding box of a character image, processing all image types
	//Extend char or cell boundary box to include all black pixels as possible
	static int	GetCharBound(CImage &Image, DSCC_POINT &TopL, DSCC_POINT &TopR, DSCC_POINT &BtmL, DSCC_POINT &BtmR);
	//Get the bounding box of a image, only process binary image
	static int	GetBound(CImage &Image, DSCC_RECT &rRange);
	//Zoom image in Y direction
	static void	ZoomY ( DSCC_BYTE *ImgS,  DSCC_BYTE *ImgD, int h, int nh, int wInBytes );
	//Zoom image
//	static int	Zoom ( DSCC_BYTE *ImgS, DSCC_BYTE *ImgD, int wInBytes, int h, int nwInBytes, int nh );
	//Get black pixel ratio in an image block
//	static double GetBlackPixelRatio ( LPBYTE FAR *lpLine,  RECT rcBound );
	static double GetBlackPixelRatio ( CImage &Img,  DSCC_RECT rcBound );
	//Get the number of black pixels in an image block
	static int	GetBlackPixelNum ( DSCC_BYTE *p, int wb, int hh, int left, int top, int w, int h ) ;
	//Calculate the number of black pixels on a line
	static int	PixelsOnLine(CImage&  Image, int xb, int yb, int xe, int ye);
	//Do horizontal projection
	static int	DoHorProject(CImage &Img, double *&Proj, int &nProj, DSCC_CRect rc = DSCC_CRect(0, 0, 0, 0) );
	//Do skewed horizontal projection
	static int DoSkewHorProject( CImage &Img, double nSkewAngle, double *&Proj, int &nProj  );
	//Do vertical projection
	static int	DoVerProject(CImage &Img, double *&Proj, int &nProj, DSCC_CRect rc = DSCC_CRect(0, 0, 0, 0) );
	//Count black pixels on a line
	static int PixelsOnHorLine(CImage&  Image, int xb, int yb, int xe, int ye);
	//Smooth projection
	static int	SmoothProject(double *Proj, int nProj);
	//Dump projection to an image file for debugging
// 	static int	DumpProject(char* snFileName, double *prj, int nProj, int bHor=TRUE, int nMaxProj=0);
 
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //	Extract connected component
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 	//Extract connected component
 	static int	ExtractConnComp( DSCC_BYTE *pImg, int w, int h, DSCC_RECT *&pConnComp, int *&nConnPixels, int &nConnComp, int bBlackConn, int Connectness, int bByte );
 	//Extract row runlength, 1 pixels per bit
 	static int	GetRowRunLengthBit( DSCC_BYTE *Line, int w, int h, int wb, int row, DSCC_POINT *RunLength, int &nRunLength, int bBlackConn );
 	//Extract row runlength, 1 pixels per byte
 	static int	GetRowRunLength( DSCC_BYTE *Line, int w, int h, int row, DSCC_POINT *RunLength, int &nRunLength, int bBlackConn );
 
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //Bit Byte conversion
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 	static int	BitToByte( DSCC_BYTE* BitImg, int wb, int w, int h, DSCC_BYTE *ByteImg, int w2, int h2, DSCC_CRect &BoundBox );
 	static int	BitToByte( DSCC_BYTE* BitImg, int wb, int w, int h, DSCC_BYTE *&ByteImg);
 	static int	ByteToBit( DSCC_BYTE* pByteImg, int w, int h, DSCC_BYTE *RawByte, int wb=0, DSCC_BYTE BlackPixel=0);
};

#endif // __IMAGE_TOOL_H__
