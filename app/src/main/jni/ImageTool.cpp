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

///////////////////////////////////////////////////////////////////////
// File Name:		ImageTool.cpp
// File Function:	Some image operation tools including copy, erase, 
//					skew correction, rotate, zoom, shift and test if the
//					pixels on a line are all white
//
//				Developed by: Yefeng Zheng
//			University of Maryland, College Park
//////////////////////////////////////////////////////////////////////

#include "tools.h"
#include "math.h"
#include "ImageObj.h"
#include "ImageTool.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern DSCC_BYTE lut[], lz[], rz[];
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageTool::CImageTool()
{

}

CImageTool::~CImageTool()
{

}
//////////////////////////////////////////////////////////////////////////////////
//	Methods for copying Image
//////////////////////////////////////////////////////////////////////////////////

/***********************************************************************************************
/*	Name:		CopyBlock
/*	Function:	Copy a block of image
/*
/************************************************************************************************/
int CImageTool::CopyBlock ( DSCC_BYTE *ps, int xs, int ys, int wInBytesS, DSCC_BYTE *pd, int xd, int yd, int wInBytesD, int w, int h, int Type, int bUpSideDown)
{
	DSCC_BYTE *tps, *tpd;
	if(Type == DIB_1BIT)
	{
		DSCC_BYTE mask[8] = {1,2,4,8,16,32,64,128} ;
		for ( int i=0 ; i<h ; i++ )
		{
			tps=ps+(ys+i)*wInBytesS+(xs>>3) ;
			if( bUpSideDown )
				tpd=pd+(yd+i)*wInBytesD+(xd>>3) ;
			else
				tpd=pd+(h-1-yd-i)*wInBytesD+(xd>>3);
			int rs = 7-(xs%8) ;
			int rd = 7-(xd%8) ;
			for ( int j=0 ; j<w ; j++ )
			{
				if ( mask[rs] & (*tps) )
					*tpd = (*tpd) | mask[rd] ;
				if ( --rs < 0 ) { rs = 7 ; tps++ ; }
				if ( --rd < 0 ) { rd = 7 ; tpd++ ; }
			}
		}
		return 0;
	}
	else if(Type == DIB_8BIT)
	{
		for ( int i=0; i<h; i++)
		{
			tps = ps+(ys+i)*wInBytesS+xs;
			if( bUpSideDown )
				tpd = pd+(h-1-yd-i)*wInBytesD+xd;
			else
				tpd = pd+(yd+i)*wInBytesD+xd;
			for ( int j=0; j<w; j++)
			{
				*tpd = *tps;	
				tpd++;		tps++;
			}
		}
		return 0;
	}
	else if(Type == DIB_24BIT)
	{
		for ( int i=0; i<h; i++)
		{
			tps = ps+(ys+i)*wInBytesS+3*xs;
			if( bUpSideDown )
				tpd = pd+(h-1-yd-i)*wInBytesD+3*xd;
			else
				tpd = pd+(yd+i)*wInBytesD+3*xd;
			for ( int j=0; j<3*w; j++)
			{
				*tpd = *tps;
				tpd++;
				tps++;
			}
		}
	}
    return -1 ;
}

/***********************************************************************************************
/*	Name:		CopyImage
/*	Function:	Copy image with more parameters to adjust
/*
/************************************************************************************************/
int CImageTool::CopyImage ( CImage &ImgS, int xs, int ys, CImage &ImgD, int xd, int yd, int w, int h)
{
    int wInBytesS = ImgS.GetLineSizeInBytes() ;
    int hs = ImgS.GetHeight() ;
    DSCC_BYTE *ps = ImgS.LockRawImg() ;
	int bUpSideDown = ImgS.GetOrientation();

    int wInBytesD = ImgD.GetLineSizeInBytes() ;
    int hd = ImgD.GetHeight() ;
    DSCC_BYTE *pd = ImgD.LockRawImg() ;
	if( bUpSideDown )
		CopyBlock ( ps, xs, hs-ys-h, wInBytesS, pd, xd, hd-yd-h, wInBytesD, w, h, ImgS.GetType(), bUpSideDown) ;
	else
		CopyBlock ( ps, xs, ys, wInBytesS, pd, xd, hd-yd-h, wInBytesD, w, h, ImgS.GetType(), bUpSideDown) ;
    ImgD.UnlockRawImg() ;
    ImgS.UnlockRawImg() ;
    return 0 ;
}

/***********************************************************************************************
/*	Name:		CopyImage
/*	Function:	Copy image
/*
/************************************************************************************************/
// int CImageTool::CopyImage ( CImage &ImgS, int xs, int ys, int w, int h, CImage &ImgD)
// {
//     if(ImgS.GetType() == DIB_1BIT)
// 	{
// 		if ( !ImgD.Initialize(w, h, 1, 1) )
// 			return -1 ;
// 	}
// 	else if(ImgS.GetType() == DIB_8BIT)
// 	{
// 		if ( !ImgD.Initialize(w, h, 8, 1) )
// 			return -1;
// 	}
// 	else if(ImgS.GetType() == DIB_24BIT)
// 	{
// 		if ( !ImgD.Initialize(w, h, 24, 1) )
// 			return -1;
// 	}
//     return CopyImage ( ImgS, xs, ys, ImgD, 0, 0, w, h) ;
// }

/**********************************************************************
/*	Name:		CopyBinaryImageByteAllign
/*	Function:	Copy binary image with byte allign to speed up 
/*				copy image speed
/*
/*********************************************************************/
// int CImageTool::CopyBinaryImageByteAllign ( CImage &ImgS, int xs, int ys, int w, int h, CImage &ImgD )
// {//Copy binary image align BYTE to speed up copying processing
//     int xe = xs+w;
// 	xs = (xs+4)/8*8;
// 	xe = (xe+4)/8*8;
// 	w = xe-xs;
// 	if ( !ImgD.Initialize(w, h, 1, 1) )
//         return -1 ;
//     int wInBytesS = ImgS.GetLineSizeInBytes() ;
//     int hs = ImgS.GetHeight() ;
//     BYTE *ps = ImgS.LockRawImg() ;
//     int wInBytesD = ImgD.GetLineSizeInBytes() ;
//     int hd = ImgD.GetHeight() ;
//     BYTE *pd = ImgD.LockRawImg() ;
// 	if(xs%8 != 0 || w%8 != 0)	return -1;
// 	int nStartByte = xs/8;
// 	int nWidthByte = w/8;
// 	for(int i=0; i<h; i++)
// 		memcpy(pd+i*wInBytesD, ps+(ys+i)*wInBytesS+nStartByte, nWidthByte);
// 	return 0;
// 
//     ImgD.UnlockRawImg() ;
//     ImgS.UnlockRawImg() ;
//     return 0 ;
// }

//////////////////////////////////////////////////////////////////////////////////
//	Methods for cutting Image
//////////////////////////////////////////////////////////////////////////////////

/**********************************************************************
/*	Name:		CutImageTop
/*	Function:	Brush the top part of a image block to white
/*
/*********************************************************************/
int CImageTool::CutImageTop ( DSCC_BYTE *pImg, int wb, int h, DSCC_POINT Left, DSCC_POINT Right, int Type )
{//Called by CutImage() and CutBinaryImage(). Do not use it indenpendently
	if(Right.x == Left.x)	return -1;
	for ( int i=Left.x ; i<=Right.x ; i++ )
	{
		int y = (Right.y-Left.y)*(i-Left.x)/(Right.x-Left.x) + Left.y ;
		if ( y <= 0 )	continue;
		if ( Type == DIB_1BIT )
			SetVerLineToWhite ( pImg, wb, h, i, 0, y, TRUE) ;
		else if( Type == DIB_8BIT )
			SetVerLineToGrayscale(pImg, wb, h, i, 0, y, 255, TRUE);
		else if( Type == DIB_24BIT )
			SetVerLineToColor ( pImg, wb, h, i, 0, y, DSCC_RGB(255, 255, 255), TRUE);
	}
    return 0 ;
}

/**********************************************************************
/*	Name:		CutImageBottom
/*	Function:	Brush the bottom part of a image block to white
/*
/*********************************************************************/
int CImageTool::CutImageBottom ( DSCC_BYTE *pImg, int wb, int h, DSCC_POINT Left, DSCC_POINT Right, int Type )
{//Called by CutImage() and CutBinaryImage(). Do not use it indenpendently
	if(Left.x == Right.x)	return -1;
	for ( int i=Left.x ; i<=Right.x ; i++ )
    {
        int y = (Right.y-Left.y)*(i-Left.x)/(Right.x-Left.x) + Left.y ;
        if ( y >= h-1 )	continue;
        if(Type == DIB_1BIT)
			SetVerLineToWhite ( pImg, wb, h, i, y+1, h-1-(1+y), TRUE) ;
		else if(Type == DIB_8BIT)	  
			SetVerLineToGrayscale(pImg, wb, h, i, y+1, h-1-(1+y), 255, TRUE);
		else if(Type == DIB_24BIT)
			SetVerLineToColor(pImg, wb, h, i, y+1, h-1-(1+y), DSCC_RGB(255, 255, 255), TRUE );
	}
    return 0 ;
}

/**********************************************************************
/*	Name:		CutImageLeft
/*	Function:	Brush the left part of a image block to white
/*
/*********************************************************************/
int CImageTool::CutImageLeft ( DSCC_BYTE *pImg, int wb, int h, DSCC_POINT Top, DSCC_POINT Bottom, int Type )
{//Called by CutImage() and CutBinaryImage(). Do not use it indenpendently
	if(Bottom.y == Top.y)	return -1;
	for ( int i=Top.y ; i<=Bottom.y ; i++ )
    {
        int x = (Bottom.x-Top.x)*(i-Top.y)/(Bottom.y-Top.y) + Top.x ;
        if ( x < 0 )	continue;
		if ( Type == DIB_1BIT )
            SetHorLineToWhite ( pImg, wb, h, 0, i, x, TRUE ) ;
		else if(Type == DIB_8BIT )
			SetHorLineToGrayscale(pImg, wb, h, 0, i, x, 255, TRUE);
		else if(Type == DIB_24BIT )
			SetHorLineToColor(pImg, wb, h, 0, i, x, DSCC_RGB(255, 255, 255), TRUE);
    }
    return 0 ;
}

/**********************************************************************
/*	Name:		CutImageRight
/*	Function:	Brush the right part of a image block to white
/*
/*********************************************************************/
int CImageTool::CutImageRight ( DSCC_BYTE *pImg, int wb, int h, DSCC_POINT Top, DSCC_POINT Bottom, int Type )
{//Called by CutImage() and CutBinaryImage(). Do not use it indenpendently
	if(Bottom.y == Top.y)	return -1;
	for ( int i=Top.y ; i<=Bottom.y ; i++ )
    {
        int x = (Bottom.x-Top.x)*(i-Top.y)/(Bottom.y-Top.y) + Top.x ;
		if( Type == DIB_1BIT && x < wb*8-1)
            SetHorLineToWhite ( pImg, wb, h, x+1, i, wb*8-1-(1+x), TRUE ) ;
		else if( Type == DIB_8BIT && x < wb-1)
			SetHorLineToGrayscale(pImg, wb, h, x+1, i, wb-1-(1+x), 255, TRUE);
		else if( Type == DIB_24BIT && x < wb/3-1)
			SetHorLineToColor(pImg, wb, h, x+1, i, wb/3-1-(1+x), DSCC_RGB(255, 255, 255), TRUE );
    }
    return 0 ;
}

/**********************************************************************
/*	Name:		CutImage
/*	Function:	Cut a image block out of the whole image
/*
/*********************************************************************/
// int CImageTool::CutImage ( CImage &ImgS, POINT TopL, POINT TopR, POINT BtmL, POINT BtmR, CImage &ImgD )
// {
//     int left = max ( min(TopL.x, BtmL.x), 0 ) ;
//     int right = min ( max(TopR.x, BtmR.x), ImgS.GetWidth()-1 ) ;
//     int top = max ( min(TopL.y, TopR.y), 0 ) ;
//     int bottom = min ( max(BtmL.y, BtmR.y), ImgS.GetHeight()-1 ) ;
// 
// 	if( right < left || bottom < top )		
// 		return -1;
// 	if( left < 0 || top < 0 || right > ImgS.GetWidth()-1 || bottom > ImgS.GetHeight()-1 )
// 		return -1;
// 	if( CopyImage ( ImgS, left, top, right-left+1, bottom-top+1, ImgD ) != 0)
// 		return -1;
// 
//     if( TopL.x == BtmL.x && TopL.y == TopR.y && TopR.x == BtmR.x && BtmL.y == BtmR.y )
// 		return 0;
// 
//     BYTE *pImg = ImgD.LockRawImg() ;
//     int wb = ImgD.GetLineSizeInBytes() ;
//     int h = ImgD.GetHeight() ;
// 	if ( wb < 0)	return -1;
// 	CutImageTop ( pImg, wb, h, ShiftPoint(TopL, -left, -top),  ShiftPoint(TopR, -left, -top), ImgS.GetType() ) ;
//     CutImageBottom ( pImg, wb, h, ShiftPoint(BtmL, -left, -top),  ShiftPoint(BtmR, -left, -top), ImgS.GetType() ) ;
//     CutImageLeft ( pImg, wb, h, ShiftPoint(TopL, -left, -top),  ShiftPoint(BtmL, -left, -top), ImgS.GetType() ) ;
//     CutImageRight ( pImg, wb, h, ShiftPoint(TopR, -left, -top),  ShiftPoint(BtmR, -left, -top), ImgS.GetType() ) ;
// 	ImgD.UnlockRawImg();
//     return 0 ;
// }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Methods for eraseing pixels
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************
/*	Name:		SetVerLineToGrayscale
/*	Funcation:	Set all pixels on a vertical line to the indicated gray level
/*
/**************************************************************************/
int CImageTool::SetVerLineToGrayscale ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int h, DSCC_BYTE Gray, int bUpSideDown )
{//Called by SetLine(). Do not use it independently
	if( bUpSideDown )
		p += (Height-1-y)*WidthInBytes + x ;
	else
		p += y*WidthInBytes + x ;
    for ( int i=0 ; i<h ; i++ )
	{
        *p =  Gray ;
		if( bUpSideDown )
			p -= WidthInBytes;
		else
			p += WidthInBytes;
	}
    return 0 ;
}

/********************************************************************************
/*	Name:		SetHorLineToGrayscale
/*	Funcation:	Set all pixels on a horizontal line to the indicated gray level
/*
/********************************************************************************/
int CImageTool::SetHorLineToGrayscale ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int w, DSCC_BYTE Gray, int bUpSideDown )
{//Called by SetLine(). Do not use it independently
	if( bUpSideDown )
		p += (Height-1-y)*WidthInBytes + x ;
	else
		p += y*WidthInBytes + x ;
    for ( int i=0 ; i<w ; i++ )
        *p++ = Gray ;
    return 0 ;
}

/********************************************************************************
/*	Name:		SetVerLineToColor
/*	Funcation:	Set all pixels on a vertical line to the indicated color
/*
/********************************************************************************/
int CImageTool::SetVerLineToColor ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int h, DSCC_COLORREF Color, int bUpSideDown )
{//COLORREF = 0x00bbggrr
	if( bUpSideDown )
		p += (Height-1-y)*WidthInBytes + 3*x;
	else
		p += y*WidthInBytes + 3*x;

	for( int i=0; i<h; i++)
	{
		*p = (DSCC_BYTE)(0x000000ff & Color);
		*(p+1) = (DSCC_BYTE)( (0x0000ff00 & Color) >> 8);
		*(p+2) = (DSCC_BYTE)( (0x00ff0000 & Color) >> 16);			
		if( bUpSideDown ) 
			p -= WidthInBytes;
		else
			p += WidthInBytes;
	}
	return 0;
}

