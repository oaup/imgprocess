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
// File NAME:		ImageObj.cpp
// File Function:	Read and write image file
//
//				Developed by: Yefeng Zheng
//			University of Maryland, College Park
/////////////////////////////////////////////////////////////////////////

#include "ImageObj.h"
#include "ImageLib.h"


//IMPLEMENT_SERIAL(CImage, CObject, 0)

CImage::CImage()
{
	m_bUpSideDown = TRUE;
	m_pImg = NULL;
	m_nWidth = NULL;
	m_nHeight = NULL;
	m_nWidthByte = NULL;
//	m_hImg = NULL;
    Initialize() ;
	InitBlackPixelsInByte();
}

CImage::~CImage()
{
	FreeMem() ;
}

/************************************************************************************
/*	Name:		FreeMem
/*	Function:	Free memory
/*	Parameter:	none
/*	return:		0
/***********************************************************************************/
int CImage::FreeMem()
{
// 	if ( !m_bImported )
//     {
//         if ( m_hImg != (HGLOBAL)NULL )
// 		    GlobalFree(m_hImg) ;
//         m_hImg = (HGLOBAL)NULL ;
//     }

	if (m_pImg != NULL) {
		delete m_pImg;
		m_pImg = NULL;
	}
    return 0 ;
}

//////////////////////////////////////////////////////////////////////////////////////
//	Initialize image
//////////////////////////////////////////////////////////////////////////////////////

/************************************************************************************
/*	Name:		Initialize
/*	Function:	Initialize to void
/*	Parameter:	none
/*	return:		TRUE
/*
/***********************************************************************************/
int CImage::Initialize()
{
//    m_hImg = (HGLOBAL)NULL ;
//    m_bImported = FALSE ;
	m_bUpSideDown = TRUE;
	return TRUE;
}

/************************************************************************************
/*	Name:		Initialize
/*	Function:	Initialize image
/*	Parameter:	Image -- Another CImage instance
/*	return:		TRUE -- Succeed
/*				FALSE -- Failed
/*
/***********************************************************************************/
// int CImage::Initialize(CImage &Image)
// {
//     FreeMem() ;
// 
//     int nSize = GlobalSize ( Image.m_hImg ) ;
//     if ( nSize > 0 )
//     {
//         m_hImg = GlobalAlloc ( GMEM_MOVEABLE, nSize ) ;
//         if ( m_hImg != (HGLOBAL)NULL )
//         {
//             void *ps = GlobalLock ( Image.m_hImg ) ;
//             void *pd = GlobalLock ( m_hImg ) ;
//             memcpy ( pd, ps, nSize ) ;
//             GlobalUnlock ( m_hImg ) ;
//             GlobalUnlock ( Image.m_hImg ) ;
// 			if( NeedInvertBits() )
// 				InvertBits();
// 			if( NeedInvertPallete() )
// 				InvertPallete();
//             m_bImported = FALSE ;
//             return TRUE ;
//         }
//     }
//     return FALSE ;
// }

/************************************************************************************
/*	Name:		Initialize
/*	Function:	Initialize image, set m_bImported to TRUE, do not free the memroy
/*				during destructing.
/*	Parameter:	hDIBImg -- Image data memory blcok
/*	return:		TRUE -- Succeed
/*				FALSE -- Failed
/*
/***********************************************************************************/
// int CImage::Initialize(HGLOBAL hDIBImg)
// {
//     if ( hDIBImg != (HGLOBAL)NULL )
//     {
// 		FreeMem() ;
//         m_hImg = hDIBImg ;
// 		if( NeedInvertBits() )
// 			InvertBits();
// 		if( NeedInvertPallete() )
// 			InvertPallete();
//         m_bImported = TRUE ;
//         return TRUE ;
//     }
//     else
//         return FALSE ;
// }

/************************************************************************************
/*	Name:		Initialize
/*	Function:	Initialize image, not clear about this function
/*	Parameter:	hdc -- HDC handle
/*				bmp -- Bitmap
/*	return:		TRUE -- Succeed
/*				FALSE -- Failed
/*
/***********************************************************************************/
// int CImage::Initialize(HDC hdc, CBitmap& bmp)
// {
//     HGLOBAL hImg = GlobalAlloc ( GHND, sizeof(BITMAPINFO) ) ;
//     if ( hImg != (HGLOBAL)NULL )
//     {
//         LPBITMAPINFO lpbi = (LPBITMAPINFO)GlobalLock(hImg) ;
//         BITMAP bitmap ;
//         bmp.GetBitmap (&bitmap) ;
//         lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
//         lpbi->bmiHeader.biWidth = bitmap.bmWidth ;
//         lpbi->bmiHeader.biHeight = bitmap.bmHeight ;
//         lpbi->bmiHeader.biPlanes = bitmap.bmPlanes ;
//         lpbi->bmiHeader.biBitCount = bitmap.bmBitsPixel ;
//         lpbi->bmiHeader.biSizeImage = GetLineSizeInBytes(lpbi)*lpbi->bmiHeader.biHeight ;
//         hImg = GlobalReAlloc ( hImg, sizeof(BITMAPINFO) + GetPaletteSize(lpbi)+lpbi->bmiHeader.biSizeImage, GMEM_MOVEABLE ) ;
// 		GlobalUnlock(hImg) ;
//         if ( hImg != (HGLOBAL)NULL )
//         {
//             lpbi = (LPBITMAPINFO)GlobalLock(hImg) ;
//             SetPalette(lpbi) ;
// 			CDC*	pDC = CDC::FromHandle(hdc);
// 			int  num = 0;
// 			num = GetDIBits (hdc, *(pDC->GetCurrentBitmap()), 0, lpbi->bmiHeader.biHeight, GetRawImgPtr(lpbi), lpbi, DIB_RGB_COLORS ) ;
//             GlobalUnlock(hImg) ;
//           	FreeMem() ;
//             m_hImg = hImg ;
// 			if( NeedInvertBits() )
// 					InvertBits();
// 			if( NeedInvertPallete() )
// 					InvertPallete();
//             m_bImported = FALSE ;
//             return TRUE ;
//         }
//         else
//             return FALSE ;
//     }
//     else
//         return FALSE ;
// }

/************************************************************************************
/*	Name:		Initialize
/*	Function:	Initialize image, allocate memory but do not fill the image data
/*	Parameter:	nWidth -- Image width
/*				nHeight -- Image height
/*				nBitCount -- Bit used every pixel
/*				nPlanes -- Number of planes
/*	return:		TRUE -- Succeed
/*				FALSE -- Failed
/*
/***********************************************************************************/
// int CImage::Initialize ( int nWidth, int nHeight, int nBitCount, int nPlanes )
// {
//     HGLOBAL hImg = GlobalAlloc ( GHND, sizeof(BITMAPINFO) ) ;
//     if ( hImg != (HGLOBAL)NULL )
//     {
//         LPBITMAPINFO lpbi = (LPBITMAPINFO)GlobalLock(hImg) ;
//         lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
//         lpbi->bmiHeader.biWidth = nWidth ;
//         lpbi->bmiHeader.biHeight = nHeight ;
//         lpbi->bmiHeader.biPlanes = nPlanes ;
//         lpbi->bmiHeader.biBitCount = nBitCount ;
//         lpbi->bmiHeader.biSizeImage = GetLineSizeInBytes(lpbi)*lpbi->bmiHeader.biHeight ;
//         hImg = GlobalReAlloc ( hImg, sizeof(BITMAPINFO) + GetPaletteSize(lpbi)+lpbi->bmiHeader.biSizeImage, GMEM_MOVEABLE ) ;
//         GlobalUnlock(hImg) ;
//         if ( hImg != (HGLOBAL)NULL )
//         {
//             lpbi = (LPBITMAPINFO)GlobalLock(hImg) ;
//             SetPalette(lpbi) ;
//             memset ( GetRawImgPtr(lpbi), 0, lpbi->bmiHeader.biSizeImage ) ;
//             GlobalUnlock(hImg) ;
//           	FreeMem() ;
//             m_hImg = hImg ;
//             m_bImported = FALSE ;
//             return TRUE ;
//         }
//         else
//             return FALSE ;
//     }
//     else
//         return FALSE ;
// }