/********************************************************************************
/*	Name:		SetHorLineToColor
/*	Funcation:	Set all pixels on a horizontal line to the indicated color
/*
/********************************************************************************/
int CImageTool::SetHorLineToColor ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int w, DSCC_COLORREF Color, int bUpSideDown )
{
	if( bUpSideDown )
		p += (Height-1-y)*WidthInBytes + 3*x;
	else
		p += y*WidthInBytes + 3*x;
	for ( int i=0; i<w; i++)
	{
		*p = (DSCC_BYTE)(0x000000ff & Color);
		*(p+1) = (DSCC_BYTE)( (0x0000ff00 & Color) >> 8);
		*(p+2) = (DSCC_BYTE)( (0x00ff0000 & Color) >> 16);			
		p += 3;
	}
	return 0;
}

/********************************************************************************
/*	Name:		SetVerLineToBlack
/*	Funcation:	Set all pixels on a vertical line to black, for binary image
/*
/********************************************************************************/
int CImageTool::SetVerLineToBlack ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int h, int bUpSideDown )
{//Called by SetLine(). Do not use it independently
    DSCC_BYTE mask = 128>>(x%8) ;
	if( bUpSideDown )
		p += (Height-1-y)*WidthInBytes + x/8 ;
	else
		p += y*WidthInBytes + x/8 ;
    for ( int i=0 ; i<h ; i++, p-=WidthInBytes )
        *p |= mask ;
    return 0 ;
}

/********************************************************************************
/*	Name:		SetVerLineToWhite
/*	Funcation:	Set all pixels on a vertical line to white, for binary image
/*
/********************************************************************************/
int CImageTool::SetVerLineToWhite ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int h, int bUpSideDown )
{//Called by SetLine(). Do not use it independently
	if( x<0 || x>=WidthInBytes*8 || y<0 || y+h>Height )
	{
		cout<<"SetVerLineToWhite Error!"<<endl;
		return -1;
	}
    DSCC_BYTE mask = 128>>(x%8) ;
	if( bUpSideDown )
		p += (Height-1-y)*WidthInBytes + x/8 ;
	else
		p += y*WidthInBytes + x/8 ;
    for ( int i=0 ; i<h ; i++ )
	{
        *p &= ~mask ;
		if( bUpSideDown )
			p -= WidthInBytes;
		else
			p += WidthInBytes;
	}
    return 0 ;
}

/********************************************************************************
/*	Name:		SetHorLineToBlack
/*	Funcation:	Set all pixels on a horizontal line to black, for binary image
/*
/********************************************************************************/
int CImageTool::SetHorLineToBlack ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int w, int bUpSideDown )
{//Called by SetLine(). Do not use it independently
    int i ;
    DSCC_BYTE mask[8] = {128,64,32,16,8,4,2,1} ;
	if( bUpSideDown )
		p += (Height-1-y)*WidthInBytes + x/8 ;
	else
		p += y*WidthInBytes + x/8 ;

    if ( x%8 > 0 )
    {
        for ( i=x%8 ; i<8 && w>0 ; i++, w-- )
            *p |= mask[i] ;
        p++ ;
    }
    while ( w >= 8 )
    {
        *p++ = 255 ;
        w -= 8 ;
    }
    for ( i=0 ; i<w ; i++ )
        *p |= mask[i] ;

    return 0 ;
}

/********************************************************************************
/*	Name:		SetHorLineToWhite
/*	Funcation:	Set all pixels on a horizontal line to white, for binary image
/*
/********************************************************************************/
int CImageTool::SetHorLineToWhite ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int w, int bUpSideDown )
{
	if( x<0 || x+w>=WidthInBytes*8 || y<0 || y>=Height )
	{
		cout<<"SetHorLineToWhite Error!"<<endl;
		return -1;
	}
    int i ;
    DSCC_BYTE mask[8] = {128,64,32,16,8,4,2,1} ;
	if( bUpSideDown )
		p += (Height-1-y)*WidthInBytes + x/8 ;
	else
		p += y*WidthInBytes + x/8 ;
    if ( x%8 > 0 )
    {
        for ( i=x%8 ; i<8 && w>0 ; i++, w-- )
            *p &= ~mask[i] ;
        p++ ;
    }
    while ( w >= 8 )
    {
        *p++ = 0 ;
        w -= 8 ;
    }
    for ( i=0 ; i<w ; i++ )
        *p &= ~mask[i] ;

    return 0 ;
}

/********************************************************************************
/*	Name:		SetHorLineToWhite
/*	Funcation:	Set all pixels on a horizontal line to white, for binary image
/*
/********************************************************************************/
int CImageTool::SetHorLineToWhite ( DSCC_BYTE *p, int x, int y, int w )
{
    int i ;
    DSCC_BYTE mask[8] = {128,64,32,16,8,4,2,1} ;
    p +=  x/8 ;
    if ( x%8 > 0 )
    {
        for ( i=x%8 ; i<8 && w>0 ; i++, w-- )
            *p &= ~mask[i] ;
        p++ ;
    }
    while ( w >= 8 )
    {
        *p++ = 0 ;
        w -= 8 ;
    }
    for ( i=0 ; i<w ; i++ )
        *p &= ~mask[i] ;

    return 0 ;
}

/*****************************************************************************************
/*	Name:		SetLine
/*	Funcation:	Erase frame lines.  Call SetVerLineToWhite(), SetVerLineToGrayscale(), 
/*				SetVerLineToBlack(), SetVerLineToColor(), SetHorLineToWhite(), 
/*				SetHorLineToGrayscale(), SetHorLineToBlack(), SetHorLineToColor() 
/*				Capable to process binary/gray/color images
/*
/*****************************************************************************************/
int CImageTool::SetLine ( DSCC_BYTE *p, int WidthInBytes, int Height, int x, int y, int Len, int bIsHorLine, int ImageType, DSCC_BYTE Gray, int bUpSideDown )
{
	if ( bIsHorLine )
	{
		if( y+Len > Height || y<0 )	return -1;
        switch ( ImageType )
        {
        case DIB_1BIT:
			{
				if( x >= WidthInBytes*8 || x<0 )
					return -1;
				return SetVerLineToWhite ( p, WidthInBytes, Height, x, y, Len, bUpSideDown ) ;
			}
        case DIB_8BIT:
            {
				if( x>=WidthInBytes || x<0 )
					return -1;
				return SetVerLineToGrayscale ( p, WidthInBytes, Height, x, y, Len, Gray, bUpSideDown ) ;
			}
		case DIB_24BIT:
			{
				if( x>=WidthInBytes/3 || x<0 )
				return SetVerLineToColor ( p, WidthInBytes, Height, x, y, Len, DSCC_RGB(255, 255, 255 ), bUpSideDown ) ; 
			}
		}
	}
    else
	{
		if( x>= Height || x<0 )
				return -1;
		switch ( ImageType )
        {
        case DIB_1BIT:
			{
				if( y+Len >= WidthInBytes*8 || y<0 )
					return -1;
				return SetHorLineToWhite ( p, WidthInBytes, Height, y, x, Len, bUpSideDown ) ;
			}
        case DIB_8BIT:
            {
				if( y+Len >= WidthInBytes || y<0 )
					return -1;
				return SetHorLineToGrayscale ( p, WidthInBytes, Height, y, x, Len, Gray, bUpSideDown ) ;
			}
		case DIB_24BIT:
			{
				if( y+Len >= WidthInBytes/3 || y<0 )
					return -1;
				return SetHorLineToColor( p, WidthInBytes, Height, y, x, Len, DSCC_RGB(255, 255, 255), bUpSideDown ) ;
			}
        }
	}
    return -1 ;
}

/********************************************************************************
/*	Name:		DrawLine
/*	Funcation:	Draw a line on an image, used for debug
/*	Parameter:	Img -- Image
/*				s -- Start point
/*				e -- End point
/*	Return:		TRUE -- Succeed
/*				FALSE -- Failed
/********************************************************************************/
int CImageTool::DrawLine ( CImage &Img, DSCC_POINT s, DSCC_POINT e )
{
	int	dx = e.x-s.x;
	int	dy = e.y-s.y;
	DSCC_BYTE *p = Img.LockRawImg();
	int	wb = Img.GetLineSizeInBytes();
	int	w = Img.GetWidth();
	int h = Img.GetHeight();
	int bUpSideDown = Img.GetOrientation();

	int dx0, dx1, dy0, dy1;
	dy0 = 0;
	dx0 = 0;
	if ( abs(dx) > abs(dy) )
	{
		if( dy == 0 )
			SetHorLineToBlack( p, wb, h, s.x, s.y, dx+1, bUpSideDown );
		else
		{
			while( dy0 != dy )
			{
				dx1 = dx*dy0/dy;
				SetHorLineToBlack( p, wb, h, s.x+dx0, s.y+dy0, dx1-dx0+1, bUpSideDown );
				dy0 += (e.y>s.y) ? 1:-1 ;
				dx0 = dx1;
			}
		}
	}
	else
	{
		if( dx == 0 )
			SetVerLineToBlack( p, wb, h, s.x, s.y, dy+1, bUpSideDown );
		else
		{
			while( dx0 != dx )
			{
				dy1 = dx0*dy/dx;
				SetVerLineToBlack( p, wb, h, s.x+dx0, s.y+dy0, dy1-dy0+1, bUpSideDown );
				dx0 += (e.x>s.x) ? 1:-1;
				dy0 = dy1;
			}
		}
	}
	Img.UnlockRawImg();
	return TRUE ;
}

/********************************************************************************
/*	Name:		DrawRect
/*	Funcation:	Draw a rectangle on an image, used for debug
/*	Parameter:	Img -- Image
/*				rc -- Rectangle to draw
/*	Return:		TRUE -- Succeed
/*				FALSE -- Failed
/********************************************************************************/
int CImageTool::DrawRect ( CImage &Img, DSCC_RECT rc )
{
	DrawLine( Img, DSCC_CPoint( rc.left, rc.top ), DSCC_CPoint( rc.right, rc.top ) );
	DrawLine( Img, DSCC_CPoint( rc.left, rc.bottom ), DSCC_CPoint( rc.right, rc.bottom ) );
	DrawLine( Img, DSCC_CPoint( rc.left, rc.top ), DSCC_CPoint( rc.left, rc.bottom ) );
	DrawLine( Img, DSCC_CPoint( rc.right, rc.top ), DSCC_CPoint( rc.right, rc.bottom ) );
	return TRUE ;
}

/****************************************************************************************************
/*	Name:		EraseBlackStrip
/*	Funcation:	Erase black strip around the image created by unapprociate scanning parameters.
/*	Parameters:	Img	-- Image
/*	Return:		0 -- Succeed
/***************************************************************************************************/
// int CImageTool::EraseBlackStrip( CImage &Img )
// {
// 	//Smear the image to fill small holes;
// 	CImage	ImgD;
// 	Dialate (Img, ImgD );
// 
// 	int			w = Img.GetWidth();
// 	int			wb = Img.GetLineSizeInBytes();
// 	int			h = Img.GetHeight();
// 	BYTE*		pImg = Img.LockRawImg();
// 	BYTE*		pImgD = ImgD.LockRawImg();
// 
// 	//Allocate memory for four borders
// 	int	*LeftPnt = (int*)malloc(sizeof(int)*h);
// 	int	*RightPnt = (int*)malloc(sizeof(int)*h);
// 	int *TopPnt = (int*)malloc(sizeof(int)*w);
// 	int *BottomPnt = (int*)malloc(sizeof(int)*w);
// 	if( LeftPnt == NULL || RightPnt == NULL || TopPnt == NULL || BottomPnt == NULL )
// 	{
// 		if( LeftPnt != NULL )
// 			free( LeftPnt );
// 		if( RightPnt != NULL )
// 			free( RightPnt );
// 		if( TopPnt != NULL )
// 			free( TopPnt );
// 		if( BottomPnt != NULL )
// 			free( BottomPnt );
// 		Img.UnlockRawImg();
// 		return -1;
// 	}
// 	//Initialize four borders
// 	int	i, j, k;
// 	for( i=0; i<h; i++)
// 	{
// 		LeftPnt[i] = -1;
// 		RightPnt[i] = w;
// 	}
// 	for( i=0; i<w; i++)
// 	{
// 		TopPnt[i] = -1;
// 		BottomPnt[i] = h;
// 	}
// 	
// 	//Get the profile of the left and right borders
// 	for( i=0; i<h; i++)
// 	{
// 		BYTE	*pLine = pImgD+(h-1-i)*wb;
// 		while( *pLine == 255 && LeftPnt[i] < w-8 )
// 		{
// 			LeftPnt[i] += 8;
// 			pLine++;
// 		}
// 		if( LeftPnt[i] < w-8 )
// 		{	
// 			for( j=7; j>=0; j--)
// 			{
// 				if( ((*pLine) & (1<<j)) == 0 )	break;
// 				LeftPnt[i]++;
// 			}
// 		}
// 		LeftPnt[i] = min( LeftPnt[i], w-1 );
// 		
// 		pLine = pImgD+(h-1-i)*wb+(w-1)/8;
// 		for( j=(w-1)%8; j>=0; j--)
// 		{
// 			if( ((*pLine) & (128>>j)) == 0 )	break;
// 			RightPnt[i]--;
// 		}
// 		if( j>=0 )		continue;
// 		pLine --;
// 		while( *pLine == 255 && RightPnt[i] >= 0 )
// 		{
// 			RightPnt[i] -= 8; 
// 			pLine--;
// 		}
// 		if( RightPnt[i] > 0 )
// 		{
// 			for( j=0; j<8; j++)
// 			{
// 				if( ((*pLine) & (1<<j)) == 0 )	break;
// 				RightPnt[i]--;
// 			}
// 		}
// 		RightPnt[i] = max( 0, RightPnt[i] );
// 	}
// 
// 	//Get the profile of the top and bottom borders
// 	for( i=0; i<w; i++)
// 	{
// 		for( j=0; j<h; j++)
// 		{
// 			if( ((*(pImgD+(h-1-j)*wb+i/8)) & (128>>(i%8))) == 0 )	break;
// 			TopPnt[i]++;
// 		}
// 		for( j=h-1; j>=0; j--)
// 		{
// 			if( ((*(pImgD+(h-1-j)*wb+i/8)) & (128>>(i%8))) == 0 )	break;
// 			BottomPnt[i]--;
// 		}
// 	}
// 
// #define	T			40
// #define PEAKWIDTH	15
// //Erase left black strip
// 	int	ConnStPnt=0, ConnEdPnt=0;
// 	while( ConnEdPnt < h)
// 	{
// 		//Find the starting range of the left border
// 		while( LeftPnt[ConnStPnt] == -1 )
// 		{
// 			if( ConnStPnt < h-1 )
// 				ConnStPnt++;
// 			else
// 				break;
// 		}
// 		if( ConnStPnt >= h-1 )
// 			break;
// 
// 		//Find teh ending range of the left border
// 		ConnEdPnt=ConnStPnt+1;
// 		while( ConnEdPnt < h)
// 		{
// 			if( LeftPnt[ConnEdPnt] >= 0 )
// 				ConnEdPnt++;
// 			else
// 				break;
// 		}
// 		//The block is too small
// 		if( ConnStPnt>0 && ConnEdPnt < h && ConnEdPnt-ConnStPnt < 60 )
// 		{
// 			int	AverWidth = 0;
// 			for( i=ConnStPnt; i<ConnEdPnt; i++)
// 				AverWidth += LeftPnt[i];
// 			AverWidth /= (ConnEdPnt-ConnStPnt);
// 			if( AverWidth < 60 )
// 			{
// 				ConnStPnt = ConnEdPnt+1;
// 				continue;
// 			}
// 		}
// 		i=ConnStPnt;
// 		while( i<ConnEdPnt-1 )
// 		{
// 			if( LeftPnt[i+1]-LeftPnt[i] > T )	//The jump is too large
// 			{
// 				j = i+1;
// 				while( j<ConnEdPnt-1 )
// 				{
// 					if( LeftPnt[j] - LeftPnt[j+1] > T )
// 						break;
// 					j++;
// 				}
// 				if( j < ConnEdPnt-1 && j-i < PEAKWIDTH ) //Smoothing the profile
// 					for( k = i+1; k<=j; k++)
// 						LeftPnt[k] = min( LeftPnt[k], LeftPnt[i]+(LeftPnt[j]-LeftPnt[i])*(k-i)/(j-i)+5);
// 				i = j+1;
// 			}
// 			else
// 				i++;
// 		}
// 		for( i=ConnStPnt; i<ConnEdPnt; i++)
// 			SetHorLineToWhite( pImg, wb, h, 0, i, min(wb*8-1, LeftPnt[i]+1), TRUE );
// 		ConnStPnt = ConnEdPnt+1;
// 	}
// 
// //Erase right black strip
// 	ConnStPnt = 0;
// 	while( ConnStPnt < h)
// 	{
// 		while( RightPnt[ConnStPnt] == w  )
// 		{
// 			if( ConnStPnt < h-1 )
// 				ConnStPnt++;
// 			else
// 				break;
// 		}
// 		if( ConnStPnt >= h-1 )	break;
// 		ConnEdPnt=ConnStPnt+1;
// 		while( ConnEdPnt < h)
// 		{
// 			if( RightPnt[ConnEdPnt] < w )
// 				ConnEdPnt++;
// 			else
// 				break;
// 		}
// 		if( ConnStPnt>0 && ConnEdPnt < h && ConnEdPnt-ConnStPnt < 60 )
// 		{
// 			int	AverWidth = 0;
// 			for( i=ConnStPnt; i<ConnEdPnt; i++)
// 				AverWidth += (w-RightPnt[i]);
// 			AverWidth /= (ConnEdPnt-ConnStPnt);
// 			if( AverWidth < 60 )
// 			{
// 				ConnStPnt = ConnEdPnt+1;
// 				continue;
// 			}
// 		}
// 		i=ConnStPnt;
// 		while( i<ConnEdPnt-1 )
// 		{
// 			if( RightPnt[i]-RightPnt[i+1] > T )
// 			{
// 				j = i+1;
// 				while( j<ConnEdPnt-1 )
// 				{
// 					if( RightPnt[j+1] - RightPnt[j] > T )
// 						break;
// 					j++;
// 				}
// 				if( j < ConnEdPnt-1 && j-i < PEAKWIDTH )
// 					for( k = i+1; k<=j; k++)
// 							RightPnt[k] = max( RightPnt[k], RightPnt[i]+(RightPnt[j]-RightPnt[i])*(k-i)/(j-i)-5);
// 				i = j+1;
// 			}
// 			else
// 				i++;
// 		}
// 		for( i=ConnStPnt; i<ConnEdPnt; i++)
// 			SetHorLineToWhite( pImg, wb, h, RightPnt[i], i, w-1-RightPnt[i], TRUE );
// 		ConnStPnt = ConnEdPnt+1;
// 	}
// 
// //Erase top black strip
// 	ConnStPnt = 0;
// 	while( ConnStPnt < w)
// 	{
// 		while( TopPnt[ConnStPnt] == -1 )
// 		{
// 			if( ConnStPnt < w-1 )
// 				ConnStPnt++;
// 			else
// 				break;
// 		}
// 		if( ConnStPnt >= w-1 )	break;
// 		ConnEdPnt=ConnStPnt+1;
// 		while( ConnEdPnt < w )
// 		{
// 			if( TopPnt[ConnEdPnt] >= 0 )
// 				ConnEdPnt++;
// 			else
// 				break;
// 		}
// 		if( ConnStPnt>0 && ConnEdPnt < h &&  ConnEdPnt-ConnStPnt < 60 )
// 		{
// 			int	AverWidth = 0;
// 			for( i=ConnStPnt; i<ConnEdPnt; i++)
// 				AverWidth += TopPnt[i];
// 			AverWidth /= (ConnEdPnt-ConnStPnt);
// 			if( AverWidth < 60 )
// 			{
// 				ConnStPnt = ConnEdPnt+1;
// 				continue;
// 			}
// 		}
// 		i=ConnStPnt;
// 		while( i<ConnEdPnt-1 )
// 		{
// 			if( TopPnt[i+1]-TopPnt[i] > T )
// 			{
// 				j = i+1;
// 				while( j<ConnEdPnt-1 )
// 				{
// 					if( TopPnt[j] - TopPnt[j+1] > T )
// 						break;
// 					j++;
// 				}
// 				if( j<ConnEdPnt-1 && j-i < PEAKWIDTH )
// 					for( k = i+1; k<=j; k++)
// 							TopPnt[k] = min( TopPnt[k], TopPnt[i]+(TopPnt[j]-TopPnt[i])*(k-i)/(j-i)+5);
// 				i = j+1;
// 			}
// 			else
// 				i++;
// 		}
// 		for( i=ConnStPnt; i<ConnEdPnt; i++)
// 			SetVerLineToWhite( pImg, wb, h, i, 0, min( h-1, TopPnt[i]+1 ), TRUE );
// 		ConnStPnt = ConnEdPnt+1;
// 	}
// 
// //Erase bottom black strip
// 	ConnStPnt = 0;
// 	while( ConnStPnt < w)
// 	{
// 		while( BottomPnt[ConnStPnt] == h )
// 		{
// 			if( ConnStPnt < w-1 )
// 				ConnStPnt++;
// 			else
// 				break;
// 		}
// 		if( ConnStPnt >= w-1 )	break;
// 		ConnEdPnt=ConnStPnt+1;
// 		while( ConnEdPnt < w)
// 		{
// 			if( BottomPnt[ConnEdPnt] < h )
// 				ConnEdPnt++;
// 			else
// 				break;
// 		}
// 		if( ConnStPnt>0 && ConnEdPnt < h && ConnEdPnt-ConnStPnt < 60 )
// 		{
// 			int	AverWidth = 0;
// 			for( i=ConnStPnt; i<ConnEdPnt; i++)
// 				AverWidth += (h-BottomPnt[i]);
// 			AverWidth /= (ConnEdPnt-ConnStPnt);
// 			if( AverWidth < 60 )
// 			{
// 				ConnStPnt = ConnEdPnt+1;
// 				continue;
// 			}
// 		}
// 		i=ConnStPnt;
// 		while( i<ConnEdPnt-1 )
// 		{
// 			if( BottomPnt[i]-BottomPnt[i+1] > T )
// 			{
// 				j = i+1;
// 				while( j<ConnEdPnt-1 )
// 				{
// 					if( BottomPnt[j+1] - BottomPnt[j] > T )
// 						break;
// 					j++;
// 				}
// 				if( j<ConnEdPnt-1 && j-i < PEAKWIDTH )
// 					for( k = i+1; k<=j; k++)
// 							BottomPnt[k] = max( BottomPnt[k], BottomPnt[i]+(BottomPnt[j]-BottomPnt[i])*(k-i)/(j-i)-5);
// 				i = j+1;
// 			}
// 			else
// 				i++;
// 		}
// 		for( i=ConnStPnt; i<ConnEdPnt; i++)
// 			SetVerLineToWhite( pImg, wb, h, i, BottomPnt[i], h-1-BottomPnt[i], TRUE );
// 		ConnStPnt = ConnEdPnt+1;
// 	}
// 
// 	//Free memory
// 	free( LeftPnt );
// 	free( RightPnt );
// 	free( TopPnt );
// 	free( BottomPnt );
// 	Img.UnlockRawImg();
// 	ImgD.UnlockRawImg();
// 	return 0;
// }

/****************************************************************************************************
/*	Name:		EraseBlackStrip
/*	Funcation:	Erase black strip around the image created by unapprociate scanning parameters.
/*				Save the processed image to files				
/*
/***************************************************************************************************/
// int CImageTool::EraseBlackStrip ( char* sFnImage )
// {
// 	CImage		Img;
// 	Img.Read(sFnImage);
// 	EraseBlackStrip( Img );
// 	CString	csFileExt = GetFileExtension( sFnImage );
// 	char	sFnResultImage[256];
// 	if( csFileExt != "bmp" )
// 		ChangeFileExt( sFnResultImage, sFnImage, "bmp");
// 	else
// 		ChangeFileExt( sFnResultImage, sFnImage,  "tif");
// 	Img.Write( sFnResultImage );
// 	return 0;
// }

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS USED TO TEST IF A WHOLE LINE OR A WHOLE COLUMN IMAGE IS WHITE 
////////////////////////////////////////////////////////////////////////

/********************************************************************************
/*	Name:		AllVerWhite
/*	Funcation:	Test if pixels on a vertical line are all white
/*				Caution!!! Though the speed is very fast, but there may be large 
/*				position error when the line is long.
/*	Parameter:	p -- Image data pointer
/*				wb -- Image width in bytes
/*				h -- Image height
/*				s -- Start point
/*				e -- End point
/*	Return:		TRUE -- Succeed
/*				FALSE -- Failed
/********************************************************************************/
int CImageTool::AllVerWhite ( DSCC_BYTE *p, int wb, int h, DSCC_POINT s, DSCC_POINT e)
{	
	int i, j, x, step;
	if(s.y > e.y)	return FALSE;
	if(abs(s.x - e.x) > e.y-s.y)	return FALSE;
	DSCC_BYTE mask[8] = {128, 64, 32, 16, 8, 4, 2, 1};
	if(s.x == e.x )		step = e.y-s.y;
	else				
		step = (e.y-s.y)/(abs(s.x-e.x)+1);
	x = s.x;
	for(i=s.y; i<e.y-step; i+=step)
	{
		for(j=0; j<abs(step); j++)
			if( *(p+(h-1-i-j)*wb+x/8) & mask[x%8])	
				return FALSE;
		if(e.x > s.x)	x++;
		else		x--;
	}
	for(;i<=e.y; i++)
	{
		if( *(p+(h-1-i)*wb+x/8) & mask[x%8] )
			return FALSE;
	}

	return TRUE;	
}

/********************************************************************************
/*	Name:		AllHorWhite
/*	Funcation:	Test if pixels on a horizontal line are all white
/*	Parameter:	p -- Image data pointer
/*				wb -- Image width in bytes
/*				h -- Image height
/*				x -- X coordinate of the start point
/*				y -- Y coordinate of the start point
/*				w -- Line length
/*	Return:		TRUE -- Succeed
/*				FALSE -- Failed
/********************************************************************************/
int CImageTool::AllHorWhite ( DSCC_BYTE *p, int wb, int h, int x, int y, int w )
{
    int i ;
    DSCC_BYTE mask[8] = {128,64,32,16,8,4,2,1} ;
    p += (h-1-y)*wb + x/8 ;
    if ( x%8 > 0 )
    {
        for ( i=x%8 ; i<8 && w>0 ; i++, w-- )
            if ( *p & mask[i] )
                return FALSE ;
        p++ ;
    }
    while ( w >= 8 )
    {
        if ( *p++ != 0 )
            return FALSE ;
        w -= 8 ;
    }
    for ( i=0 ; i<w ; i++ )
        if ( *p & mask[i] )
            return FALSE ;
    return TRUE ;
}

/********************************************************************************
/*	Name:		AllWhite
/*	Funcation:	Test if pixels on a line are all white
/*				Caution!!! Though the speed is very fast, but there may be large 
/*				position error when the line is long.
/*	Parameter:	p -- Image data pointer
/*				wb -- Image width in bytes
/*				h -- Image height
/*				s -- Start point
/*				e -- End point
/*	Return:		TRUE -- Succeed
/*				FALSE -- Failed
/********************************************************************************/
int CImageTool::AllWhite ( DSCC_BYTE *p, int wb, int h, DSCC_POINT s, DSCC_POINT e )
{
	if ( abs(s.x - e.x) > abs(s.y - e.y) )
	{
		int w = e.x-s.x+1 ;
		if ( w <= 0 )
			return FALSE ;

		int step = w / (abs(e.y-s.y)+1) ;
		if ( step == 0 )
			return FALSE ;

		int dy = (e.y>s.y) ? 1:-1 ;
		int x = s.x ;
		int y = s.y ;
		while ( x <= e.x )
		{
			if ( !AllHorWhite(p, wb, h, x, y, min((long)step, e.x-x+1)) )
				return FALSE ;
			x += step ;
			y += dy ;
		}
		return TRUE ;
	}
	else
		return AllVerWhite(p, wb, h, s, e);
}

/********************************************************************************
/*	Name:		GrayAllWhite
/*	Funcation:	Test if pixels on a line are all white, for gray image
/*				Caution!!! Though the speed is very fast, but there may be large 
/*				position error when the line is long.
/*	Parameter:	p -- Image data pointer
/*				wb -- Image width in bytes
/*				h -- Image height
/*				s -- Start point
/*				e -- End point
/*				Gray -- The gray level to set
/*	Return:		TRUE -- Succeed
/*				FALSE -- Failed
/********************************************************************************/
int CImageTool::GrayAllWhite ( DSCC_BYTE *p, int wb, int h, DSCC_POINT s, DSCC_POINT e, DSCC_BYTE Gray )
{
    if(fabs((double)(s.x - e.x)) > fabs((double)(s.y - e.y)))
	{//Hor
		int xs = min(s.x, e.x);
		int xe = max(s.x, e.x);
		int ys, ye;
		if(xs == s.x)
		{	ys = s.y ;	ye = e.y;}
		else
		{	ys = e.y;	ye = s.y;}

		int step = (xe-xs) / (abs(ye-ys)+1) ;
		if ( step == 0 )
			return FALSE ;

		int dy = (ye>ys) ? 1:-1 ;
		int x = xs;
		int y = ys;
		while ( x <= xe )
		{
			int i = 0;
			while(i<step && x+i<xe)
			{	if ( *(p+(h-1-y)*wb+x+i) < Gray)
					return FALSE ;
				i++;
			}
			x += step ;
			y += dy ;
		}
	}
	else
	{//Ver
		int ys = min(s.y, e.y);
		int ye = max(s.y, e.y);
		int xs, xe;
		if(ys == s.y)
		{	xs = s.x;	xe = e.x;}
		else
		{	xs = e.x;   xe = s.x;}
		int step = (ye-ys)/(abs(xe-xs)+1) ;
		if ( step == 0)		
			return FALSE;

		int dx = (xe>xs) ? 1:-1 ;
		int y = ys;
		int x = xs;
		while ( y<= ye)
		{
			int	i = 0;
			while(i<step && y+i<ye)
			{
				if( *(p+(h-1-y-i)*wb + x) < Gray)		return FALSE;
				i++;
			}
			y += step;
			x += dx;
		}
	}
    return TRUE ;
}