/************************************************************************************
/*	Name:		Initialize
/*	Function:	Initialize image
/*	Parameter:	lpLine -- Line pointer to image lines
/*				nWidth -- Image width
/*				nHeight -- Image height
/*				nImgType -- Image type
/*	return:		TRUE -- Succeed
/*				FALSE -- Failed
/*
/***********************************************************************************/
// int CImage::Initialize (unsigned char ** lpLine, int nWidth, int nHeight, int nImgType)
// {
// 	int i, ww ;
// 	HGLOBAL hDIBImg ;
// 	LPBITMAPINFO lpDIBImg ;
// 	BYTE *lpTmp ;
// 
// 	if ( lpLine == NULL)		return FALSE; 
// 	if ( nImgType != DIB_1BIT)	return FALSE;
// 
// 	ww = (nWidth+31)/32*32 ;
// 	hDIBImg = GlobalAlloc ( GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER)+2*sizeof(RGBQUAD)+ww/8*nHeight+16 ) ;
// 	if(hDIBImg != (HGLOBAL)NULL )
// 	{
//         lpDIBImg = (LPBITMAPINFO)GlobalLock( hDIBImg ) ;
//         if ( lpDIBImg != (LPBITMAPINFO)NULL )
//         {
//             lpDIBImg->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
//             lpDIBImg->bmiHeader.biWidth = nWidth ;
//             lpDIBImg->bmiHeader.biHeight = nHeight ;
//             lpDIBImg->bmiHeader.biPlanes = 1 ;
//             lpDIBImg->bmiHeader.biBitCount = 1 ;
//             lpDIBImg->bmiHeader.biCompression = BI_RGB ;
//             lpDIBImg->bmiHeader.biSizeImage = ww/8*nHeight ;
//             lpDIBImg->bmiHeader.biClrUsed = 0 ;
//             lpDIBImg->bmiHeader.biClrImportant = 0 ;
//             SetPalette ( lpDIBImg ) ;
//             lpTmp = (BYTE *)GetRawImgPtr( lpDIBImg ) ;
//             for ( i=0 ; i<nHeight ; i++ )
//                 memcpy ( lpTmp+(nHeight-1-i)*ww/8, lpLine[i], (nWidth+7)/8 ) ;
//             GlobalUnlock(hDIBImg) ;
//           	FreeMem() ;
//             m_hImg = hDIBImg ;
//             m_bImported = FALSE ;
//             return TRUE ;
//         }
//         else
//             GlobalFree ( hDIBImg ) ;
//     }
// 	return FALSE;
// }

/************************************************************************************
/*	Name:		Initialize
/*	Function:	Initialize image
/*	Parameter:	lpRawImg -- Pointer to DIB image data
/*				nWidth -- Image width
/*				nHeight -- Image height
/*				nImgType -- Image type
/*				bUpsideDown --  TRUE	---	the original image is upside down
/*								FALSE	--- the original image is right up
/*	return:		TRUE -- Succeed
/*				FALSE -- Failed
/***********************************************************************************/
// int CImage::Initialize ( BYTE *lpRawImg, int nWidth, int nHeight, int nImgType, int bUpsideDown )
// {
//   int i, j, ww ;
//   HGLOBAL hDIBImg ;
//   LPBITMAPINFO lpDIBImg ;
//   BYTE *lpTmp ;
// 
//   if ( lpRawImg )
//   {
//     if ( nImgType == DIB_8BIT )
//     {
//         ww = (nWidth+3)/4*4 ;
//         hDIBImg = GlobalAlloc ( GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)+ww*nHeight+16 ) ;
//         if ( hDIBImg != (HGLOBAL)NULL )
//         {
//             lpDIBImg = (LPBITMAPINFO)GlobalLock( hDIBImg ) ;
//             if ( lpDIBImg != (LPBITMAPINFO)NULL )
//             {
//                 lpDIBImg->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
//                 lpDIBImg->bmiHeader.biWidth = nWidth ;
//                 lpDIBImg->bmiHeader.biHeight = nHeight ;
//                 lpDIBImg->bmiHeader.biPlanes = 1 ;
//                 lpDIBImg->bmiHeader.biBitCount = 8 ;
//                 lpDIBImg->bmiHeader.biCompression = BI_RGB ;
//                 lpDIBImg->bmiHeader.biSizeImage = ww * nHeight ;
//                 lpDIBImg->bmiHeader.biClrUsed = 0 ;
//                 lpDIBImg->bmiHeader.biClrImportant = 0 ;
//                 SetPalette ( lpDIBImg ) ;
//                 lpTmp = (BYTE *)GetRawImgPtr( lpDIBImg ) ;
//                 for ( i=0 ; i<nHeight ; i++ )
// 				{
// 					if( !bUpsideDown )
// 						memcpy ( lpTmp+(nHeight-1-i)*ww, lpRawImg+nWidth*i, nWidth ) ;
// 					else
// 						memcpy ( lpTmp+i*ww, lpRawImg+nWidth*i, nWidth ) ;
// 				}
//                 GlobalUnlock(hDIBImg) ;
//           	    FreeMem() ;
//                 m_hImg = hDIBImg ;
//                 m_bImported = FALSE ;
//                 return TRUE ;
//             }
//             else
//                 GlobalFree ( hDIBImg ) ;
//         }
//     }
//     else if ( nImgType == DIB_1BIT )
//     {
//         ww = (nWidth+31)/32*32 ;
// 		hDIBImg = GlobalAlloc ( GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER)+2*sizeof(RGBQUAD)+ww/8*nHeight+16 ) ;
//         if ( hDIBImg != (HGLOBAL)NULL )
//         {
//             lpDIBImg = (LPBITMAPINFO)GlobalLock( hDIBImg ) ;
//             if ( lpDIBImg != (LPBITMAPINFO)NULL )
//             {
//                 lpDIBImg->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
//                 lpDIBImg->bmiHeader.biWidth = nWidth ;
//                 lpDIBImg->bmiHeader.biHeight = nHeight ;
//                 lpDIBImg->bmiHeader.biPlanes = 1 ;
//                 lpDIBImg->bmiHeader.biBitCount = 1 ;
//                 lpDIBImg->bmiHeader.biCompression = BI_RGB ;
//                 lpDIBImg->bmiHeader.biSizeImage = ww/8*nHeight ;
//                 lpDIBImg->bmiHeader.biClrUsed = 0 ;
//                 lpDIBImg->bmiHeader.biClrImportant = 0 ;
//                 SetPalette ( lpDIBImg ) ;
//                 lpTmp = (BYTE *)GetRawImgPtr( lpDIBImg ) ;
//                 for ( i=0 ; i<nHeight ; i++ )
// 				{
// 					if( !bUpsideDown )
// 						memcpy ( lpTmp+(nHeight-1-i)*ww/8, lpRawImg+(nWidth+7)/8*i, (nWidth+7)/8 ) ;
// 					else
// 						memcpy ( lpTmp+i*ww/8, lpRawImg+(nWidth+7)/8*i, (nWidth+7)/8 ) ;
// 				}
//                 GlobalUnlock(hDIBImg) ;
//           	    FreeMem() ;
//                 m_hImg = hDIBImg ;
//                 m_bImported = FALSE ;
//                 return TRUE ;
//             }
//             else
//                 GlobalFree ( hDIBImg ) ;
//         }
//     }
//     else if ( nImgType == DIB_24BIT )
//     {
//         ww = (nWidth*3+31)/32*32 ;
//         hDIBImg = GlobalAlloc ( GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER)+ww*nHeight+16 ) ;
//         if ( hDIBImg != (HGLOBAL)NULL )
//         {
//             lpDIBImg = (LPBITMAPINFO)GlobalLock( hDIBImg ) ;
//             if ( lpDIBImg != (LPBITMAPINFO)NULL )
//             {
//                 lpDIBImg->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
//                 lpDIBImg->bmiHeader.biWidth = nWidth ;
//                 lpDIBImg->bmiHeader.biHeight = nHeight ;
//                 lpDIBImg->bmiHeader.biPlanes = 1 ;
//                 lpDIBImg->bmiHeader.biBitCount = 24 ;
//                 lpDIBImg->bmiHeader.biCompression = BI_RGB ;
//                 lpDIBImg->bmiHeader.biSizeImage = ww*nHeight ;
//                 lpDIBImg->bmiHeader.biClrUsed = 0 ;
//                 lpDIBImg->bmiHeader.biClrImportant = 0 ;
//                 lpTmp = (BYTE *)GetRawImgPtr( lpDIBImg ) ;
//                 for ( i=0 ; i<nHeight ; i++ )
//                 {
// 					if( !bUpsideDown )
// 						memcpy ( lpTmp+(nHeight-1-i)*ww, lpRawImg+nWidth*3*i, nWidth*3 ) ;
// 					else
// 						memcpy ( lpTmp+i*ww, lpRawImg+nWidth*3*i, nWidth*3 ) ;
// 				}
//                 for ( i=0 ; i<nHeight ; i++ )
//                 {
//                     BYTE *pLine = lpTmp+i*ww ;
//                     for ( j=0 ; j<nWidth ; j++, pLine+=3 )
//                     {
//                         BYTE blue = *pLine ;
//                         *pLine = *(pLine+2) ;  // red
//                         *(pLine+2) = blue ;
//                     }
//                 }
//                 GlobalUnlock(hDIBImg) ;
//           	    FreeMem() ;
//                 m_hImg = hDIBImg ;
//                 m_bImported = FALSE ;
//                 return TRUE ;
//             }
//             else
//                 GlobalFree ( hDIBImg ) ;
//         }
//     }
//   }
//   return FALSE ;
// }