/********************************************************************************
/*	Name:		ColorAllWhite
/*	Funcation:	Test if pixels on a line are all white, for color image
/*				Caution!!! Though the speed is very fast, but there may be large 
/*				position error when the line is long.
/*	Parameter:	p -- Image data pointer
/*				wb -- Image width in bytes
/*				h -- Image height
/*				s -- Start point
/*				e -- End point
/*				Channel -- Which channel in RGB to test
/*				Gray -- Intensity level to set
/*	Return:		TRUE -- Succeed
/*				FALSE -- Failed
/********************************************************************************/
int CImageTool::ColorAllWhite ( DSCC_BYTE *p, int wb, int h, DSCC_POINT s, DSCC_POINT e, int Channel, DSCC_BYTE Gray )
{
    if(fabs((double)(s.x - e.x)) > fabs((double)(s.y - e.y)))
	{//Hor
		int xs = min(s.x, e.x);
		int xe = max(s.x, e.x);
		int ys, ye;
		if(xs == s.x)
		{	ys = s.y ;	ye = e.y;}
		else
		{	ys = e.y;	ye = s.y;}

		int step = (xe-xs) / (abs(ye-ys)+1) ;
		if ( step == 0 )
			return FALSE ;

		int dy;
		if(ye>ys)			dy = 1;
		else if( ye< ys)	dy = -1;
		else				dy = 0;
		int x = xs;
		int y = ys;
		while ( x <= xe )
		{
			if( y<0 || y>=h )	
				return FALSE;
			if( x<0 || x>=wb)	
				return FALSE;
			int i = 0;
			while(i<step && x+i<xe)
			{	if ( *(p+(h-1-y)*wb+3*(x+i)+Channel) < Gray)
					return FALSE ;
				i++;
			}
			x += step ;
			y += dy ;
		}
	}
	else
	{//Ver
		int ys = min(s.y, e.y);
		int ye = max(s.y, e.y);
		int xs, xe;
		if(ys == s.y)
		{	xs = s.x;	xe = e.x;}
		else
		{	xs = e.x;   xe = s.x;}
		int step = (ye-ys)/(abs(xe-xs)+1) ;
		if ( step == 0)		
			return FALSE;

		int dx;
		if(xe>xs)		dx = 1;
		else if( xe<xe) dx = -1;
		else			dx = 0;
		int y = ys;
		int x = xs;
		while ( y<= ye)
		{
			if( y<0 || y>=h )
				return FALSE;
			if( x<0 || x>=wb)
				return FALSE;
			int	i = 0;
			while(i<step && y+i<ye)
			{
				if( *(p+(h-1-y-i)*wb + 3*x + Channel) < Gray)
					return FALSE;
				i++;
			}
			y += step;
			x += dx;
		}
	}
    return TRUE ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//	Skew correction
/////////////////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************************************
/*	Name:		BlockCopyBi0
/*	Function:	Copy pixels, written in assemble language to accelate, called by BlockCopyBi
/*
/************************************************************************************************/
// int CImageTool::BlockCopyBi0 ( DSCC_BYTE *ImgS, int xs, int ys, int wInBytesS,
//                                DSCC_BYTE *ImgD, int xd, int yd, int wInBytesD,
//                                int w, int h )
// {
//         DSCC_BYTE MvS, MvD ;
//         
//         if ( h==0 || w== 0 )    // No work at all
//             return 1 ;
// // See if the parameters are valid 
//         if ( xs<0 || xd<0 || ys<0 || yd<0 || w<0 || h<0 || wInBytesS<=0 || wInBytesD<=0 ||
//              xs+w-1>(wInBytesS<<3) || xd+w-1>(wInBytesD<<3) )
//             return 2 ;
//             
// // Calculate the starting address of Source and Destination
//         ImgS += wInBytesS*ys+(xs>>3) ;
//         ImgD += wInBytesD*yd+(xd>>3) ;
// 
//         MvS = xs%8 ;
//         MvD = 8-(xd%8) ;
//         _asm
//         {
//                 mov bh,1
//                 mov cl,MvS
//                 shl bh,cl
// 
//                 mov esi,ImgS
//                 mov edi,ImgD
// 
//     NextLine:
//                 push edi
//                 push esi
//                 push bx
//                 
//                 mov ah,[esi]
//                 inc esi
//                 mov cl,MvS
//                 shl ah,cl
//                 mov al,[edi]
//                 stc
//                 rcr al,1
//                 mov cl,MvD
//                 dec cl
//                 shr al,cl
//                 
//                 mov ecx,w
//     NextRow:
//                 shl ah,1
//                 rcl al,1
//                 jnc ThisByteD
//                 stosb
//                 mov al,1
//     ThisByteD:
//                 shl bh,1
//                 jnc ThisByteS
//                 mov ah,[esi]
//                 inc esi
//                 mov bh,1
//     ThisByteS:
//                 loop NextRow
//                 cmp al,1
//                 jz NoExtra
//                 mov ah,0ffh
//     RollOn:                
//                 shl ah,1
//                 shl al,1
//                 jnc RollOn
//                 not ah
//                 and [edi],ah        ; clear the hi bits
//                 or [edi],al
//     NoExtra:
//                 pop bx
//                 pop esi
//                 pop edi
//                 add esi,wInBytesS
//                 add edi,wInBytesD
//                 dec h
//                 jnz NextLine
//         }
//         return 0 ;
// }

/************************************************************************************************
/*	Name:		BlockCopyBi
/*	Function:	Copy pixels, written in assemble language to accelate, used for skew correction
/*				of binary images
/*
/************************************************************************************************/
// int CImageTool::BlockCopyBi ( DSCC_BYTE *ImgS, int xs, int ys, int wInBytesS,
//                              DSCC_BYTE *ImgD, int xd, int yd, int wInBytesD,
//                              int w, int h )
// {
//         DSCC_BYTE Mvs, LeftS, LeftD, RightD ;
//         int Bytes ;
// 
//         if ( h==0 || w== 0 )    // No work at all
//             return 1 ;
// // See if the parameters are valid 
//         if ( xs<0 || xd<0 || ys<0 || yd<0 || w<0 || h<0 || wInBytesS<=0 || wInBytesD<=0 ||
//              xs+w-1>(wInBytesS<<3) || xd+w-1>(wInBytesD<<3) )
//             return 2 ;
//              
//         if ( w<= 8 )
//             return BlockCopyBi0 ( ImgS,  xs, ys, wInBytesS, ImgD, xd, yd, wInBytesD, w, h ) ;
// 
// // Calculate the starting address of Source and Destination
//         ImgS += wInBytesS*ys+(xs>>3) ;
//         ImgD += wInBytesD*yd+(xd>>3) ;
// 
//         LeftS = 8-(xs%8) ;
//         LeftD = (8-(xd%8))%8 ;
//         Bytes = (w-LeftD)/8 ;
//         RightD = w - Bytes*8 - LeftD ;
//         
//         if ( LeftS >= LeftD )
//         {
//             Mvs = LeftS - LeftD ;
//             _asm
//             {
//                     mov cl,LeftD
//                     mov bl,0ffh
//                     shl bl,cl
//                     mov cl,RightD
//                     mov bh,0ffh
//                     shr bh,cl
//                     
//                     mov esi,ImgS
//                     mov edi,ImgD
//                 
//         NextLine:
//                     mov edx,Bytes
//                     mov cl,Mvs
//                     push edi
//                     push esi
//                     
//                     cmp bl,0ffh
//                     jz NoLeft
//                     lodsb
//                     dec esi
//                     shr al,cl
//                     and [edi],bl    ;bl=111..00, clear low bits
//                     not bl             ;bl=000..11
//                     and al,bl        ;clear hi bits
//                     not bl             ;restore
//                     or [edi],al
//                     inc edi
//         NoLeft:
//                     cmp edx,0
//                     jz NoBytes
//         NextByte:
//                     lodsw
//                     dec esi
//                     ror ax,cl
//                     mov [edi],ah
//                     inc edi
//                     dec edx
//                     jnz NextByte
//         NoBytes:
//                     cmp bh,0ffh
//                     jz NoRight
//                     lodsw
//                     ror ax,cl
//                     and [edi],bh ;bl=000..11, clear hi bits
//                     not bh             ;bl=111..00
//                     and ah,bh        ;clear low bits
//                     not bh             ;restore
//                     or [edi],ah
//         NoRight:
//                     pop esi
//                     pop edi
//                     add esi,wInBytesS
//                     add edi,wInBytesD
//                     dec h
//                     jnz NextLine
//             }
//         }
//         else
//         {
//             Mvs = LeftD - LeftS ;
//             _asm
//             {
//                     mov cl,LeftD
//                     mov bl,0ffh
//                     shl bl,cl
//                     mov cl,RightD
//                     mov bh,0ffh
//                     shr bh,cl
//                     
//                     mov esi,ImgS
//                     mov edi,ImgD
//                 
//     _NextLine:
//                     mov edx,Bytes
//                     mov cl,Mvs
//                     push edi
//                     push esi
//                     
//                     cmp bl,0ffh
//                     jz _NoLeft
//                     lodsw
//                     dec esi
//                     rol ax,cl
//                     and [edi],bl    ;bl=111..00, clear low bits
//                     not bl             ;bl=000..11
//                     and al,bl        ;clear hi bits
//                     not bl             ;restore
//                     or [edi],al
//                     inc edi
//         _NoLeft:
//                     cmp edx,0
//                     jz _NoBytes
//         _NextByte:
//                     lodsw
//                     dec esi
//                     rol ax,cl
//                     stosb
//                     dec edx
//                     jnz _NextByte
//         _NoBytes:
//                     cmp bh,0ffh
//                     jz _NoRight
//                     lodsw
//                     rol ax,cl
//                     and [edi],bh ;bl=000..11, clear hi bits
//                     not bh             ;bl=111..00
//                     and al,bh        ;clear low bits
//                     not bh             ;restore
//                     or [edi],al
//         _NoRight:
//                     pop esi
//                     pop edi
//                     add esi,wInBytesS
//                     add edi,wInBytesD
//                     dec h
//                     jnz _NextLine
//             }
//         }
//         return 0 ;
// }

/************************************************************************************************
/*	Name:		BinaryImageSkewCorrection
/*	Function:	Skew correction for binary image
/*
/************************************************************************************************/
// CImage * CImageTool::BinaryImageSkewCorrection ( CImage* _SrcImg, double Angle )
// {
//     CImage *SrcImg, *DstImg ;
//     BYTE *pImgD, *pImgS ;
//     SrcImg = _SrcImg ;
// 
//     int wd, hd, wb, step, i ;
//     int ws= SrcImg->GetWidth() ;
//     int wbs = SrcImg->GetLineSizeInBytes() ;
//     int hs=SrcImg->GetHeight() ;
// 
//     if ( fabs(Angle) < 1e-6 )
//         step = ws ;
//     else
//         step = (int)(1/fabs(Angle)) ;
// 
//     SrcImg = _SrcImg ;
//     if ( ws>step || hs>step )
//     {
//         if ( ws > step )
//         {
//             pImgS = SrcImg->LockRawImg() ;
//             int dh = (ws+step-1)/step ;
//             wd=ws, hd=hs+dh ;
//             DstImg = new CImage() ;
//             DstImg->Initialize ( wd, hd, 1, 1 ) ;
//             pImgD = DstImg->LockRawImg() ;
//             wb = DstImg->GetLineSizeInBytes() ;
//             for ( i=0 ; i<dh ; i++ )
//             {
//                 int ww = min ( step, ws-i*step ) ;
//                 if ( Angle > 0 )
//                     BlockCopyBi ( pImgS, i*step, 0, wbs, pImgD, i*step, dh-1-i, wb, ww, hs ) ;
//                 else
//                     BlockCopyBi ( pImgS, i*step, 0, wbs, pImgD, i*step, i, wb, ww, hs ) ;
//             }
//             DstImg->UnlockRawImg() ;
//             SrcImg->UnlockRawImg() ;
//             SrcImg=DstImg, hs=hd ;
//         }
//         if ( hs > step )
//         {
//             pImgS = SrcImg->LockRawImg() ;
//             int dw = (hs+step-1)/step ;
//             wd=ws+dw, hd=hs ;
//             DstImg = new CImage() ;
//             DstImg->Initialize ( wd, hd, 1, 1 ) ;
//             pImgD = DstImg->LockRawImg() ;
//             wb = DstImg->GetLineSizeInBytes() ;
//             for ( i=0 ; i<dw ; i++ )
//             {
//                 int hh = min ( step, hs-i*step ) ;
//                 if ( Angle > 0 )
//                     BlockCopyBi ( pImgS, 0, hs-i*step-hh, wbs, pImgD, dw-1-i, hd-i*step-hh, wb, ws, hh ) ;
//                 else
//                     BlockCopyBi ( pImgS, 0, hs-i*step-hh, wbs, pImgD, i, hd-i*step-hh, wb, ws, hh ) ;
//             }
//             DstImg->UnlockRawImg() ;
//             SrcImg->UnlockRawImg() ;
//             if ( SrcImg != _SrcImg )
//                 delete SrcImg ;
//         }
//     }
//     else
//     {
//         pImgS = SrcImg->LockRawImg() ;
//         wd=ws, hd=hs ;
//         DstImg = new CImage() ;
//         DstImg->Initialize ( wd, hd, 1, 1 ) ;
//         pImgD = DstImg->LockRawImg() ;
//         memcpy ( pImgD, pImgS, wbs*hd ) ;
//         DstImg->UnlockRawImg() ;
//         SrcImg->UnlockRawImg() ;
//     }
//     return DstImg ;
// }

/************************************************************************************************
/*	Name:		BinaryImageSkewCorrection
/*	Function:	Skew correction for binary image
/*
/************************************************************************************************/
// int CImageTool::BinaryImageSkewCorrection ( CImage &_SrcImg, CImage &_DstImg, double Angle )
// {
//     CImage SrcImg, DstImg ;
//     BYTE *pImgD, *pImgS ;
//     int ws, wbs, hs, wd, hd, wb, step, i ;
// 
//     ws= _SrcImg.GetWidth() ;
//     wbs = _SrcImg.GetLineSizeInBytes() ;
//     hs = _SrcImg.GetHeight() ;
// 
//     if ( fabs(Angle) < 1e-6 )
//         step = ws ;
//     else
//         step = (int)(1/fabs(Angle)) ;
// 
//     if ( ws>step || hs>step )
//     {
//         SrcImg.Initialize(_SrcImg) ;
//         if ( ws > step )
//         {
//             pImgS = SrcImg.LockRawImg() ;
//             int dh = (ws+step-1)/step ;
//             wd=ws, hd=hs+dh ;
//             DstImg.Initialize ( wd, hd, 1, 1 ) ;
//             pImgD = DstImg.LockRawImg() ;
//             wb = DstImg.GetLineSizeInBytes() ;
//             for ( i=0 ; i<dh ; i++ )
//             {
//                 int ww = min ( step, ws-i*step ) ;
//                 if ( Angle > 0 )
//                     BlockCopyBi ( pImgS, i*step, 0, wbs, pImgD, i*step, dh-1-i, wb, ww, hs ) ;
//                 else
//                     BlockCopyBi ( pImgS, i*step, 0, wbs, pImgD, i*step, i, wb, ww, hs ) ;
//             }
//             DstImg.UnlockRawImg() ;
//             SrcImg.UnlockRawImg() ;
//             SrcImg.Initialize(DstImg) ;
//             hs=hd ;
//         }
//         if ( hs > step )
//         {
//             pImgS = SrcImg.LockRawImg() ;
//             int dw = (hs+step-1)/step ;
//             wd=ws+dw, hd=hs ;
//             DstImg.Initialize ( wd, hd, 1, 1 ) ;
//             pImgD = DstImg.LockRawImg() ;
//             wb = DstImg.GetLineSizeInBytes() ;
//             for ( i=0 ; i<dw ; i++ )
//             {
//                 int hh = min ( step, hs-i*step ) ;
//                 if ( Angle > 0 )
//                     BlockCopyBi ( pImgS, 0, hs-i*step-hh, wbs, pImgD, dw-1-i, hd-i*step-hh, wb, ws, hh ) ;
//                 else
//                     BlockCopyBi ( pImgS, 0, hs-i*step-hh, wbs, pImgD, i, hd-i*step-hh, wb, ws, hh ) ;
//             }
//             DstImg.UnlockRawImg() ;
//             SrcImg.UnlockRawImg() ;
//         }
//     }
//     else
//         DstImg.Initialize(_SrcImg) ;
// 
//     _DstImg.Initialize(DstImg) ;
//     return 0 ;
// }

/************************************************************************************************
/*	Name:		SkewCorrectionCut
/*	Function:	Skew correction for binary image, then cut some extra border brought by skewness
/*
/************************************************************************************************/
// int	CImageTool::SkewCorrectionCut( CImage &Img, double dSkewAngle, CImage &CorrectedImg )
// {
// 	BinaryImageSkewCorrection( Img, CorrectedImg, dSkewAngle );
// 	return 0;
// }
/************************************************************************************************
/*	Name:		SkewCorrectionCut
/*	Function:	Skew correction for binary image, then cut some extra border brought by skewness
/*
/************************************************************************************************/
// int	CImageTool::SkewCorrectionCut( CImage &Img, double dSkewAngle )
// {
// 	int		Width = Img.GetWidth();
// 	int		Height = Img.GetHeight();
// 	CRect	rc;
// 	CImage   CorrectedImg;
// 	BinaryImageSkewCorrection( Img, CorrectedImg, dSkewAngle );
// 	rc.top = (int)fabs( Width*tan(dSkewAngle)*15/20 );
// 	rc.left = (int)fabs( Height*tan(dSkewAngle)*15/20 );
// 	rc.left = rc.left/8*8;
// 	if( 2*rc.top >= Height )     rc.top = 0;
// 	if( 2*rc.left >= Width  )	 rc.left = 0;
// 	rc.bottom = CorrectedImg.GetHeight() - (int)fabs( Width*tan(dSkewAngle)*10/20 );
// 	rc.right = CorrectedImg.GetWidth() - (int)fabs( Height*tan(dSkewAngle)*10/20 );
// 	Img.Initialize( CorrectedImg.GethImg(), rc, DIB_1BIT );
// 	return 0;
// }
/************************************************************************************************
/*	Name:		ColorImageSkewCorrection
/*	Function:	Skew correction for color image, support several color format
/*
/************************************************************************************************/
// CImage * CImageTool::ColorImageSkewCorrection ( CImage* _SrcImg, double Angle )
// {
// 	int		nBitCount, nWhite;
// 	switch( _SrcImg->GetType() )
// 	{
// 	case DIB_4BIT:	
// 		nBitCount = 4;
// 		break;
// 	case DIB_8BIT:	
// 		nBitCount = 8;
// 		break;
// 	case DIB_24BIT:	
// 		nBitCount = 24;
// 		break;
// 	default:
// 		AfxMessageBox(TEXT("Error when calling function: ColorImageSlantCorrection, Not a color image!"));
// 		return NULL;
// 	}
// 
// 	CImage *SrcImg, *DstImg ;
//     BYTE *pImgD, *pImgS ;
//     SrcImg = _SrcImg ;
// 
// 	int		nLByte, nRByte;
//     int wd, hd, wb, step, i, j, k;
//     int ws= SrcImg->GetWidth() ;
//     int wbs = SrcImg->GetLineSizeInBytes() ;
//     int hs=SrcImg->GetHeight() ;
// 
//     if ( fabs(Angle) < 1e-6 )
//         step = ws ;
//     else
//         step = (int)(1/fabs(Angle)) ;
// 
//     SrcImg = _SrcImg ;
//     if ( ws>step || hs>step )
//     {
//         if ( ws > step )
//         {
//             pImgS = SrcImg->LockRawImg() ;
//             int dh = (ws+step-1)/step ;
//             wd=ws, hd=hs+dh ;
//             DstImg = new CImage() ;
//             DstImg->Initialize ( wd, hd, nBitCount, 1 ) ;
//             pImgD = DstImg->LockRawImg() ;
//             wb = DstImg->GetLineSizeInBytes() ;
// 						
// 						if( nBitCount == 8 || nBitCount == 4 )
// 						{	
// 							nWhite=0;
// 							LPBITMAPINFO lpbiS = (LPBITMAPINFO)GlobalLock(SrcImg->GethImg() ) ;
// 							LPBITMAPINFO lpbiD = (LPBITMAPINFO)GlobalLock(DstImg->GethImg() ) ;
// 							for( i=0; i< (1<<nBitCount); i++ )
// 							{
// 									lpbiD->bmiColors[i] = lpbiS->bmiColors[i];
// 									if( lpbiS->bmiColors[i].rgbRed + lpbiS->bmiColors[i].rgbGreen + lpbiS->bmiColors[i].rgbBlue >
// 											lpbiS->bmiColors[nWhite].rgbRed + lpbiS->bmiColors[nWhite].rgbGreen + lpbiS->bmiColors[nWhite].rgbBlue )
// 											nWhite = i;
// 							}
// 							if( nBitCount == 4 )
// 								nWhite = nWhite*16+nWhite;
// 						}
// 						else 
// 							nWhite = 255;
// 						memset ( pImgD, nWhite, hd*wb ) ;
// 
// 						for ( i=0 ; i<dh ; i++ )
//             {
// 	                int ww = min ( step, ws-i*step ) ;
// 									for( j=0; j<hs; j++)
// 									{
// 										if( Angle > 0 )
// 										{
// 											if( nBitCount == 4 )
// 											{
// 												if( i*step % 2 != 0 )
// 												{
// 													*( pImgD+(hd-1-(dh-1-i+j))*wb+i*step/2 ) &= 0xf0;
// 													*( pImgD+(hd-1-(dh-1-i+j))*wb+i*step/2 ) |= (*( pImgS+(hs-1-j)*wbs+i*step/2 )) & 0x0f;
// 													nLByte = i*step/2+1;
// 												}
// 												else
// 													nLByte = i*step/2;
// 												if( (i*step+ww) % 2 != 0 )
// 												{
// 													nRByte = (i*step+ww-1)/2-1;
// 													*( pImgD+(hd-1-(dh-1-i+j))*wb+nRByte+1 ) &= 0x0f;
// 													*( pImgD+(hd-1-(dh-1-i+j))*wb+nRByte+1 ) |= (*( pImgS+(hs-1-j)*wbs+nRByte+1 )) & 0xf0;
// 												}
// 												else 
// 													nRByte = (i*step+ww-1)/2;
// 												memcpy( pImgD+(hd-1-(dh-1-i+j))*wb+nLByte, pImgS+(hs-1-j)*wbs+nLByte, nRByte-nLByte+1 );
// 											}
// 											else if( nBitCount == 8 )
// 												memcpy( pImgD+(hd-1-(dh-1-i+j))*wb+i*step, pImgS+(hs-1-j)*wbs+i*step, ww );
// 											else if( nBitCount == 24 )
// 												memcpy( pImgD+(hd-1-(dh-1-i+j))*wb+i*step*3, pImgS+(hs-1-j)*wbs+i*step*3, 3*ww );
// 										}
// 										else
// 										{
// 											if( nBitCount == 4 )
// 											{
// 												if( i*step % 2 != 0 )
// 												{
// 													*( pImgD+(hd-1-(i+j))*wb+i*step/2 ) &= 0xf0;
// 													*( pImgD+(hd-1-(i+j))*wb+i*step/2 ) 	|= (*( pImgS+(hs-1-j)*wbs+i*step/2 )) & 0x0f;
// 													nLByte = i*step/2+1;
// 												}
// 												else
// 													nLByte = i*step/2;
// 												if( (i*step+ww) % 2 != 0 )
// 												{
// 													nRByte = (i*step+ww-1)/2-1;
// 													*( pImgD+(hd-1-(i+j))*wb+nRByte+1 ) &= 0x0f;
// 													*( pImgD+(hd-1-(i+j))*wb+nRByte+1 ) |= (*( pImgS+(hs-1-j)*wbs+nRByte+1 )) & 0xf0;
// 												}
// 												else 
// 													nRByte = (i*step+ww-1)/2;
// 												memcpy( pImgD+(hd-1-(i+j))*wb+nLByte, pImgS+(hs-1-j)*wbs+nLByte, nRByte-nLByte+1 );
// 											}
// 											else if( nBitCount == 8 )
// 												memcpy( pImgD+(hd-1-(i+j))*wb+i*step, pImgS+(hs-1-j)*wbs+i*step, ww );
// 											else if( nBitCount == 24 )
// 												memcpy( pImgD+(hd-1-(i+j))*wb+i*step*3, pImgS+(hs-1-j)*wbs+i*step*3, 3*ww );
// 										}
// 									}
// 						}
//             DstImg->UnlockRawImg() ;
//             SrcImg->UnlockRawImg() ;
//             SrcImg=DstImg, hs=hd ;
//         }
//         if ( hs > step )
//         {
//             pImgS = SrcImg->LockRawImg() ;
//             int dw = (hs+step-1)/step ;
//             wd=ws+dw, hd=hs ;
//             DstImg = new CImage() ;
//             DstImg->Initialize ( wd, hd, nBitCount, 1 ) ;
//             pImgD = DstImg->LockRawImg() ;
//             wb = DstImg->GetLineSizeInBytes() ;
// 						memset( pImgD, nWhite, hd*wb );
// 						if( nBitCount == 8 || nBitCount == 4 )
// 						{
// 							LPBITMAPINFO lpbiS = (LPBITMAPINFO)GlobalLock(SrcImg->GethImg() ) ;
// 							LPBITMAPINFO lpbiD = (LPBITMAPINFO)GlobalLock(DstImg->GethImg() ) ;
// 							for( i=0; i< (1<<nBitCount); i++ )
// 									lpbiD->bmiColors[i] = lpbiS->bmiColors[i];
// 						}
//             for ( i=0 ; i<dw ; i++ )
//             {
//                 int hh = min ( step, hs-i*step ) ;
// 								for( j=i*step; j<i*step+hh ; j++)
// 								{
// 									if( Angle > 0 )
// 									{
// 										if( nBitCount == 4 )
// 										{
// 											if( i%2 == 0 )
// 												memcpy( pImgD+(hd-1-j)*wb+i/2, pImgS+(hs-1-j)*wbs, wbs);  
// 											else
// 											{
// 												BYTE	*pLineS = pImgS+(hs-1-j)*wbs;
// 												BYTE	*pLineD = pImgD+(hd-1-j)*wb;
// 												*(pLineD+i/2) &= 0xf0;
// 												*(pLineD+i/2) |= (*pLineS) & 0x0f;
// 												*(pLineD+i/2+wbs-1) &= 0x0f;
// 												*(pLineD+i/2+wbs-1) |= *(pLineS+wbs-1) & 0xf0;
// 												memset( pLineD+i/2+1, 0, wbs );
// 												for( k=0;  k<wbs; k++ )
// 												{
// 													*(pLineD+k+i/2) |= (*(pLineS+k) & 0xf0);
// 													*(pLineD+k+i/2+1) |= (*(pLineS+k) & 0x0f);
// 												}
// 											}
// 										}
// 										else if( nBitCount == 8 )
// 											memcpy( pImgD+(hd-1-j)*wb+i,  pImgS+(hs-1-j)*wbs, wbs);
// 										else if( nBitCount == 24 )
// 											memcpy( pImgD+(hd-1-j)*wb+i*3,  pImgS+(hs-1-j)*wbs, wbs);
// 									}
// 									else
// 									{
// 										if( nBitCount == 4 )
// 										{
// 												if( (dw-1-i)%2 == 0 )
// 													memcpy( pImgD+(hd-1-j)*wb+(dw-1-i)/2, pImgS+(hs-1-j)*wbs, wbs);
// 												else
// 												{
// 													BYTE	*pLineS = pImgS+(hs-1-j)*wbs;
// 													BYTE	*pLineD = pImgD+(hd-1-j)*wb;
// 													*(pLineD+(dw-1-i)/2) &= 0xf0;
// 													*(pLineD+(dw-1-i)/2) |= (*pLineS) & 0x0f;
// 													*(pLineD+(dw-1-i)/2+wbs-1) &= 0x0f;
// 													*(pLineD+(dw-1-i)/2+wbs-1) |= *(pLineS+wbs-1) & 0xf0;
// 													memset( pLineD+(dw-1-i)/2+1, 0, wbs );
// 													for( k=0;  k<wbs; k++ )
// 													{
// 														*(pLineD+k+(dw-1-i)/2) |= (*(pLineS+k) & 0xf0);
// 														*(pLineD+k+(dw-1-i)/2+1) |= (*(pLineS+k) & 0x0f);
// 													}
// 												}
// 										}
// 										else if( nBitCount == 8 )
// 											memcpy( pImgD+(hd-1-j)*wb+(dw-1-i),	pImgS+(hs-1-j)*wbs, wbs);
// 										else if( nBitCount == 24 )
// 											memcpy( pImgD+(hd-1-j)*wb+(dw-1-i)*3,	pImgS+(hs-1-j)*wbs, wbs);
// 									}
// 								}
//           }
//           DstImg->UnlockRawImg() ;
//           SrcImg->UnlockRawImg() ;
//           if ( SrcImg != _SrcImg )
//               delete SrcImg ;
// 				}
//     }
//     else
//     {
// 		pImgS = SrcImg->LockRawImg() ;
//         wd=ws, hd=hs ;
//         DstImg = new CImage() ;
//         DstImg->Initialize ( wd, hd, nBitCount, 1 ) ;
// 				if( nBitCount == 8 || nBitCount == 4 )
// 				{	
// 					LPBITMAPINFO lpbiS = (LPBITMAPINFO)GlobalLock(SrcImg->GethImg() ) ;
// 					LPBITMAPINFO lpbiD = (LPBITMAPINFO)GlobalLock(DstImg->GethImg() ) ;
// 					for( i=0; i< (1<<nBitCount); i++ )
// 							lpbiD->bmiColors[i] = lpbiS->bmiColors[i];
// 				}
//         pImgD = DstImg->LockRawImg() ;
//         memcpy ( pImgD, pImgS, wbs*hd ) ;
//         DstImg->UnlockRawImg() ;
//         SrcImg->UnlockRawImg() ;
//     }
//     return DstImg ;
// }

/***********************************************************************
/*	Name:		Rotate
/*	Function:	Rotate Image by 90 degree, write by Pan S.Y., Not tested yet
/*				Called by Zoom()
/***********************************************************************/
// int CImageTool::Rotate ( DSCC_BYTE *ImgS, int SrcXs, int SrcXe, int SrcWdInBytes, int h,
//                    DSCC_BYTE *ImgD, int DstXs, int DstWdInBytes )
// {
//         int w ;
//         DSCC_BYTE SrcStartOff, DstLeft ;
// 
//         ImgS += SrcXs>>3 ;
//         ImgD += DstXs>>3 ;
//         w = SrcXe - SrcXs + 1 ;
//         SrcStartOff = SrcXs%8 ;
//         DstLeft = 8-(DstXs%8) ;
// 
//         _asm
//         {
//                 mov esi,ImgS
//                 mov edi,ImgD
//                 mov ebx,SrcWdInBytes
// 
//                 mov ah,80h
//                 mov cl,SrcStartOff
//                 shr ah,cl
//                 
//         NextRow:
//                 mov al,[edi]
//                 stc
//                 rcr al,1
//                 mov cl,DstLeft
//                 dec cl
//                 shr al,cl
// 
//                 push esi
//                 push edi
//                 
//                 mov ecx,h
//         NextLine:
//                 test [esi],ah
//                 stc
//                 jnz non_zero
//                 clc
//         non_zero:
//                 rcl al,1
//                 jnc ThisByteD
//                 stosb
//                 mov al,1
//         ThisByteD:
//                 add esi,ebx
//                 loop NextLine
//                 cmp al,1
//                 jz NoExtra
//                 mov dh,0ffh
//         RollOn:
//                 shl dh,1
//                 shl al,1
//                 jnc RollOn
//                 not dh
//                 and [edi],dh
//                 or [edi],al
//         NoExtra:
//                 pop edi
//                 pop esi
//                 shr ah,1
//                 jnc ThisByteS
//                 inc esi
//                 mov ah,80h
//         ThisByteS:
//                 add edi,DstWdInBytes
//                 dec w
//                 jnz NextRow	
//         }
//         return 0 ;
// }

/************************************************************************
/*  Name:		UpsideDown
/*	Function:	Rotate image upside down
/*	Parameter:	ImgS -- Original image
/*				wb 	 -- Line size in byes
/*				h    -- Image height
/*				ImgD -- Rotated image		
/***********************************************************************/
int CImageTool::UpsideDown( DSCC_BYTE *ImgS, int wb, int h, DSCC_BYTE *&ImgD )
{
	ImgD = (DSCC_BYTE*)malloc( sizeof(DSCC_BYTE)*wb*h );
	for( int i=0; i<h; i++ )
		memcpy( ImgD+wb*i, ImgS+wb*(h-1-i), wb );
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//	Other tools
/////////////////////////////////////////////////////////////////////////////////////////////////////

/***********************************************************************
/*	Name:		ShiftImage
/*	Function:	Shift image
/*
/***********************************************************************/
int	CImageTool::ShiftImage(DSCC_BYTE* pByteS, int w, int h, int wb, DSCC_BYTE* pByte, int RowShift, int ColShift)
{
	memset(pByte,0, wb*h);
	if(RowShift<0)
	{
		for(int i=-RowShift; i<h; i++)
			memcpy(pByte+(h-1-(i+RowShift))*wb, pByteS+(h-1-i)*wb, wb);
	}
	else
		for(int i=0; i<h-RowShift; i++)
			memcpy(pByte+(h-1-(i+RowShift))*wb, pByteS+(h-1-i)*wb, wb);

	int		ByteShift, BitShift, ByteRS, ByteLS;
	DSCC_BYTE*	pLine;
	if(ColShift>0)
	{
		ByteShift = ColShift/8;
		BitShift = ColShift%8;
		for(int i=0; i<h; i++)
		{	
			pLine = pByte+i*wb;
			int j = wb - 1 - ByteShift;
			for(; j>=1; j--)
			{
				ByteRS = (*(pLine+j))>>BitShift;
				ByteLS = ((*(pLine+j-1))<<(8-BitShift))%256;
				*(pLine+j+ByteShift) = ByteLS | ByteRS;
			}
			*(pLine+ByteShift) = (*pLine)>>BitShift;
			for(j=0; j<ByteShift; j++)
				*(pLine+j) = 0;
		}
	}
	else
	{
		ByteShift = (-ColShift)/8;
		BitShift = (-ColShift)%8;
		for(int i=0; i<h; i++)
		{
			pLine = pByte+i*wb;
			int j = ByteShift;
			for(; j<wb-1; j++)
			{
				ByteLS = ( (*(pLine+j)) <<BitShift)%256;
				ByteRS = (*(pLine+j+1))>>(8-BitShift);
				*(pLine+j-ByteShift) = ByteLS | ByteRS;
			}
			*(pLine+wb-1-ByteShift) = ( (*(pLine+wb-1))<<BitShift )%256;
			for(j=wb-1-ByteShift; j<wb; j++)
				*(pLine+j) = 0;
		}
	}
	return 0;
}

/***********************************************************************
/*	Name:		GetBound
/*	Function:	Get bounding box of a image, supporting only binary image
/*
/***********************************************************************/
int CImageTool::GetBound(CImage &Image, DSCC_RECT &rRange)
{
	DSCC_BYTE	mask[8] = {128, 64, 32, 16, 8, 4, 2, 1};
	if(Image.GetType() != DIB_1BIT )
	{
		rRange.left = 0;
		rRange.right = Image.GetWidth()-1;
		rRange.top = 0;
		rRange.bottom = Image.GetHeight()-1;
		return 0;
	}
	int i, j, k, w, wb, h;
	DSCC_BYTE *p = Image.LockRawImg(); 
	w=Image.GetWidth ();
	h=Image.GetHeight ();
	wb=Image.GetLineSizeInBytes ();
	i=0;
	while(i<h)
	{
		for(j=0; j<wb; j++)
			if(*(p+(h-1-i)*wb+j) != 0)	break;
		if(j==wb)	i++;
		else		break;
	}
	rRange.top = i;
	i=h-1;
	while(i>0)
	{
		for(j=0; j<wb; j++)
			if(*(p+(h-1-i)*wb+j) != 0)	break;
		if(j==wb)	i--;
		else		break;
	}
	rRange.bottom = i;

	rRange.left = (wb+1)*8;
	j=0;
	while(j<wb)
	{
		for(i=0; i<h; i++)
		{
			if(*(p+i*wb+j) !=0)	
			{
				for( k=0; k<8; k++ )
				{
					if( *(p+i*wb+j) & mask[k] )
					{
						rRange.left = min( rRange.left, (long)(8*j+k));
						break;
					}
				}
			}
		}
		if(rRange.left != (wb+1)*8)	
			break;
		else
			j++;
	}

	j=wb-1;
	rRange.right = 0;
	while(j>0)
	{
		for(i=0; i<h; i++)
		{
			if(*(p+i*wb+j) != 0)
			{
				for( k=7; k>=0; k-- )
				{
					if( *(p+i*wb+j) & mask[k] )
					{
						rRange.right = max( rRange.right, long(8*j+k));
						break;
					}
				}
			}
		}
		if( rRange.right != 0 )
			break;
		else 
			j--;
	}
	Image.UnlockRawImg ();
	return 0;
}

/*****************************************************************************
/*	Name:		GetCharBound
/*	Funcation:	Get	bounding box of a character, supporting binary/gray/color
/*				image. Extend char or cell boundary box to include all black 
/*				pixels as possible
/*
/*****************************************************************************/
int CImageTool::GetCharBound(CImage &Image, DSCC_POINT &TopL, DSCC_POINT &TopR, DSCC_POINT &BtmL, DSCC_POINT &BtmR)
{
	int i, w, wb, h;
	DSCC_BYTE *pImg = Image.LockRawImg(); 
	w=Image.GetWidth ();
	h=Image.GetHeight ();
	wb=Image.GetLineSizeInBytes ();
	int	ImgType = Image.GetType();

	int	bAllWhite;
    int MaxDy, MaxDx ;
	
    MaxDy = (BtmL.y - TopL.y) / 3 ;
	MaxDx = (TopR.x - TopL.x) / 3;
    for ( i=0 ; i<MaxDy ; i++ ) 
    {
 		if( TopL.y >= h-1 || TopR.y >= h-1 || TopL.y <= 0 || TopR.y <= 0)	
			break;
       if ( ImgType == DIB_1BIT)
			bAllWhite = AllWhite(pImg, wb, h, TopL, TopR);
		else if ( ImgType == DIB_8BIT)
			bAllWhite = GrayAllWhite(pImg, wb, h, TopL, TopR, 128);
		else if ( ImgType == DIB_24BIT )
			bAllWhite = ColorAllWhite(pImg, wb, h, TopL, TopR, 1, 128 );
		else
			return -1;
		if ( bAllWhite )			break ; 
		TopL.y-- ; 
		TopR.y-- ;
    }

    for ( i=0 ; i<MaxDy ; i++ )
    {
		if( BtmL.y <= 0 || BtmR.y <= 0 || BtmL.y >= h-1 || BtmR.y >= h-1)	
			break;
        if ( ImgType == DIB_1BIT)
			bAllWhite = AllWhite(pImg, wb, h, BtmL, BtmR);
		else if ( ImgType == DIB_8BIT)
			bAllWhite = GrayAllWhite(pImg, wb, h, BtmL, BtmR, 128);
		else if ( ImgType == DIB_24BIT)
			bAllWhite = ColorAllWhite(pImg, wb, h, BtmL, BtmR, 1, 128);
		else
			return -1;
		if ( bAllWhite )			break ;
        BtmL.y++ ; 
		BtmR.y++ ;
    }

	for ( i=0; i<MaxDx; i++)
	{
		if( TopL.x <= 0 || BtmL.x <= 0 || TopL.x >= w-1 || BtmL.x >= w-1 )
			break;
		if ( ImgType == DIB_1BIT)
			bAllWhite = AllWhite(pImg, wb, h, TopL, BtmL);
		else if ( ImgType == DIB_8BIT)
			bAllWhite = GrayAllWhite(pImg, wb, h, TopL, BtmL, 128);
		else if ( ImgType == DIB_24BIT)
			bAllWhite = ColorAllWhite(pImg, wb, h, TopL, BtmL, 1, 128);
		else
			return -1;
		if ( bAllWhite )			break;
		TopL.x--;
		BtmL.x--;
	}

	for ( i=0; i<MaxDx; i++)
	{
        if( TopR.x >= w-1 || BtmR.x >= w-1 || TopR.x <= 0 || BtmR.x <= 0 )
			break;
		if ( ImgType == DIB_1BIT )
			bAllWhite = AllWhite(pImg, wb, h, TopR, BtmR);
		else if ( ImgType == DIB_8BIT )
			bAllWhite = GrayAllWhite(pImg, wb, h, TopR, BtmR, 128);
		else if ( ImgType == DIB_24BIT )
			bAllWhite = ColorAllWhite(pImg, wb, h, TopR, BtmR, 1, 128);
		else	return -1;
		if ( bAllWhite )			break;
		TopR.x++;		
		BtmR.x++;
	}

	Image.UnlockRawImg ();
	return 0;
}

/*****************************************************************************
/*	Name:		ZoomY
/*	Funcation:	Zoom image in Y ordinate, written by Pan S.Y., not tested yet
/*
/*****************************************************************************/
void CImageTool::ZoomY ( DSCC_BYTE *ImgS,  DSCC_BYTE *ImgD, int h, int nh, int wInBytes )
{
        int SourceY, LastSrcY, i, j, k ;
        DSCC_BYTE *ThisLine, *tThisLine, *LastLine, *tImgS ;

        LastSrcY = -1 ;
        for ( i=0 ; i<nh ; i++ )
        {
                ThisLine = ImgD + i*wInBytes ;
                SourceY = (i+1)*(h-1)/nh ;
                if ( SourceY == LastSrcY )
                {
                        LastLine = ImgD + (i-1)*wInBytes ;
                        tThisLine = ThisLine ;
                        for ( j=0 ; j<wInBytes ; j++ )
                                *tThisLine++ = *LastLine++ ;
                }
                else
                {
                        tImgS = ImgS+wInBytes*(LastSrcY+1) ;
                        tThisLine = ThisLine ;
                        for ( j=0 ; j<wInBytes ; j++ )
                                *tThisLine++ = *tImgS++ ;
                        for ( k=LastSrcY+2 ; k<=SourceY ; k++ )
                        {
                                tImgS = ImgS+wInBytes*k ;
                                tThisLine = ThisLine ;
                                for ( j=0 ; j<wInBytes ; j++ )
                                        *tThisLine++ |= *tImgS++ ;
                        }
                }
                LastSrcY = SourceY ;
        }
}

/*****************************************************************************
/*	Name:		Zoom
/*	Funcation:	Zoom image, written by Pan S.Y., not tested yet
/*
/*****************************************************************************/
// int CImageTool::Zoom ( DSCC_BYTE *ImgS, DSCC_BYTE *ImgD, int wInBytes, int h, int nwInBytes, int nh )
// {
//         DSCC_BYTE *ImgY, *ImgX, *tImgD ;
// 
//         int w = wInBytes<<3 ;
//         int nw = nwInBytes<<3 ;
//         int ImgXwInBytes = (nh+7)>>3 ;
// 
//         ImgY = (DSCC_BYTE *)malloc (wInBytes*nh) ;
//         ImgX = (DSCC_BYTE *)malloc (ImgXwInBytes*w) ;
//         tImgD = (DSCC_BYTE *)malloc (ImgXwInBytes*nw) ;
// 
//         if ( ImgY==NULL || ImgX==NULL || tImgD==NULL )
//                 return -1 ;
//         ZoomY ( ImgS, ImgY, h, nh, wInBytes ) ;
// // Rotate
//         Rotate ( ImgY, 0, w-1, wInBytes, nh, ImgX, 0, ImgXwInBytes) ;
//         ZoomY ( ImgX, tImgD, w, nw, ImgXwInBytes ) ;
// // Rotate Back
//         Rotate ( tImgD, 0, nh-1, ImgXwInBytes, nw, ImgD, 0, nwInBytes ) ;
//         free ( tImgD ) ;
//         free ( ImgX ) ;
//         free ( ImgY ) ;
//         return 0 ;
// }

/*****************************************************************************
/*	Name:		GetBlackPixelRatio
/*	Funcation:	Get black pixel ratio in a image block.
/*
/*****************************************************************************/
// double CImageTool::GetBlackPixelRatio ( LPBYTE FAR *lpLine,  RECT rcBound )
// {//It is not optimal, should use lut, lz, rz defined in Tools.cpp
//     int i, j, k ;
//     BYTE mask[8] = {128, 64, 32, 16, 8, 4, 2, 1} ;
// 
//     int TotalBlackPixels = 0 ;
//     for ( i=0 ; i<rcBound.bottom-rcBound.top ; i++ )
//     {
//         BYTE *p = lpLine[i]+ rcBound.left/8 ;
//         j = rcBound.left % 8 ;
//         for ( k=rcBound.left ; k<rcBound.right ; k++ )
//         {
//             if ( *p&mask[j++] )
//                 TotalBlackPixels++ ;
//             if ( j == 8 )
//                 p++, j=0 ;
//         }
//     }
//     return (double)TotalBlackPixels / ( (double)(rcBound.right-rcBound.left+1)*(double)(rcBound.bottom-rcBound.top+1) ) ;
// }

/*****************************************************************************
/*	Name:		GetBlackPixelRatio
/*	Funcation:	Get black pixel ratio in a image block.
/*
/*****************************************************************************/
double CImageTool::GetBlackPixelRatio ( CImage &Img,  DSCC_RECT rcBound )
{//It is not optimal, should use lut, lz, rz defined in Tools.cpp
    int i, j;
    DSCC_BYTE mask[8] = {128, 64, 32, 16, 8, 4, 2, 1} ;

	int	w = Img.GetWidth();
	int h = Img.GetHeight();
	int	wb = Img.GetLineSizeInBytes();
	if( rcBound.right == 0 || rcBound.bottom == 0 )
	{
		rcBound.left = 0;
		rcBound.right = w-1;
		rcBound.top = 0;
		rcBound.bottom = h-1;
	}

	DSCC_BYTE	*pImg = Img.LockRawImg();
    int TotalBlackPixels = 0 ;
    for ( i=rcBound.top ; i<=rcBound.bottom ; i++ )
    {
        DSCC_BYTE *p = pImg + (h-1-i)*wb + rcBound.left/8 ;
		if( (rcBound.left+7)/8 == (rcBound.right+7)/8 )
			TotalBlackPixels += lut[ (*p) & lz[rcBound.left%8] & rz[rcBound.right%8] ];
		else
		{
			if( rcBound.left%8 != 0 )
			{
				TotalBlackPixels += lut[ (*p) & lz[rcBound.left%8] ];
				p++;
			}
			for ( j=(rcBound.left+7)/8 ; j<=rcBound.right/8 ; j++ )
			{
				TotalBlackPixels += lut[ *p ];
				p++;
			}
			if( rcBound.right%8 != 0 )
				TotalBlackPixels += lut[ (*p) & rz[rcBound.right%8] ];
		}
    }
	Img.UnlockRawImg();
    return (double)TotalBlackPixels / ( (double)(rcBound.right-rcBound.left+1)*(double)(rcBound.bottom-rcBound.top+1) ) ;
}
/*****************************************************************************
/*	Name:		GetBlackPixelNum
/*	Funcation:	Get the number of black pixels in an image block.
/*	Parameter:	p -- Pointer to the image data, DIB format
/*				wb -- Line size in bytes
/*				hh -- Image height
/*				left -- Left ordinate of the image block
/*				top -- Top ordinate of the image block
/*				w -- Width of the image block
/*				h -- Height of the image block
/*	Return:		Number of black pixels in an image block	
/*
/*****************************************************************************/
int CImageTool::GetBlackPixelNum ( DSCC_BYTE *p, int wb, int hh, int left, int top, int w, int h )
{//It is not optimal, should use lut, lz, rz defined in Tools.cpp
    DSCC_BYTE mask[8] = {128, 64, 32, 16, 8, 4, 2, 1 } ;
    int sum = 0 ;

    for ( int i=0 ; i<h ; i++ )
    {
        DSCC_BYTE *tp = p+(hh-1-top-i)*wb + left/8 ;
        int k = left%8 ;
        for ( int j=0 ; j<w ; j++ )
        {
            if ( *tp & mask[k++] ) sum++ ;
            if ( k == 8 ) tp++, k=0 ;
        }
    }
    return sum ;
}

/*****************************************************************************
/*	Name:		PixelsOnLine
/*	Function:	Calculate the number of black pixels on a line
/*	Parameter:	Image  -- Input image
/*				xb     -- X coordinate of start point
/*				yb     -- Y coordinate of start point
/*				xe     -- X coordinate of end point
/*				ye     -- Y coordinate of end point
/*	Return:		Number of black pixels on a line
/****************************************************************************/
int CImageTool::PixelsOnLine(CImage&  Image, int xb, int yb, int xe, int ye)
{
	//extern unsigned char	lz[8], rz[8];
	//extern unsigned char	lut[256];
	int		j, step,x,y, lbyte, rbyte, lbit, rbit;
	int		proj;
	int	   w = Image.GetWidth();
	int	   wb = Image.GetLineSizeInBytes(); 
	int	   h = Image.GetHeight();
	DSCC_BYTE	byte;
	if( lut[1] == 0 )	BlackPixelsInByte();
	
	if(xb > xe)
	{
		int tmp = xb;		xb = xe;		xe = tmp;
		tmp = yb;			yb = ye;		ye = tmp;
	}
	if(xb<0 || xe>=w)					return -1;
//	if(xe-xb < 5*abs(yb-ye) || xe<=xb)	return -1;
	if( xe<=xb )	return -1;
	if(ye == yb)		step = xe-xb;
	else				step = max(1, (xe-xb)/abs(ye-yb));

	DSCC_BYTE*  pImage = Image.LockRawImg();
	int   bUpSideDown = Image.GetOrientation();

	proj = 0;
	y = yb;
	if(y<2 || y>h-3)	return -1;

	for(x=xb;x<xe+step; x+=step)
	{	
		if(x != xb && (x-xb)%step == 0)
		{
			if(ye > yb)		y++;
			else			y--;
			if(y<=1 || y>=h-2)		break;
		}
		lbyte = x/8;		rbyte = min(xe, x+step)/8;
		lbit = x%8;			rbit =  min(xe, x+step)%8;
		if(lbyte == rbyte)
		{
			if( bUpSideDown )
				byte = *(pImage+(h-1-y)*wb+lbyte);
			else
				byte = *(pImage+y*wb+lbyte);
			proj += lut[byte&lz[lbit]&rz[rbit]];
		}
		else
		{
			if( bUpSideDown )
				byte = *(pImage+(h-1-y)*wb + lbyte);
			else
				byte = *(pImage+y*wb + lbyte);
			proj += lut[byte&lz[lbit]]; 
			for(j=lbyte+1; j<rbyte; j++)
			{
				if( bUpSideDown )
					byte = *(pImage+(h-1-y)*wb+j);
				else
					byte = *(pImage+y*wb+j);
				proj += lut[byte];
			}
			if( bUpSideDown )
				byte = *(pImage+(h-1-y)*wb+rbyte);
			else
				byte = *(pImage+y*wb+rbyte);
			proj += lut[byte&rz[rbit]];
		}			
	}

	Image.UnlockRawImg(); 
	return  proj;
}

/***********************************************************************
/*	Name:		DoHorProject
/*	Fucntion:	Get horizontal projection
/*	Parameter:	Img -- Image
/*				Proj -- Horizontal projection
/*				nProj -- Projection number
/*				rc -- Interested area
/*	Return:		0  -- Correct
/*				-1 -- Error
/***********************************************************************/
int	CImageTool::DoHorProject(CImage &Img, double *&Proj, int &nProj, DSCC_CRect rc )
{
	if( rc.bottom == 0 || rc.right == 0 )
	{
		rc.left = 0;
		rc.top = 0;
		rc.right = Img.GetWidth()-1;
		rc.bottom = Img.GetHeight()-1;
	}
	nProj = rc.bottom - rc.top + 1;
	Proj = (double*)malloc( sizeof(double)*nProj );
	memset( Proj,0, sizeof(double)*nProj );
	
	DSCC_BYTE *pImg = Img.LockRawImg();
	int	 wb = Img.GetLineSizeInBytes();
	int	 h = Img.GetHeight();
	DSCC_BYTE *lut = Img.GetBlackPixelsInByteMask();
	int i, j;
	DSCC_BYTE *pByte;
	for( i=rc.top; i<=rc.bottom; i++ )
	{
		if( Img.GetOrientation() )
			pByte = pImg+(h-1-i)*wb+rc.left/8;
		else
			pByte = pImg+i*wb+rc.left/8;

		if( rc.left%8 != 0 )
		{
			Proj[i-rc.top] += lut[(*pByte) & lz[rc.left%8]];
			pByte++;
		}
		for( j=(rc.left+7)/8; j<rc.right/8; j++ )
		{
			Proj[i-rc.top] += lut[*pByte];
			pByte ++;
		}
		if( rc.right%8 != 0 )
			Proj[i-rc.top] += lut[(*pByte) & rz[rc.right%8]];
	}
	Img.UnlockRawImg();
	return 0;
}

/***********************************************************************
/*	Name:		DoSkewHorProject
/*	Fucntion:	Get skewed horizontal projection
/*	Parameter:	Img -- Image
/*				Proj -- Horizontal projection
/*				nProj -- Projection number
/*	Return:		0  -- Correct
/*				-1 -- Error
/***********************************************************************/
int CImageTool::DoSkewHorProject( CImage &Img, double nSkewAngle, double *&Proj, int &nProj  )
{
	int w = Img.GetWidth( );
	int h = Img.GetHeight( );
	int	dh = (int)( w*tan(nSkewAngle) );
	Proj = (double*)malloc( sizeof(double)*(h+abs(dh)+10) );
	if( Proj == NULL )
	{
		printf( "memory used up!\n" );
		return -1;
	}

	int nLeftStart, nRightStart, nLeftEnd, nRightEnd;
	if( nSkewAngle >= 0 )
	{
		nLeftStart = 1;
		nRightStart = -dh+1;
	}
	else
	{
		nLeftStart = dh+1;
		nRightStart = 1;
	}
	nLeftEnd = abs(dh) + h + nLeftStart;
	nRightEnd = abs(dh) + h + nRightStart;

	int	xs, ys, xe, ye;
	DSCC_CPoint	CrossPnt;
	for( nProj=0; nLeftStart<nLeftEnd; nLeftStart++, nRightStart++, nProj++)
	{
		if( nLeftStart < 0 || nRightStart < 0 )
		{
			GetCrossPoint( 0, nLeftStart, w-1, nRightStart, 0, 0, w-1, 0, CrossPnt );
			if( nLeftStart < 0 )
			{
				xs = CrossPnt.x;
				ys = CrossPnt.y;
				xe = w-1;
				ye = nRightStart;
			}
			else
			{
				xs = 0;
				ys = nLeftStart;
				xe = CrossPnt.x;
				ye = CrossPnt.y;
			}
		}
		else if( nLeftStart >= h || nRightStart >= h )
		{
			GetCrossPoint( 0, nLeftStart, w-1, nRightStart, 0, h-1, w-1, h-1, CrossPnt );
			if( nLeftStart >= h )
			{
				xs = CrossPnt.x;
				ys = CrossPnt.y;
				xe = w-1;
				ye = nRightStart;
			}
			else
			{
				xs = 0;
				ys = nLeftStart;
				xe = CrossPnt.x;
				ye = CrossPnt.y;
			}
		}
		else
		{
			xs = 0;
			ys = nLeftStart;
			xe = w-1;
			ye = nRightStart;
		}
		Proj[nProj] = CImageTool::PixelsOnHorLine( Img, xs, ys, xe, ye );
	}
	return 0;
}

/***********************************************************************
/*	Name:		DoVerProject
/*	Fucntion:	Get vertical projection
/*	Parameter:	Img -- Image
/*				Proj -- Vertical projection
/*				nProj -- Projection number
/*				rc -- Interested area
/*	Return:		0  -- Correct
/*				-1 -- Error
/***********************************************************************/
int	CImageTool::DoVerProject(CImage &Img, double *&Proj, int &nProj, DSCC_CRect rc )
{
	DSCC_BYTE mask[]={128,64,32,16,8,4,2,1};
	if( rc.bottom == 0 || rc.right == 0 )
	{
		rc.left = 0;
		rc.top = 0;
		rc.right = Img.GetWidth()-1;
		rc.bottom = Img.GetHeight()-1;
	}
	nProj = rc.right - rc.left + 1;
	Proj = (double*)malloc( sizeof(double)*nProj );
	memset( Proj,0, sizeof(double)*nProj );
	
	DSCC_BYTE *pImg = Img.LockRawImg();
	int	 wb = Img.GetLineSizeInBytes();
	int	 h = Img.GetHeight();
	int i, j;
	DSCC_BYTE *pByte;
	for( i=rc.top; i<=rc.bottom; i++ )
	{
		if( Img.GetOrientation() )
			pByte = pImg+(h-1-i)*wb+rc.left/8;
		else
			pByte = pImg+i*wb+rc.left/8;

		for( j=rc.left; j<=rc.right; j++ )
		{
			if(  ( (*pByte) & mask[j%8] ) )
				Proj[j-rc.left]++ ;
			if( j%8 == 7 )
				pByte++;
		}
	}
	Img.UnlockRawImg();
	return 0;
}

/*****************************************************************************
/*	Name:		PixelsOnHorLine
/*	Function:	Calculate the number of black pixels on a line
/*	Parameter:	Image  -- Input image
/*				xb     -- X coordinate of start point
/*				yb     -- Y coordinate of start point
/*				xe     -- X coordinate of end point
/*				ye     -- Y coordinate of end point
/*	Return:		Number of black pixels on a line
/****************************************************************************/
int CImageTool::PixelsOnHorLine(CImage&  Image, int xb, int yb, int xe, int ye)
{
//	extern unsigned char	lz[8], rz[8];
//	extern unsigned char	lut[256];
	int		xl, xr, y, lbyte, lbit, rbyte, rbit;
	DSCC_BYTE	ImgByte;
	int		proj;
	int	    w = Image.GetWidth();
	int	    wb = Image.GetLineSizeInBytes(); 
	int	    h = Image.GetHeight();
	if( lut[1] == 0 )	BlackPixelsInByte();
	
	if(xb > xe)
	{
		int tmp = xb;		xb = xe;		xe = tmp;
		tmp = yb;			yb = ye;		ye = tmp;
	}
	if(xb<0 || xe>=w)					return -1;
	if(yb<0 || ye>=h)					return -1;

	DSCC_BYTE*  pImage = Image.LockRawImg();
	int   bUpSideDown = Image.GetOrientation();

	proj = 0;
	double	step;
	if( ye == yb )		step = xe-xb;
	else				step = fabs( 1.0*(xe-xb)/(ye-yb) );
	for( y=min(yb, ye); y<=max(yb, ye); y++)
	{	
		xl = (int)( xb + 1.0*(y-yb)*(xe-xb)/(ye-yb) - step/2 );
		xr = (int)( xl + step - 1);
		xl = max(xl, xb );
		xr = min(xr, xe );

		lbyte = xl/8;
		lbit = xl%8;
		rbyte = xr/8;
		rbit =  xr%8;
		if(lbyte == rbyte)
		{
			if( bUpSideDown )
				ImgByte = *(pImage+(h-1-y)*wb+lbyte);
			else
				ImgByte = *(pImage+y*wb+lbyte);
			proj += lut[ImgByte&lz[lbit]&rz[rbit]];
		}
		else
		{
			if( bUpSideDown )
				ImgByte = *(pImage+(h-1-y)*wb + lbyte);
			else
				ImgByte = *(pImage+y*wb + lbyte);
			proj += lut[ImgByte&lz[lbit]]; 
			for(int	j=lbyte+1; j<rbyte; j++)
			{
				if( bUpSideDown )
					ImgByte = *(pImage+(h-1-y)*wb+j);
				else
					ImgByte = *(pImage+y*wb+j);
				proj += lut[ImgByte];
			}
			if( bUpSideDown )
				ImgByte = *(pImage+(h-1-y)*wb+rbyte);
			else
				ImgByte = *(pImage+y*wb+rbyte);
			proj += lut[ImgByte&rz[rbit]];
		}			
	}

	Image.UnlockRawImg(); 
	return  proj;

}

/***********************************************************************
/*	Name:		SmoothProject
/*	Fucntion:	Smooth projection
/*	Parameter:	Proj -- Projecton
/*				nProj -- Projection number
/*	Return:		0  -- Correct
/*				-1 -- Error
/***********************************************************************/
int	CImageTool::SmoothProject(double *Proj, int nProj)
{
	double	*Proj2 = (double*)malloc(sizeof(double)*nProj);
	memcpy( Proj2, Proj, sizeof(double)*nProj );
	for( int i=1; i<nProj-1; i++ )
		Proj[i] = floor( (Proj2[i-1]+Proj2[i]+Proj2[i+1])/3 );
	free( Proj2 );
	return 0;
}

/***********************************************************************
/*	Name:		DumpProject
/*	Fucntion:	Dump projection to an image file for debugging
/*	Parameter:	snFileName -- Image file name
/*				prj	-- Projection
/*				nProj -- Projection number
/*				nMaxProj -- Maximum projection
/*	Return:		0  -- Correct
/*				-1 -- Error
/***********************************************************************/
// int CImageTool::DumpProject(char* snFileName, double *prj, int nProj,  int bHor, int nMaxProj )
// {
//     int i, w, ImgSize;
// 	if( nMaxProj == 0 )
// 	{
// 		for( i=0; i<nProj; i++ )
// 			nMaxProj = max( nMaxProj, prj[i] );
// 	}
// 
//     CBitmap bmp, *pOldBMP;
// 	if( bHor )
// 	{
// 	    w = (nMaxProj+31)/32*32 ;
// 	    ImgSize = w/8*nProj ;
// 		if( !bmp.CreateBitmap ( w, nProj, 1, 1, NULL ) )
// 			return -1;
// 	}
// 	else
//     {
// 		w = (nProj+31)/32*32;
// 		ImgSize = w/8*nMaxProj;
// 		if( !bmp.CreateBitmap ( w, nMaxProj, 1, 1, NULL ) )
// 			return -1;
// 	}
//   
// 	CDC dc ;
//     dc.CreateCompatibleDC ( &CClientDC( AfxGetMainWnd() ) ) ;
// 
//     CPen  *pOldPen, *pPen;
// 
//     if ( (pOldBMP=dc.SelectObject(&bmp)) != NULL )
//     {
//         BYTE *p = (BYTE *)malloc(ImgSize) ;
//         if ( p )
//         {
//             memset ( p, 255, ImgSize ) ;
//             bmp.SetBitmapBits( ImgSize, p ) ;
//             free ( p ) ;
// 			pPen = new CPen(PS_SOLID, 1, RGB(0,0,0));
// 			pOldPen = dc.SelectObject ( pPen ) ;
// 			for(i=0; i<nProj; i++)
// 			{
// 				if( prj[i] <= 0 )	continue;
// 				if( bHor )
// 				{
// 					dc.MoveTo(0, i);
// 					dc.LineTo(prj[i],i);
// 				}
// 				else
// 				{
// 					dc.MoveTo(i, nMaxProj-prj[i] );
// 					dc.LineTo(i, nMaxProj );
// 				}
// 			}
// 			dc.SelectObject ( pOldPen ) ;
// 			delete pPen ;
// 			CImage DIB ;
//             if ( DIB.Initialize(dc.GetSafeHdc(),bmp) )
//             {
//                 dc.SelectObject ( pOldBMP ) ;
//                 DIB.Write (snFileName) ;
//             }
//         }
//     }
//     return 0 ;
// }

/******************************************************************************
/*	Name:		GetRowRunLengthBit
/*	Function:	Extract row runlength
/*	Parameter:	Line -- Image data, 1 pixel per bit
/*				w -- Image width
/*				h -- Image height
/*				wb -- Image width in byte
/*				row -- Image row
/*				RunLength -- Runlength
/*				nRunLength -- Number of runlength
/*				bBlackConn -- TRUE   Black pixels connected component
/*							  FALSE  White pixels connected component
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int	CImageTool::GetRowRunLengthBit( DSCC_BYTE *Line, int w, int h, int wb, int row, DSCC_POINT *RunLength, int &nRunLength, int bBlackConn )
{
	int i, j;
    int bInValley ;

    DSCC_BYTE mask[8] = {128,64,32,16,8,4,2,1} ;
    DSCC_BYTE tmp ;
	DSCC_BYTE *p = Line + wb*row ;

	nRunLength=0 ;
    for ( i=0, j=0, bInValley=FALSE, tmp=*p++ ; i<w ; i++ )
    {
        if ( ( bBlackConn && (tmp & mask[j]) ) ||
			 ( !bBlackConn && !(tmp &mask[j]) ) )
        {
            if ( !bInValley )
            {
                RunLength[nRunLength].x = i;
                bInValley = TRUE ;
            }
        }
        else
        {
            if ( bInValley )
            {
				RunLength[nRunLength].y = i-1;
				nRunLength ++;
                bInValley = FALSE ;
            }
        }
        j++ ;
        if ( j == 8 )
        {
            tmp= *p++ ;
            j = 0 ;
        }
    }
    if ( bInValley )
	{	
		RunLength[nRunLength].y = i-1;
		nRunLength++;
	}
    return 0 ;
}

/******************************************************************************
/*	Name:		GetRowRunLength
/*	Function:	Extract row runlength
/*	Parameter:	Line -- Image data, 1 pixel per byte
/*				w -- Image width
/*				h -- Image height
/*				row -- Image row
/*				RunLength -- Runlength
/*				nRunLength -- Number of runlength
/*				bBlackConn -- TRUE   Black pixels connected component
/*							  FALSE  White pixels connected component
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int	CImageTool::GetRowRunLength( DSCC_BYTE *Line, int w, int h, int row, DSCC_POINT *RunLength, int &nRunLength, int bBlackConn )
{
	int	Left, Right;
	nRunLength = 0;
	Left=0; 
	while( Left < w )
	{
		if( bBlackConn )
			while( Left<w && Line[row*w+Left] == 0)	Left++;
		else
			while( Left<w && Line[row*w+Left] == 1)	Left++;

		Right = Left;
		if( Left >= w )		break;
		if( bBlackConn )
			while( Right<w && Line[row*w+Right] == 1 ) Right++;
		else
			while( Right<w && Line[row*w+Right] == 0 ) Right++;
		RunLength[nRunLength].x = Left;
		RunLength[nRunLength].y = Right-1;
		nRunLength++;
		
		Left = Right;
	}
	return 0;
}

/******************************************************************************
/*	Name:		ExtractConnComp
/*	Function:	Extract connected components
/*	Parameter:	Line -- Image data, 1 pixel per byte or 1 pixel per bit
/*				w -- Image width
/*				h -- Image height
/*				pConnComp -- Bounding box of connected component
/*				nConnPixels -- Pixels number of connected component
/*				nConnComp -- Number of connected component
/*				bBlackConn -- TRUE   Black pixels connected component
/*							  FALSE  White pixels connected component
/*				Connectness --  4, 4-connected; 8, 8-connected
/*				bByte --  TRUE -- 1 pixel per byte
/*						  FALSE -- 	1 pixel per bit
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int	CImageTool::ExtractConnComp( DSCC_BYTE *pImg, int w, int h, DSCC_RECT *&pConnComp, int *&nConnPixels, int &nConnComp, int bBlackConn, int Connectness, int bByte )
{
	if( Connectness != 4 )
		Connectness = 8;

	int		MAXCONNCOMP = 100;
	pConnComp = (DSCC_RECT*)malloc( sizeof(DSCC_RECT)*MAXCONNCOMP );
	nConnPixels = (int*)malloc( sizeof(int)*MAXCONNCOMP );
	if( pConnComp == NULL || nConnPixels == NULL )
		return -1;
	nConnComp = 0;

	int		i, j, k;
	DSCC_POINT	*RunLength = (DSCC_POINT*)malloc(sizeof(DSCC_POINT)*w );
	DSCC_POINT	*LastRunLength = (DSCC_POINT*)malloc(sizeof(DSCC_POINT)*w );
	int		*nConnNO = (int*)malloc( sizeof(int)*w );
	int		*nLastConnNO = (int*)malloc( sizeof(int)*w );
	if( RunLength == NULL || LastRunLength == NULL || nConnNO == NULL || nLastConnNO == NULL )
		return -1;

	int		nRunLength=0, nLastRunLength=0;
	int		MaxLeft, MinRight;
	int		wb = (w+7)/8;
	for( i=0; i<h; i++ )
	{
		//Get runlengths
		if( bByte )
			GetRowRunLength( pImg, w, h, i, RunLength, nRunLength, bBlackConn );
		else
			GetRowRunLengthBit( pImg, w, h, wb, i, RunLength, nRunLength, bBlackConn );

		//Merge runlength to connected components
		for( j=0; j<nRunLength; j++ )
		{
			nConnNO[j] = -1;
			for( k=0; k<nLastRunLength; k++ )
			{
				MaxLeft = max( RunLength[j].x, LastRunLength[k].x );
				MinRight = min( RunLength[j].y, LastRunLength[k].y );
				if( Connectness == 4 && MaxLeft > MinRight )
					continue;
				else if( Connectness == 8 && MaxLeft > MinRight+1 )
					continue;
				if( nConnNO[j] == -1 )
				{//Merge runlength to connected component
					pConnComp[nLastConnNO[k]].left = min( pConnComp[nLastConnNO[k]].left, RunLength[j].x );
					pConnComp[nLastConnNO[k]].right = max( pConnComp[nLastConnNO[k]].right, RunLength[j].y );
					pConnComp[nLastConnNO[k]].bottom = i;
					nConnPixels[nLastConnNO[k]] += RunLength[j].y-RunLength[j].x+1;
					nConnNO[j] = nLastConnNO[k];
				}
				else
				{
					if( nConnNO[j] != nLastConnNO[k] )
					{//Mergt two connected componets
						pConnComp[nConnNO[j]].left = min( pConnComp[nConnNO[j]].left, pConnComp[nLastConnNO[k]].left );
						pConnComp[nConnNO[j]].right = max( pConnComp[nConnNO[j]].right, pConnComp[nLastConnNO[k]].right );
						pConnComp[nConnNO[j]].top = min( pConnComp[nConnNO[j]].top, pConnComp[nLastConnNO[k]].top );
						pConnComp[nConnNO[j]].bottom = i;
						nConnPixels[nConnNO[j]] += nConnPixels[nLastConnNO[k]];
						nConnPixels[nLastConnNO[k]] = -1;
						int		nOldConn = nLastConnNO[k];
						int		nNewConn = nConnNO[j];
						int m = 0;
						for( ; m<nLastRunLength; m++ )
						{
							if( nLastConnNO[m] == nOldConn )
								nLastConnNO[m] = nNewConn;
						}
						for( m=0; m<nRunLength; m++ )
						{
							if( nConnNO[m] == nOldConn )
								nConnNO[m] = nNewConn;
						}
					}
				}
			}
			if( nConnNO[j] == - 1)
			{
				pConnComp[nConnComp].top = i;
				pConnComp[nConnComp].bottom = i;
				pConnComp[nConnComp].left = RunLength[j].x;
				pConnComp[nConnComp].right = RunLength[j].y;
				nConnPixels[nConnComp] = RunLength[j].y-RunLength[j].x+1;
				nConnNO[j] = nConnComp;
				nConnComp++;
				if( nConnComp >= MAXCONNCOMP )
				{
					MAXCONNCOMP += 100 ;
					pConnComp = (DSCC_RECT*)realloc(pConnComp, sizeof(DSCC_RECT)*MAXCONNCOMP );
					nConnPixels = (int*)realloc(nConnPixels, sizeof(int)*MAXCONNCOMP );
				}
			}
		}
		//Update runlengths of the last line
		for( j=0; j<nRunLength; j++ )
		{
			LastRunLength[j] = RunLength[j];
			nLastConnNO[j] = nConnNO[j];
		}
		nLastRunLength = nRunLength;
	}

//Delete some dead connected components
	i=0;
	while( i<nConnComp )
	{
		if( nConnPixels[i] == -1 )
		{
			for( j=i; j<nConnComp-1; j++ )
			{
				pConnComp[j] = pConnComp[j+1];
				nConnPixels[j] = nConnPixels[j+1];
			}
			nConnComp--;
		}
		else
			i++;
	}
	free( RunLength );
	free( LastRunLength );
	free( nConnNO );
	free( nLastConnNO );
	return 0;
}

/******************************************************************************
/*	Name:		BitToByte
/*	Function:	Convert 1 bit/pixel to 1 byte/pixel
/*				Reserved for DIB 4 byte allignment
/*	Parameter:	BitImg -- Raw image
/*				w -- Real image width
/*				h -- Image height
/*				ByteImg -- Converted image
/*				BoundBox -- Boundbox of char image
/*	Return:		0  -- Succeed
/*				-1 -- Failed
/*****************************************************************************/
int	CImageTool::BitToByte( DSCC_BYTE* BitImg, int wb, int w, int h, DSCC_BYTE *ByteImg, int w2, int h2, DSCC_CRect &BoundBox )
{
	if( h2 < h )	return -1;
	BoundBox.left = w;		BoundBox.right = 0;
	BoundBox.top = h;		BoundBox.bottom = 0;
	int i, j, k, TotalPixels = 0;
	DSCC_BYTE	TempBitImg;
	memset( ByteImg,0, w2*h2*sizeof(DSCC_BYTE) );
	for(i=0; i<h; i++)
	{
		for( j=0; j<wb; j++)
		{
			TempBitImg = BitImg[i*wb+j];
			k=7; 
			while( k>=0 )
			{
				if( TempBitImg >= 1<<k && j*8+7-k < w)
				{
					ByteImg[(h-1-i)*w2+j*8+7-k] = 1;
					TempBitImg -= 1<<k;
					TotalPixels ++;
					BoundBox.left	= min(BoundBox.left, (long)(j * 8 + 7 - k));
					BoundBox.right = max(BoundBox.right, (long)(j * 8 + 7 - k));
					BoundBox.top = min(BoundBox.top, (long)(h - 1 - i));
					BoundBox.bottom = max(BoundBox.bottom, (long)(h - 1 - i));
				}
				k--;
			}
		}
	}
	return TotalPixels;
}

/******************************************************************************
/*	Name:		BitToByte
/*	Function:	Convert 1 bit/pixel to 1 byte/pixel
/*				Reserved for DIB 4 byte allignment
/*	Parameter:	BitImg -- Raw image
/*				w -- Real image width
/*				h -- Image height
/*				ByteImg -- Converted image
/*	Return:		0  -- Succeed
/*				-1 -- Failed
/*****************************************************************************/
int	CImageTool::BitToByte( DSCC_BYTE* BitImg, int wb, int w, int h, DSCC_BYTE *&ByteImg)
{
	int i, j, k;
	DSCC_BYTE	TempBitImg;
	ByteImg = (DSCC_BYTE*)malloc( sizeof(DSCC_BYTE)*w*h );
	memset( ByteImg,0, w*h*sizeof(DSCC_BYTE) );
	for(i=0; i<h; i++)
	{
		for( j=0; j<wb; j++)
		{
			TempBitImg = BitImg[i*wb+j];
			k=7; 
			while( k>=0 )
			{
				if( TempBitImg >= 1<<k && j*8+7-k < w)
				{
					ByteImg[(h-1-i)*w+j*8+7-k] = 1;
				}
				k--;
			}
		}
	}
	return 0;
}
/******************************************************************************
/*	Name:		ByteToBit
/*	Function:	Convert 1 byte/pixel to 1 bit/pixel
/*	Parameter:	Line -- 1 byte/pixel image
/*				w -- Image width
/*				h -- Image height
/*				RawByte -- Converted 1 bit/pixel image
/*	Return:		0  -- Succeed
/*				-1 -- Failed
/*****************************************************************************/
int	CImageTool::ByteToBit( DSCC_BYTE* pByteImg, int w, int h, DSCC_BYTE *RawByte, int wb, DSCC_BYTE BlackPixel )
{
	int i, j, k;
	if( wb == 0 )
		wb = (w+7)/8;
	memset( RawByte,0, wb*h*sizeof(DSCC_BYTE) );
	for(i=0; i<h; i++)
	{
		for( j=0; j<wb; j++)
		{
			for( k=8*j; k<min( 8*j+8, w ); k++ )
				if( pByteImg[i*w+k] == BlackPixel )
					RawByte[i*wb+j] += 1<<(7-k%8);
		}
	}
	return 0;
}

/******************************************************************************
/*	Name:		Dilate
/*	Function:	Dilate each pixel to four pixels. Used for line detecting on edge image
/*	Parameter:	Img  -- Image
/*				DImg -- Dilated image
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
// int CImageTool::Dialate ( CImage &Img, CImage &DImg )
// {
// 	if( Img.GetType() != DIB_1BIT )		return -1;
// 	int		w = Img.GetWidth();
// 	int		h = Img.GetHeight();
// 	int		wb = Img.GetLineSizeInBytes();
// 	BYTE	*pImg = Img.LockRawImg();
// 
// 	DImg.Initialize( w, h, 1, 1);
// 	BYTE	*pDImg = DImg.LockRawImg();
// 
// 	for( int i=0; i<h; i++ )
// 	{
// 		for( int j=0; j<wb; j++ )
// 		{
// 			BYTE	Block = *(pImg+i*wb+j);
// 			BYTE	DBlock = Block;
// 			//Dilate along horizontal line
// 			for( int k=7; k>=0; k-- )
// 			{
// 				if( (Block & (1<<k)) != 0 )
// 				{
// 					if( k != 7 )
// 						DBlock |= (1<<(k+1));
// 					if( k != 0 )
// 						DBlock |= (1<<(k-1));
// 				}
// 			}
// 			if( j > 0 )
// 			{
// 				BYTE  LeftBlock = *(pImg+i*wb+(j-1));
// 				if( ( LeftBlock & 1 ) != 0 )
// 					DBlock |= 128;
// 			}
// 			if( j < wb-1 )
// 			{
// 				BYTE RightBlock = *(pImg+i*wb+(j+1));
// 				if( RightBlock >= 128 )
// 					DBlock |= 1;
// 			}
// 			//Dilate along vertical line
// 			if( i > 0 )
// 			{
// 				BYTE TopBlock = *(pImg + (i-1)*wb+j);
// 				DBlock |= TopBlock ;
// 			}
// 			if( i < h-1 )
// 			{
// 				BYTE BottomBlock = *(pImg + (i+1)*wb+j );
// 				DBlock |= BottomBlock;
// 			}
// 			*(pDImg + i*wb + j) = DBlock;
// 		}
// 	}
// 
// 	Img.UnlockRawImg();
// 	DImg.UnlockRawImg();
// 	return 0;
// }