/************************************************************************************
/*	Name:		Clear
/*	Function:	Clear image
/*	Parameter:	NULL
/*	return:		0 -- Succeed
/***********************************************************************************/
// int	 CImage::Clear( )
// {
// 	BYTE *pImg = LockRawImg( );
// 	int wb = GetLineSizeInBytes( );
// 	int h = GetHeight( );
// 	memset( pImg, 0, wb*h );
// 	UnlockRawImg( );
// 	return 0;
// }

/************************************************************************************
/*	Name:		InitializeUpSideDown
/*	Function:	Initialize image
/*	Parameter:	lpRawImg -- Pointer to DIB image data
/*				nWidth -- Image width
/*				nHeight -- Image height
/*				nImgType -- Image type
/*	return:		TRUE -- Succeed
/*				FALSE -- Failed
/*
/***********************************************************************************/
// int CImage::InitializeUpSideDown ( BYTE *lpRawImg, int nWidth, int nHeight, int nImgType )
// {
//   int i, j, ww ;
//   HGLOBAL hDIBImg ;
//   LPBITMAPINFO lpDIBImg ;
//   BYTE *lpTmp ;
// 
//   if ( lpRawImg )
//   {
//     if ( nImgType == DIB_8BIT )
//     {
//         ww = (nWidth+3)/4*4 ;
//         hDIBImg = GlobalAlloc ( GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)+ww*nHeight+16 ) ;
//         if ( hDIBImg != (HGLOBAL)NULL )
//         {
//             lpDIBImg = (LPBITMAPINFO)GlobalLock( hDIBImg ) ;
//             if ( lpDIBImg != (LPBITMAPINFO)NULL )
//             {
//                 lpDIBImg->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
//                 lpDIBImg->bmiHeader.biWidth = nWidth ;
//                 lpDIBImg->bmiHeader.biHeight = nHeight ;
//                 lpDIBImg->bmiHeader.biPlanes = 1 ;
//                 lpDIBImg->bmiHeader.biBitCount = 8 ;
//                 lpDIBImg->bmiHeader.biCompression = BI_RGB ;
//                 lpDIBImg->bmiHeader.biSizeImage = ww * nHeight ;
//                 lpDIBImg->bmiHeader.biClrUsed = 0 ;
//                 lpDIBImg->bmiHeader.biClrImportant = 0 ;
//                 SetPalette ( lpDIBImg ) ;
//                 lpTmp = (BYTE *)GetRawImgPtr( lpDIBImg ) ;
//                 for ( i=0 ; i<nHeight ; i++ )
//                     memcpy ( lpTmp+i*ww, lpRawImg+nWidth*i, nWidth ) ;
//                 GlobalUnlock(hDIBImg) ;
//           	    FreeMem() ;
//                 m_hImg = hDIBImg ;
//                 m_bImported = FALSE ;
//                 return TRUE ;
//             }
//             else
//                 GlobalFree ( hDIBImg ) ;
//         }
//     }
//     else if ( nImgType == DIB_1BIT )
//     {
//         ww = (nWidth+31)/32*32 ;
// 		hDIBImg = GlobalAlloc ( GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER)+2*sizeof(RGBQUAD)+ww/8*nHeight+16 ) ;
//         if ( hDIBImg != (HGLOBAL)NULL )
//         {
//             lpDIBImg = (LPBITMAPINFO)GlobalLock( hDIBImg ) ;
//             if ( lpDIBImg != (LPBITMAPINFO)NULL )
//             {
//                 lpDIBImg->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
//                 lpDIBImg->bmiHeader.biWidth = nWidth ;
//                 lpDIBImg->bmiHeader.biHeight = nHeight ;
//                 lpDIBImg->bmiHeader.biPlanes = 1 ;
//                 lpDIBImg->bmiHeader.biBitCount = 1 ;
//                 lpDIBImg->bmiHeader.biCompression = BI_RGB ;
//                 lpDIBImg->bmiHeader.biSizeImage = ww/8*nHeight ;
//                 lpDIBImg->bmiHeader.biClrUsed = 0 ;
//                 lpDIBImg->bmiHeader.biClrImportant = 0 ;
//                 SetPalette ( lpDIBImg ) ;
//                 lpTmp = (BYTE *)GetRawImgPtr( lpDIBImg ) ;
//                 for ( i=0 ; i<nHeight ; i++ )
//                     memcpy ( lpTmp+i*ww/8, lpRawImg+i*ww/8, (nWidth+7)/8 ) ;
//                 GlobalUnlock(hDIBImg) ;
//           	    FreeMem() ;
//                 m_hImg = hDIBImg ;
//                 m_bImported = FALSE ;
//                 return TRUE ;
//             }
//             else
//                 GlobalFree ( hDIBImg ) ;
//         }
//     }
//     else if ( nImgType == DIB_24BIT )
//     {
//         ww = (nWidth*3+31)/32*32 ;
//         hDIBImg = GlobalAlloc ( GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER)+ww*nHeight+16 ) ;
//         if ( hDIBImg != (HGLOBAL)NULL )
//         {
//             lpDIBImg = (LPBITMAPINFO)GlobalLock( hDIBImg ) ;
//             if ( lpDIBImg != (LPBITMAPINFO)NULL )
//             {
//                 lpDIBImg->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
//                 lpDIBImg->bmiHeader.biWidth = nWidth ;
//                 lpDIBImg->bmiHeader.biHeight = nHeight ;
//                 lpDIBImg->bmiHeader.biPlanes = 1 ;
//                 lpDIBImg->bmiHeader.biBitCount = 24 ;
//                 lpDIBImg->bmiHeader.biCompression = BI_RGB ;
//                 lpDIBImg->bmiHeader.biSizeImage = ww*nHeight ;
//                 lpDIBImg->bmiHeader.biClrUsed = 0 ;
//                 lpDIBImg->bmiHeader.biClrImportant = 0 ;
//                 lpTmp = (BYTE *)GetRawImgPtr( lpDIBImg ) ;
//                 for ( i=0 ; i<nHeight ; i++ )
//                     memcpy ( lpTmp+i*ww, lpRawImg+nWidth*3*i, nWidth*3 ) ;
//                 for ( i=0 ; i<nHeight ; i++ )
//                 {
//                     BYTE *pLine = lpTmp+i*ww ;
//                     for ( j=0 ; j<nWidth ; j++, pLine+=3 )
//                     {
//                         BYTE blue = *pLine ;
//                         *pLine = *(pLine+2) ;  // red
//                         *(pLine+2) = blue ;
//                     }
//                 }
//                 GlobalUnlock(hDIBImg) ;
//           	    FreeMem() ;
//                 m_hImg = hDIBImg ;
//                 m_bImported = FALSE ;
//                 return TRUE ;
//             }
//             else
//                 GlobalFree ( hDIBImg ) ;
//         }
//     }
//   }
//   return FALSE ;
// }

/************************************************************************************
/*	Name:		Initialize
/*	Function:	Initialize image
/*	Parameter:	lpRawImg -- Pointer to DIB image data
/*				nWidth -- Image width
/*				nHeight -- Image height
/*				nWidthByte -- Bytes number every image line
/*				nImgType -- Image type
/*	return:		TRUE -- Succeed
/*				FALSE -- Failed
/*
/***********************************************************************************/
// int CImage::Initialize ( BYTE *lpRawImg, int nWidth, int nHeight, int nWidthByte, int nBitCount, int bUpSideDown )
// {
// 	if ( lpRawImg == NULL )
// 	  return FALSE;
// 	m_hImg = NULL;
// 	m_pImg = lpRawImg;
//     m_bImported = TRUE ;
// 	m_nWidth = nWidth;
// 	m_nHeight = nHeight;
// 	m_nWidthByte = nWidthByte;
// 	m_nBitCount = nBitCount;
// 	m_bUpSideDown = bUpSideDown;
//     return TRUE ;
// }

/************************************************************************************
/*	Name:		Initialize
/*	Function:	Initialize image
/*	Parameter:	nWidth -- Image width
/*				nHeight -- Image height
/*				nWidthByte -- Bytes number every image line
/*				nImgType -- Image type
/*	return:		TRUE -- Succeed
/*				FALSE -- Failed
/*
/***********************************************************************************/
int CImage::Initialize(int nWidth, int nHeight, int nWidthByte, int nBitCount, int bUpSideDown) {
	DSCC_BYTE* lpRawImg = new DSCC_BYTE[nHeight*nWidthByte];
	if (lpRawImg == NULL)
		return FALSE;
//	m_hImg = NULL;
	m_pImg = lpRawImg;
//	m_bImported = TRUE;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nWidthByte = nWidthByte;
	m_nBitCount = nBitCount;
	m_bUpSideDown = bUpSideDown;
	return TRUE;
}

/************************************************************************************
/*	Name:		Initialize
/*	Function:	Copy an image region from a whole image
/*	Parameter:	hDIBRawImg -- Whole image data
/*				rcRegion -- Image region to copied
/*				nImgType -- Image type
/*	return:		TRUE -- Succeed
/*				FALSE -- Failed
/*
/***********************************************************************************/
// int CImage::Initialize ( HGLOBAL hDIBRawImg, CRect rcRegion, int nImgType )
// {
//   int i, j, ww ;
//   HGLOBAL hDIBImg ;
//   LPBITMAPINFO lpDIBImg ;
//   BYTE *lpTmp ;
//   int nWidth = rcRegion.right-rcRegion.left;
//   int nHeight = rcRegion.bottom-rcRegion.top;
//   LPBITMAPINFO	lpDIBRawImg = (LPBITMAPINFO)GlobalLock( hDIBRawImg );
//   int nWholeWidth = lpDIBRawImg->bmiHeader.biWidth;
//   int nWholeHeight = lpDIBRawImg->bmiHeader.biHeight;
//   int w2;
//   BYTE*	lpRawImg = (BYTE *)GetRawImgPtr( lpDIBRawImg );
// 
//   if ( lpRawImg )
//   {
//     if ( nImgType == DIB_8BIT )
//     {
//         ww = (nWidth+3)/4*4 ;
//         w2 = (nWholeWidth+3)/4*4;
// 		hDIBImg = GlobalAlloc ( GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)+ww*nHeight+16 ) ;
//         if ( hDIBImg != (HGLOBAL)NULL )
//         {
//             lpDIBImg = (LPBITMAPINFO)GlobalLock( hDIBImg ) ;
//             if ( lpDIBImg != (LPBITMAPINFO)NULL )
//             {
//                 lpDIBImg->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
//                 lpDIBImg->bmiHeader.biWidth = nWidth ;
//                 lpDIBImg->bmiHeader.biHeight = nHeight ;
//                 lpDIBImg->bmiHeader.biPlanes = 1 ;
//                 lpDIBImg->bmiHeader.biBitCount = 8 ;
//                 lpDIBImg->bmiHeader.biCompression = BI_RGB ;
//                 lpDIBImg->bmiHeader.biSizeImage = ww * nHeight ;
//                 lpDIBImg->bmiHeader.biClrUsed = 0 ;
//                 lpDIBImg->bmiHeader.biClrImportant = 0 ;
//                 SetPalette ( lpDIBImg ) ;
//                 lpTmp = (BYTE *)GetRawImgPtr( lpDIBImg ) ;
//                 for ( i=0 ; i<nHeight ; i++ )
//                     memcpy ( lpTmp+(nHeight-1-i)*ww, lpRawImg+(nWholeHeight-1-i-rcRegion.top)*w2+rcRegion.left, nWidth ) ;
//                 GlobalUnlock(hDIBImg) ;
//           	    GlobalUnlock(hDIBRawImg) ;
// 				FreeMem() ;
//                 m_hImg = hDIBImg ;
//                 m_bImported = FALSE ;
//                 return TRUE ;
//             }
//             else
//                 GlobalFree ( hDIBImg ) ;
//         }
//     }
//     else if ( nImgType == DIB_1BIT )
//     {
//         ww = (nWidth+31)/32*32 ;
// 		w2 = (nWholeWidth+31)/32*32 ;
// 		hDIBImg = GlobalAlloc ( GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER)+2*sizeof(RGBQUAD)+ww/8*nHeight+16 ) ;
//         if ( hDIBImg != (HGLOBAL)NULL )
//         {
//             lpDIBImg = (LPBITMAPINFO)GlobalLock( hDIBImg ) ;
//             if ( lpDIBImg != (LPBITMAPINFO)NULL )
//             {
//                 lpDIBImg->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
//                 lpDIBImg->bmiHeader.biWidth = nWidth ;
//                 lpDIBImg->bmiHeader.biHeight = nHeight ;
//                 lpDIBImg->bmiHeader.biPlanes = 1 ;
//                 lpDIBImg->bmiHeader.biBitCount = 1 ;
//                 lpDIBImg->bmiHeader.biCompression = BI_RGB ;
//                 lpDIBImg->bmiHeader.biSizeImage = ww/8*nHeight ;
//                 lpDIBImg->bmiHeader.biClrUsed = 0 ;
//                 lpDIBImg->bmiHeader.biClrImportant = 0 ;
//                 SetPalette ( lpDIBImg ) ;
//                 lpTmp = (BYTE *)GetRawImgPtr( lpDIBImg ) ;
//                 for ( i=0 ; i<nHeight ; i++ )
//                     memcpy ( lpTmp+(nHeight-1-i)*ww/8, lpRawImg+(nWholeHeight-1-i-rcRegion.top)*w2/8+rcRegion.left/8, (nWidth+7)/8 ) ;
//                 GlobalUnlock(hDIBImg) ;
// 				GlobalUnlock(hDIBRawImg) ;
//           	    FreeMem() ;
//                 m_hImg = hDIBImg ;
//                 m_bImported = FALSE ;
//                 return TRUE ;
//             }
//             else
//                 GlobalFree ( hDIBImg ) ;
//         }
//     }
//     else if ( nImgType == DIB_24BIT )
//     {
//         ww = (nWidth*3+31)/32*32 ;
// 		w2 = (nWholeWidth*3+31)/32*32 ;
//         hDIBImg = GlobalAlloc ( GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER)+ww*nHeight+16 ) ;
//         if ( hDIBImg != (HGLOBAL)NULL )
//         {
//             lpDIBImg = (LPBITMAPINFO)GlobalLock( hDIBImg ) ;
//             if ( lpDIBImg != (LPBITMAPINFO)NULL )
//             {
//                 lpDIBImg->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
//                 lpDIBImg->bmiHeader.biWidth = nWidth ;
//                 lpDIBImg->bmiHeader.biHeight = nHeight ;
//                 lpDIBImg->bmiHeader.biPlanes = 1 ;
//                 lpDIBImg->bmiHeader.biBitCount = 24 ;
//                 lpDIBImg->bmiHeader.biCompression = BI_RGB ;
//                 lpDIBImg->bmiHeader.biSizeImage = ww*nHeight ;
//                 lpDIBImg->bmiHeader.biClrUsed = 0 ;
//                 lpDIBImg->bmiHeader.biClrImportant = 0 ;
//                 lpTmp = (BYTE *)GetRawImgPtr( lpDIBImg ) ;
//                 for ( i=0 ; i<nHeight ; i++ )
//                     memcpy ( lpTmp+(nHeight-1-i)*ww*3, lpRawImg+(nWholeHeight-1-i)*w2*3+rcRegion.left*3, nWidth*3 ) ;
//                 for ( i=0 ; i<nHeight ; i++ )
//                 {
//                     BYTE *pLine = lpTmp+i*ww ;
//                     for ( j=0 ; j<nWidth ; j++, pLine+=3 )
//                     {
//                         BYTE blue = *pLine ;
//                         *pLine = *(pLine+2) ;  // red
//                         *(pLine+2) = blue ;
//                     }
//                 }
//                 GlobalUnlock(hDIBImg) ;
// 				GlobalUnlock(hDIBRawImg) ;
//           	    FreeMem() ;
//                 m_hImg = hDIBImg ;
//                 m_bImported = FALSE ;
//                 return TRUE ;
//             }
//             else
//                 GlobalFree ( hDIBImg ) ;
//         }
//     }
//   }
//   return FALSE ;
// }

/************************************************************************************
/*	Name:		Initialize
/*	Function:	Copy an image region from a whole image
/*	Parameter:	LPBYTE *lpLine -- Image line pointer array
/*				nWidth -- Image width
/*				nHeight -- Image height
/*				rcRegion -- Image region to copied
/*				nImgType -- Image type
/*	return:		TRUE -- Succeed
/*				FALSE -- Failed
/***********************************************************************************/
// int CImage::Initialize ( LPBYTE *lpLine, int nWidth, int nHeight, CRect rcRegion, int nImgType )
// {
// 	if( lpLine == NULL )
// 		return FALSE;
// 	int i, j, ww ;
// 	HGLOBAL hDIBImg ;
// 	LPBITMAPINFO lpDIBImg ;
// 	BYTE *lpTmp ;
// 	int w = rcRegion.right-rcRegion.left;
// 	int h = rcRegion.bottom-rcRegion.top;
// 	int w2;
// 
// 	 if ( nImgType == DIB_1BIT )
//     {
//         ww = (w+31)/32*32 ;
// 		w2 = (nWidth+31)/32*32 ;
// 		hDIBImg = GlobalAlloc ( GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER)+2*sizeof(RGBQUAD)+ww/8*h+16 ) ;
//         if ( hDIBImg != (HGLOBAL)NULL )
//         {
//             lpDIBImg = (LPBITMAPINFO)GlobalLock( hDIBImg ) ;
//             if ( lpDIBImg != (LPBITMAPINFO)NULL )
//             {
//                 lpDIBImg->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
//                 lpDIBImg->bmiHeader.biWidth = w ;
//                 lpDIBImg->bmiHeader.biHeight = h ;
//                 lpDIBImg->bmiHeader.biPlanes = 1 ;
//                 lpDIBImg->bmiHeader.biBitCount = 1 ;
//                 lpDIBImg->bmiHeader.biCompression = BI_RGB ;
//                 lpDIBImg->bmiHeader.biSizeImage = ww/8*h ;
//                 lpDIBImg->bmiHeader.biClrUsed = 0 ;
//                 lpDIBImg->bmiHeader.biClrImportant = 0 ;
//                 SetPalette ( lpDIBImg ) ;
//                 lpTmp = (BYTE *)GetRawImgPtr( lpDIBImg ) ;
//                 for ( i=0 ; i<h ; i++ )
//                     memcpy ( lpTmp+(h-1-i)*ww/8, lpLine[i+rcRegion.top]+rcRegion.left/8, (w+7)/8 ) ;
//                 GlobalUnlock(hDIBImg) ;
//           	    FreeMem() ;
//                 m_hImg = hDIBImg ;
//                 m_bImported = FALSE ;
//                 return TRUE ;
//             }
//             else
//                 GlobalFree ( hDIBImg ) ;
//         }
//     }
// 	else if ( nImgType == DIB_8BIT )
//     {
//         ww = (w+3)/4*4 ;
//         w2 = (nWidth+3)/4*4;
// 		hDIBImg = GlobalAlloc ( GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)+ww*h+16 ) ;
//         if ( hDIBImg != (HGLOBAL)NULL )
//         {
//             lpDIBImg = (LPBITMAPINFO)GlobalLock( hDIBImg ) ;
//             if ( lpDIBImg != (LPBITMAPINFO)NULL )
//             {
//                 lpDIBImg->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
//                 lpDIBImg->bmiHeader.biWidth = w ;
//                 lpDIBImg->bmiHeader.biHeight = h ;
//                 lpDIBImg->bmiHeader.biPlanes = 1 ;
//                 lpDIBImg->bmiHeader.biBitCount = 8 ;
//                 lpDIBImg->bmiHeader.biCompression = BI_RGB ;
//                 lpDIBImg->bmiHeader.biSizeImage = ww * h ;
//                 lpDIBImg->bmiHeader.biClrUsed = 0 ;
//                 lpDIBImg->bmiHeader.biClrImportant = 0 ;
//                 SetPalette ( lpDIBImg ) ;
//                 lpTmp = (BYTE *)GetRawImgPtr( lpDIBImg ) ;
//                 for ( i=0 ; i<h ; i++ )
//                     memcpy ( lpTmp+(h-1-i)*ww, lpLine[i+rcRegion.top]+rcRegion.left, w ) ;
//                 GlobalUnlock(hDIBImg) ;
// 				FreeMem() ;
//                 m_hImg = hDIBImg ;
//                 m_bImported = FALSE ;
//                 return TRUE ;
//             }
//             else
//                 GlobalFree ( hDIBImg ) ;
//         }
//     }
//     else if ( nImgType == DIB_24BIT )
//     {
//         ww = (w*3+31)/32*32 ;
// 				w2 = (nWidth*3+31)/32*32 ;
//         hDIBImg = GlobalAlloc ( GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER)+ww*h+16 ) ;
//         if ( hDIBImg != (HGLOBAL)NULL )
//         {
//             lpDIBImg = (LPBITMAPINFO)GlobalLock( hDIBImg ) ;
//             if ( lpDIBImg != (LPBITMAPINFO)NULL )
//             {
//                 lpDIBImg->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
//                 lpDIBImg->bmiHeader.biWidth = w ;
//                 lpDIBImg->bmiHeader.biHeight = h ;
//                 lpDIBImg->bmiHeader.biPlanes = 1 ;
//                 lpDIBImg->bmiHeader.biBitCount = 24 ;
//                 lpDIBImg->bmiHeader.biCompression = BI_RGB ;
//                 lpDIBImg->bmiHeader.biSizeImage = ww*h ;
//                 lpDIBImg->bmiHeader.biClrUsed = 0 ;
//                 lpDIBImg->bmiHeader.biClrImportant = 0 ;
//                 lpTmp = (BYTE *)GetRawImgPtr( lpDIBImg ) ;
//                 for ( i=0 ; i<h ; i++ )
//                     memcpy ( lpTmp+(h-1-i)*ww*3, lpLine[i+rcRegion.top]+rcRegion.left*3, w*3 ) ;
//                 for ( i=0 ; i<h ; i++ )
//                 {
//                     BYTE *pLine = lpTmp+i*ww ;
//                     for ( j=0 ; j<w ; j++, pLine+=3 )
//                     {
//                         BYTE blue = *pLine ;
//                         *pLine = *(pLine+2) ;  // red
//                         *(pLine+2) = blue ;
//                     }
//                 }
//                 GlobalUnlock(hDIBImg) ;
//           	    FreeMem() ;
//                 m_hImg = hDIBImg ;
//                 m_bImported = FALSE ;
//                 return TRUE ;
//             }
//             else
//                 GlobalFree ( hDIBImg ) ;
//         }
//     }
// 	return FALSE ;
// }

///////////////////////////////////////////////////////////////////////////////////////////////////
//	Read and write image
//////////////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************************
/*	Name:		LockRawImg
/*	Function:	Lock image data in memory
/*	Parameter:	None
/*	return:		NULL -- Faied
/*				Others -- Pointer to image data
/***********************************************************************************/
DSCC_BYTE *CImage::LockRawImg()
{
//     if ( m_hImg != (HGLOBAL)NULL )
//     {
//         LPBITMAPINFO lpbi = (LPBITMAPINFO)GlobalLock(m_hImg) ;
//         return GetRawImgPtr ( lpbi ) ;
//     }
//     else
        return m_pImg ;
}

/************************************************************************************
/*	Name:		UnlockRawImg
/*	Function:	Unlock image data in memory
/*	Parameter:	None
/*	return:		0
/*
/***********************************************************************************/
int CImage::UnlockRawImg()
{
//  	if( m_hImg != NULL )
//  		GlobalUnlock(m_hImg) ;
    return 0 ;
}

/************************************************************************************
/*	Name:		GetRawImgPtr
/*	Function:	Get pointer of raw image data
/*	Parameter:	lpbi -- Pointer to BITMAPINFO 
/*	return:		NULL -- Faied
/*				Others -- Pointer to raw image data
/*
/***********************************************************************************/
// BYTE *CImage::GetRawImgPtr ( LPBITMAPINFO lpbi )
// {
//     BYTE *p = (BYTE *)lpbi ;
//     int nPaletteSize = GetPaletteSize(lpbi) ;
//     if ( nPaletteSize >= 0 )
//         return p+sizeof(BITMAPINFOHEADER)+nPaletteSize ;
//     else
//         return (BYTE *)NULL ;
// }


/************************************************************************************
/*	Name:		GetFileExtension
/*	Function:	Get file extension
/*	Parameter:	FileName -- Image file name;
/*	return:		File extension
/***********************************************************************************/
// CString CImage::GetFileExtension ( CString FileName )
// {
//     int pos ;
//     if ( (pos=FileName.ReverseFind('.')) >= 0 )
//     {
//         return FileName.Right(FileName.GetLength()-pos-1) ;
//     }
//     else
//         return "" ;
// }

/************************************************************************************
/*	Name:		Read
/*	Function:	Read image
/*	Parameter:	csFileName -- Image file name;
/*				bJudgeInvert -- Invert automatically or not
/*	return:		0 -- Succeed
/***********************************************************************************/
// int CImage::Read(char* csFileName, int bJudgeInvert)
// {
// 	//if( ReadImage( csFileName, m_hImg ) != 0 )
// 		return -1;
// 	if( bJudgeInvert )
// 	{
// 		if ( m_hImg == (HGLOBAL)NULL )
// 			return -1 ;
// 		if( NeedInvertBits() )
// 			InvertBits();
// 		if( NeedInvertPallete() )
// 			InvertPallete();
// 	}
// 	else if( GetType() == DIB_1BIT )
// 	{
// 		if( NeedInvertPallete() )
// 		{
// 			InvertPallete();
// 			InvertBits();
// 		}
// 	}
// 	return 0 ;
// }

/************************************************************************************
/*	Name:		Write
/*	Function:	Write image file
/*	Parameter:	csFileName -- Image file name
/*	return:		0  -- Succeed
/*				-1 -- Failed
/*
/***********************************************************************************/
// int CImage::Write(CString csFileName)
// {
// 	CString Dir = "";
//     int pos, l ;
//     l = csFileName.GetLength();
//     if ( (pos=csFileName.ReverseFind('\\')) >= 0 )
// 			Dir = csFileName.Left ( pos+1 ) ;
//     if( Dir != "" )
// 	{
// 		CFileFind a;
// //		if( Dir.GetAt(1) != ':' )
// //			Dir = ".\\"+Dir;
// 		
// 		CString sFile = Dir + "\\*.*";
// 		if( a.FindFile(sFile, 0) == 0)		
// 			if( CreateDirectory(Dir, NULL) == 0 )
// 			{
// 				printf( "The directory does not exist and the creation of the directory failed!\n" );
// 				return -1;
// 			}
// 		a.Close();
// 	}
// 	//WriteImage( csFileName.GetBuffer(256), m_hImg );
//     return 0 ;
// }

////////////////////////////////////////////////////////////////////////////////////////
//	Get image information
////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************************
/*	Name:		GetLineSizeInByte
/*	Function:	Get the number of bytes used to represent an image line
/*	Parameter:	lpbi -- Pointer to BITMAPINFO
/*	return:		Number of bytes used to represent an image line
/*
/***********************************************************************************/
// int CImage::GetLineSizeInBytes(LPBITMAPINFO lpbi)
// {
//     int nWidth  = lpbi->bmiHeader.biWidth ;
//     switch ( lpbi->bmiHeader.biBitCount )
//     {
//         case 1 :
//             return (nWidth+31)/32*4 ;
// 		case 4 :
// 			return ((nWidth+1)/2+3)/4*4 ;
//         case 8 :
//             return (nWidth+3)/4*4 ;
//         case 16 :
//             return (nWidth*2+3)/4*4 ;
//         case 24 :
//             return (nWidth*3+3)/4*4 ;
//         case 32 :
//             return nWidth*4 ;
//         default :
//             return -1 ;
//     }
// }

/************************************************************************************
/*	Name:		GetLineSizeInByte
/*	Function:	Get the number of bytes used to represent an image line
/*	Parameter:	None
/*	return:		Number of bytes used to represent an image line
/*
/***********************************************************************************/
int CImage::GetLineSizeInBytes()
{
//     if ( m_hImg != (HGLOBAL)NULL )
//     {
//         LPBITMAPINFO lpbi = (LPBITMAPINFO)GlobalLock(m_hImg) ;
// 	    int n = GetLineSizeInBytes(lpbi) ;
//         GlobalUnlock(m_hImg) ;
//         return n ;
//     }
//     else
		return m_nWidthByte;
}

/************************************************************************************
/*	Name:		GetWidth
/*	Function:	Get image width in pixels
/*	Parameter:	None
/*	return:		Image width
/*
/***********************************************************************************/
int CImage::GetWidth()
{
// 	if (m_hImg != (HGLOBAL)NULL) {
// 		LPBITMAPINFO lpbi = (LPBITMAPINFO)GlobalLock(m_hImg);
// 		int nWidth = lpbi->bmiHeader.biWidth;
// 		GlobalUnlock(m_hImg);
// 		return nWidth;
// 	} else
        return m_nWidth;
}

/************************************************************************************
/*	Name:		GetHeight
/*	Function:	Get image height in pixels
/*	Parameter:	None
/*	return:		Image height
/*
/***********************************************************************************/
int CImage::GetHeight()
{
//     if ( m_hImg != (HGLOBAL)NULL )
//     {
//         LPBITMAPINFO lpbi = (LPBITMAPINFO)GlobalLock(m_hImg) ;
// 	    int nHeight  = lpbi->bmiHeader.biHeight ;
//         GlobalUnlock(m_hImg) ;
//         return nHeight ;
//     }
//     else
        return m_nHeight;
}

/************************************************************************************
/*	Name:		GetOrientation
/*	Function:	Is upside down
/*	Parameter:	None
/*	Return:		TRUE -- Upside down
/*				FALSe -- Not upside down
/***********************************************************************************/
int CImage::GetOrientation()
{
	return	m_bUpSideDown;
}
/************************************************************************************
/*	Name:		GetType
/*	Function:	Get image type
/*	Parameter:	None
/*	return:		Image type
/*
/***********************************************************************************/
int CImage::GetType()
{
// 	if (m_hImg != (HGLOBAL)NULL) {
// 		LPBITMAPINFO lpbi = (LPBITMAPINFO)GlobalLock(m_hImg);
// 		m_nBitCount = lpbi->bmiHeader.biBitCount;
// 		GlobalUnlock(m_hImg);
// 	}
    switch ( m_nBitCount )
    {
        case 1 :
            return DIB_1BIT ;
		case 4:
			return DIB_4BIT ;
        case 8 :
            return DIB_8BIT ;
        case 16 :
            return DIB_16BIT ;
        case 24 :
            return DIB_24BIT ;
        case 32 :
            return DIB_32BIT ;
        default :
            return DIB_UNKNOWN ;
    }
}

/************************************************************************************
/*	Name:		GetPaletteSize
/*	Function:	Get palette size
/*	Parameter:	None
/*	return:		Palette size
/*
/***********************************************************************************/
// int CImage::GetPaletteSize ( LPBITMAPINFO lpbi )
// {
//     switch ( lpbi->bmiHeader.biBitCount )
//     {
//         case 1 :
// 			return 2*sizeof(RGBQUAD) ;
// 		case 4 :
// 			return 16*sizeof(RGBQUAD) ;
//         case 8 :
//             return 256*sizeof(RGBQUAD) ;
//         case 16 :
//         case 24 :
//         case 32 :
//             return 0 ;
//         default :
//             return -1 ;
//     }
// }

/************************************************************************************
/*	Name:		GethImg
/*	Function:	Get image data handle
/*	Parameter:	None
/*	return:		Image data handle
/*
/***********************************************************************************/
// HGLOBAL CImage::GethImg()
// {
//     return m_hImg ;
// }

/************************************************************************************
/*	Name:		GetBlackPixelRatio
/*	Function:	Get black pixel ratio of an image
/*	Parameter:	None
/*	return:		Black pixel ratio
/*
/***********************************************************************************/
double CImage::GetBlackPixelRatio()
{
    if ( GetType() != DIB_1BIT )
        return -1 ;

    int Tbl[256], i, j ;
    DSCC_BYTE mask[8] = {1, 2, 4, 8, 16, 32, 64, 128} ;
	DSCC_BYTE rz[8]={ 0xff,0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe };//Right mask

    i = Tbl[0] = 0 ;
    do {
        i++ ;
        Tbl[i] = 0 ;
        for ( j=0 ; j<8 ; j++ )
            if ( i&mask[j] )
                Tbl[i]++ ;
    } while ( i != 255 ) ;

    int w = GetWidth() ; 
    int h = GetHeight() ;

    int TotalBlackPixels = 0 ;
    DSCC_BYTE *pImg = LockRawImg() ;
    int wb = GetLineSizeInBytes() ;
    for ( i=0 ; i<h ; i++ )
    {
        DSCC_BYTE *p = pImg + i*wb ;
        for ( j=0 ; j<(w+7)/8 ; j++ )
		{
			if( j==(w+7)/8-1 )
	            TotalBlackPixels += Tbl[p[j]&rz[w%8]] ;
			else
				TotalBlackPixels += Tbl[p[j]] ;
		}
    }
    UnlockRawImg() ;
    return (double)TotalBlackPixels / ( (double)w * (double)h ) ;
}


///////////////////////////////////////////////////////////////////////////////////////////
//	Other methods
///////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************************
/*	Name:		NeedInvertBits
/*	Function:	Judge if need to invert the image
/*	Parameter:	None
/*	return:		TRUE -- Need to invert image
/*				FALSE -- Do not need
/*
/***********************************************************************************/
// int	CImage::NeedInvertBits()
// {
// 	if( GetType() != DIB_1BIT && GetType() != DIB_8BIT )
// 		return FALSE;
// 	int		w = GetWidth();
// 	int		wb = GetLineSizeInBytes();
// 	int		h = GetHeight();
// 	if( h<=1 )	return FALSE;
// 	BYTE	*pImg = LockRawImg();
// 	int		i;
// 	long	BlackCount=0;
// 	LPBITMAPINFO lpbi = (LPBITMAPINFO) GlobalLock ( m_hImg ) ;
// 
// 	if( GetType() == DIB_1BIT )
// 	{
// 		int	wb2 = (w+7)/8;
// 		for( i=0; i<h; i++)
// 		{
// 			BlackCount += lut[*( pImg+(h-1-i)*wb+i*(wb2-1)/(h-1) )];
// 			BlackCount += lut[*( pImg+(h-1-i)*wb+(h-1-i)*(wb2-1)/(h-1)) ];
// 		}
// 		GlobalUnlock( m_hImg );
// 		UnlockRawImg();
// 		if( BlackCount > 8*h )
// 			return TRUE;
// 		else
// 			return FALSE;
// 	}
// 	else
// 	{
//         LPBITMAPINFO lpbi = (LPBITMAPINFO) GlobalLock ( m_hImg ) ;
// 		//If not gray image return FALSE, don't invert bits.
// 		int	bUsed[256];
// 		for( int i=0; i<256; i++ )
// 			bUsed[i] = FALSE;
// 		for( i=0; i<256; i++ )
// 		{
// 			if( lpbi->bmiColors[i].rgbBlue != lpbi->bmiColors[i].rgbGreen || lpbi->bmiColors[i].rgbBlue != lpbi->bmiColors[i].rgbRed )
// 				break;
// 			bUsed[lpbi->bmiColors[i].rgbBlue] = TRUE;
// 		}
// 		if( i< 256 )
// 		{
// 			GlobalUnlock( m_hImg );
// 			UnlockRawImg();
// 			return FALSE;
// 		}
// 		for( i=0; i<256; i++)
// 			if( bUsed[i] == FALSE )
// 			{
// 				GlobalUnlock( m_hImg );
// 				UnlockRawImg();
// 				return FALSE; 
// 			}
// 
// 		for( i=0; i<h; i++ )
// 		{
// 			BlackCount += *(pImg+(h-1-i)*wb+i*(w-1)/(h-1));
// 			BlackCount += *(pImg+(h-1-i)*wb+(h-1-i)*(w-1)/(h-1));
// 		}
// 		GlobalUnlock( m_hImg );
// 		UnlockRawImg();
// 		if( BlackCount > 128*h )
// 			return FALSE;
// 		else 
// 			return TRUE;
// 	}
// 	return FALSE;
// }

/************************************************************************************
*	Name:		NeedInvertPalette
/*	Function:	Judge if need to invert palette
/*	Parameter:	None
/*	return:		TRUE -- Need to invert palette
/8				FALSE -- Do not need
/*
/***********************************************************************************/
// int CImage::NeedInvertPallete()
// {
//     int rt ;
//     if ( GetType() == DIB_1BIT )
//     {
//         LPBITMAPINFO lpbi = (LPBITMAPINFO) GlobalLock ( m_hImg ) ;
//         rt =  (int)lpbi->bmiColors[0].rgbBlue+(int)lpbi->bmiColors[0].rgbGreen+(int)lpbi->bmiColors[0].rgbRed
//             < (int)lpbi->bmiColors[1].rgbBlue+(int)lpbi->bmiColors[1].rgbGreen+(int)lpbi->bmiColors[1].rgbRed ;
//         GlobalUnlock (m_hImg) ;
//         return rt ;
//     }
// 	else if ( GetType() == DIB_8BIT )
// 	{
//         LPBITMAPINFO lpbi = (LPBITMAPINFO) GlobalLock ( m_hImg ) ;
// 		//If not gray image, return FALSE, do not invert pallete.
// 		int	bUsed[256];
// 		int i = 0;
// 		for( ; i<256; i++ )
// 			bUsed[i] = FALSE;
// 		for( i=0; i<256; i++ )
// 		{
// 			if( lpbi->bmiColors[i].rgbBlue != lpbi->bmiColors[i].rgbGreen || lpbi->bmiColors[i].rgbBlue != lpbi->bmiColors[i].rgbRed )
// 				break;
// 			bUsed[lpbi->bmiColors[i].rgbBlue] = TRUE;
// 		}
// 		if( i< 256 )
// 		{
// 			GlobalUnlock( m_hImg );
// 			return FALSE;
// 		}
// 		for( i=0; i<256; i++)
// 			if( bUsed[i] == FALSE )
// 			{
// 				GlobalUnlock( m_hImg );
// 				return FALSE;
// 			}
// 
// 		rt =  (int)lpbi->bmiColors[0].rgbBlue+(int)lpbi->bmiColors[0].rgbGreen+(int)lpbi->bmiColors[0].rgbRed
//             > (int)lpbi->bmiColors[1].rgbBlue+(int)lpbi->bmiColors[1].rgbGreen+(int)lpbi->bmiColors[1].rgbRed ;
//         GlobalUnlock (m_hImg) ;
//         return rt ;
// 	}
//     else
//         return FALSE ;
// }

/************************************************************************************
/*	Name:		InvertBits
/*	Function:	Invert image bits
/*	Parameter:	None
/*	return:		Image width
/*
/***********************************************************************************/
// int CImage::InvertBits ( )
// {
//     int i, j ;
//     BYTE mask[8] = { 128, 64, 32, 16, 8, 4, 2, 1 } ;
// 
// 	int	x = 0;				
// 	int y = 0;
// 	int w = GetWidth();		
// 	int h = GetHeight();
//     if ( w<0 || h<0 )
//         return -1 ;
//     if ( GetType() == DIB_1BIT )
//     {
//         LPBITMAPINFO lpbi = (LPBITMAPINFO) GlobalLock ( m_hImg ) ;
//         int wi = lpbi->bmiHeader.biWidth ;
//         int hi = lpbi->bmiHeader.biHeight ;
//         if ( x+w > wi ) w=wi-x ;
//         if ( y+h > hi ) h=hi-y ;
//         int wb = GetLineSizeInBytes(lpbi) ;
//         int n = (x+wi-1)/8 - x/8 + 1 ;
//         BYTE *pImg = GetRawImgPtr ( lpbi ) ;
//         for ( i=y ; i<y+h ; i++ )
//         {
//             BYTE *p1, *p2 ;
//             p1 = p2 = pImg + (h-1-i)*wb + x/8 ;
//             for ( j=0 ; j<n ; j++, p1++ )
//                 *p1 = ~(*p1) ;
//             if ( (x%8) != 0 )
//                 for ( j=0 ; j<x%8 ; j++ )
//                     *p2 ^= mask[j] ;
//             p1-- ;
//             if ( ((x+wi-1)%8) != 7 )
//                 for ( j=((x+wi-1)%8)+1 ; j<=7 ; j++ )
//                     *p1 ^= mask[j] ;
//         }
//         GlobalUnlock (m_hImg) ;
//         return 0 ;
//     }
// 	else if ( GetType() == DIB_8BIT )
// 	{
//         LPBITMAPINFO lpbi = (LPBITMAPINFO) GlobalLock ( m_hImg ) ;
//         int wi = lpbi->bmiHeader.biWidth ;
//         int hi = lpbi->bmiHeader.biHeight ;
//         if ( x+w > wi ) w=wi-x ;
//         if ( y+h > hi ) h=hi-y ;
//         BYTE *pImg = GetRawImgPtr ( lpbi ) ;
//         for ( i=y ; i<y+h ; i++ )
// 		{
// 			BYTE *pByte = pImg+wi*(hi-1-i)+x;
// 			int j = 0;
// 			while(j<w)
// 			{
// 				*pByte = 255 - *pByte;
// 				pByte++;
// 				j++;
// 			}
// 		}
// 		GlobalUnlock (m_hImg) ;
//         return 0 ;
// 	}
//     else
//         return -1 ;
// }

/************************************************************************************
/*	Name:		InvertPallete
/*	Function:	Invert image palette
/*	Parameter:	None
/*	return:		0  -- Succeed
/*				-1 -- Failed
/*
/***********************************************************************************/
// int CImage::InvertPallete()
// {
//     if ( GetType() == DIB_1BIT )
//     {
//         LPBITMAPINFO lpbi = (LPBITMAPINFO) GlobalLock ( m_hImg ) ;
//         RGBQUAD tmp = lpbi->bmiColors[0] ;
//         lpbi->bmiColors[0] = lpbi->bmiColors[1] ;
//         lpbi->bmiColors[1] = tmp ;
// 		GlobalUnlock( m_hImg );
// 		return 0;
//     }
// 	else if( GetType() == DIB_8BIT )
// 	{
//         LPBITMAPINFO lpbi = (LPBITMAPINFO) GlobalLock ( m_hImg ) ;
//         RGBQUAD tmp;
// 		for(int i=0; i<128; i++)
// 		{
// 			tmp = lpbi->bmiColors[i] ;
// 			lpbi->bmiColors[i] = lpbi->bmiColors[255-i] ;
// 			lpbi->bmiColors[255-i] = tmp ;
// 		}
// 		GlobalUnlock( m_hImg );
// 		return 0;
// 	}
//     else
//         return -1 ;
// }

/************************************************************************************
/*	Name:		ShowFullImgDIB
/*	Function:	Display whole image
/*	Parameter:	hdc -- HDC handle
/*				xd -- X coordinate offset
/*				yd -- Y coordinate offset
/*	return:		0  -- Succeed
/*				-1 -- Failed
/*
/***********************************************************************************/
// int CImage::ShowFullImgDIB ( HDC hdc, int xd, int yd )
// {
// 	if ( m_hImg != (HGLOBAL)NULL )
//     {
//         LPBITMAPINFO lpbi = (LPBITMAPINFO) GlobalLock ( m_hImg ) ;
//         lpbi->bmiHeader.biXPelsPerMeter = GetDeviceCaps(hdc,HORZRES)*1000L / GetDeviceCaps(hdc,HORZSIZE) ;
//         lpbi->bmiHeader.biYPelsPerMeter = GetDeviceCaps(hdc,VERTRES)*1000L  / GetDeviceCaps(hdc,VERTSIZE) ;
//         SetDIBitsToDevice ( hdc, xd, yd, lpbi->bmiHeader.biWidth, lpbi->bmiHeader.biHeight,
//                                          0, 0, 0, lpbi->bmiHeader.biHeight, GetRawImgPtr(lpbi), lpbi, DIB_RGB_COLORS ) ;
// 	    GlobalUnlock ( m_hImg ) ;
// 	    return 0 ;
//     }
//     else
//         return -1 ;
// }

/************************************************************************************
/*	Name:		ShowStretchImgDIB
/*	Function:	Stretch and display whole image to a specific region
/*	Parameter:	hdc -- HDC handle
/*				xd -- X coordinate offset
/*				yd -- Y coordinate offset
/*				width -- Displayed image width
/*				height -- Displayed image height
/*	return:		0  -- Succeed
/*				-1 -- Failed
/*
/***********************************************************************************/
// int	CImage::ShowStretchImgDIB ( HDC hdc, int xd, int yd, int width, int height)
// {
// 	if ( m_hImg != (HGLOBAL)NULL )
//     {
//         LPBITMAPINFO lpbi = (LPBITMAPINFO) GlobalLock ( m_hImg ) ;
//         lpbi->bmiHeader.biXPelsPerMeter = GetDeviceCaps(hdc,HORZRES)*1000L / GetDeviceCaps(hdc,HORZSIZE) ;
//         lpbi->bmiHeader.biYPelsPerMeter = GetDeviceCaps(hdc,VERTRES)*1000L  / GetDeviceCaps(hdc,VERTSIZE) ;
//         StretchDIBits ( hdc, xd, yd, width, height,
//                  0, 0, lpbi->bmiHeader.biWidth, lpbi->bmiHeader.biHeight, GetRawImgPtr(lpbi), lpbi, DIB_RGB_COLORS, SRCCOPY ) ;
// 	    GlobalUnlock ( m_hImg ) ;
// 	    return 0 ;
//     }
//     else
//         return -1 ;
// }

/************************************************************************************
/*	Name:		SetPalette
/*	Function:	Set image palette
/*	Parameter:	lpbi -- Pointer to BITMAPINFO
/*	return:		0  -- Succeed
/*				-1 -- Failed
/*
/***********************************************************************************/
// int CImage::SetPalette(LPBITMAPINFO lpbi)
// {
//     if ( lpbi != (LPBITMAPINFO)NULL )
//     {
//         if ( lpbi->bmiHeader.biBitCount == 8 )
//             for ( int i=0 ; i<256 ; i++ )
//             {
//                 lpbi->bmiColors[i].rgbRed = (BYTE)i ;
//                 lpbi->bmiColors[i].rgbGreen = (BYTE)i ;
//                 lpbi->bmiColors[i].rgbBlue = (BYTE)i ;
//                 lpbi->bmiColors[i].rgbReserved = 0 ;
//             }
//         else if ( lpbi->bmiHeader.biBitCount == 1 )
//         {
//             lpbi->bmiColors[0].rgbBlue = lpbi->bmiColors[0].rgbGreen = lpbi->bmiColors[0].rgbRed = 255 ;
//             lpbi->bmiColors[1].rgbBlue = lpbi->bmiColors[1].rgbGreen = lpbi->bmiColors[1].rgbRed = 0 ;
//             lpbi->bmiColors[0].rgbReserved = lpbi->bmiColors[1].rgbReserved = 0 ;
//         }
//         return 0 ;
//     }
//     else
//         return -1 ;
// }

/************************************************************************************
/*	Name:		SetPalette
/*	Function:	Set image palette
/*	Parameter:	pColors -- Color array
/*				start -- Start color in the array to set
/*				end -- End color in the array to set
/*	return:		0  -- Succeed
/*				-1 -- Failed
/*
/***********************************************************************************/
// int CImage::SetPalette(RGBQUAD *pColors, int start, int end )
// {
//     if ( GetType() != DIB_8BIT )
//         return -1 ;
//     if ( m_hImg )
//     {
//         LPBITMAPINFO lpbi = (LPBITMAPINFO) GlobalLock ( m_hImg ) ;
//         for ( int i=start ; i<=end ; i++ )
//             if ( i<256 && i>=0 )
//                 lpbi->bmiColors[i] = pColors[i-start] ;
//         GlobalFree ( m_hImg ) ;
//         return 0 ;
//     }
//     else
//         return -1 ;
// }

/******************************************************************************
/*	Name:		BlackPixelsIsInByte
/*	Function:	Initialize array lut[0-255], which calculate the number of
/*				black pixels in a byte
/*	Parameter:	void
/*	Return:		void
/*****************************************************************************/
void CImage::InitBlackPixelsInByte()
{
	for(int i=0;i<256;i++)
	{
		lut[i]=0;
		for(int j=0;j<8;j++)
		{
			if((i>>j)&0x01)
				lut[i]++;
		}
	}
}
/******************************************************************************
/*	Name:		GetPixelsIsInByteMask
/*	Function:	Return array lut, which calculate the number of
/*				black pixels in a byte
/*	Parameter:	void
/*	Return:		lut
/*****************************************************************************/
DSCC_BYTE *CImage::GetBlackPixelsInByteMask()
{
	return lut;
}
/******************************************************************************
/*	Name:		IsColor
/*	Function:	Retrieval the palette to see if the image is color
/*	Parameter:	void
/*	Return:		TRUE -- Color image
/*				FALSE -- Not color image
/*****************************************************************************/
// int CImage::IsColor( )
// {
// 	LPBITMAPINFO lpbi = (LPBITMAPINFO) GlobalLock ( m_hImg ) ;
// 	//If not gray image, return FALSE, do not invert pallete.
// 	int	nPalette;
// 	switch( GetType() )
// 	{
// 	case DIB_1BIT:
// 	case DIB_UNKNOWN:
// 		return FALSE;
// 	case DIB_4BIT:
// 		nPalette = 1<<4;
// 		break;
// 	case DIB_8BIT:
// 		nPalette = 1<<8;
// 		break;
// 	case DIB_16BIT:
// 	case DIB_24BIT:
// 	case DIB_32BIT:
// 		return TRUE;
// 	default:
// 		return FALSE;
// 	}
// 
// 	for( int i=0; i<nPalette; i++ )
// 		if( lpbi->bmiColors[i].rgbBlue != lpbi->bmiColors[i].rgbGreen || lpbi->bmiColors[i].rgbBlue != lpbi->bmiColors[i].rgbRed )
// 			return TRUE;
// 	return FALSE;
// }
