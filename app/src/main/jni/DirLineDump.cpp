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
// File Name:		DirLineDump.cpp
// File Function:	Dump CHAIN, CHAINS, LINE, CONNCOMP, etc. Used for debuging.
//
//				Developed by: Yefeng Zheng
//				First Created: June 2001
//			University of Maryland, College Park
//////////////////////////////////////////////////////////////////////////


#include "DirLine.h"


/*************************************************************
/*	Name:		DumpNodes
/*	Function:	Dump NODE to image file for debugging
/*
/************************************************************/
// int CDirLine::DumpNodes ( CStringA FileName, int w, int h)
// {
//     int i ;
//     int ImgSize = (w+31)/32*32/8*h ;
//     CBitmap bmp, *pOldBMP ;
//     if ( bmp.CreateBitmap ( w, h, 1, 1, NULL ) )
//     {
//         CDC dc ;
//         dc.CreateCompatibleDC ( &CClientDC( AfxGetMainWnd() ) ) ;
// 
//         CPen  *pOldPen, Pen(PS_SOLID, 1, RGB(0,0,0)) ;
//         pOldPen = dc.SelectObject ( &Pen ) ;
// 
//         if ( (pOldBMP=dc.SelectObject(&bmp)) != NULL )
//         {
//             BYTE *p = (BYTE *)malloc(ImgSize) ;
//             if ( p )
//             {
//                 memset ( p, 255, ImgSize ) ;
//                 bmp.SetBitmapBits( ImgSize, p ) ;
//                 free ( p ) ;
// 				for(int k=0; k<=m_nCurTree; k++)
// 				{
// 					for ( i=0 ; i<m_pTree[k]->m_nDepth ; i++ )
// 					{
// 						int pNode = m_pTree[k]->m_pColHead[i] ;
// 						while ( pNode >= 0 )
// 						{
// 							LineValley v = m_pTree[k]->m_pNode[pNode].v ;
// 							int vx = v.x & 0x1fffffff;
// 							if( m_bIsHorLine )
// 							{
// 								dc.MoveTo ( vx, v.yvs ) ;
// 								dc.LineTo ( vx, v.yve+1 ) ;
// 							}
// 							else
// 							{
// 								dc.MoveTo ( v.yvs, vx );
// 								dc.LineTo ( v.yve+1, vx );
// 							}
// 							pNode = m_pTree[k]->m_pNode[pNode].pUnder ;
// 						}
// 					}
// 				}
//                 CImage DIB ;
//                 if ( DIB.Initialize(dc.GetSafeHdc(),bmp) )
//                 {
//                     dc.SelectObject ( pOldBMP ) ;
//                     //DIB.Write (FileName) ;
//                 }
//             }
//         }
//         dc.SelectObject ( pOldPen ) ;
//     }
//     return 0 ;
// }

/*************************************************************
/*	Name:		DumpChain
/*	Function:	Dump CHAIN to image file for debugging
/*
/************************************************************/
// int CDirLine::DumpChain ( CStringA  FileName, int w, int h)
// {
//     int i, j, nTree ;
// 	LineValley v;
//     CBitmap bmp, *pOldBMP ;
// 	nTree = 0;
// 	int ImgSize = (w+31)/32*32/8*h;
// 
//     if ( bmp.CreateBitmap ( w, h, 1, 1, NULL ) )
//     {
//         CDC dc ;
//         dc.CreateCompatibleDC ( &CClientDC( AfxGetMainWnd() ) ) ;
// 
//         CPen  *pOldPen, Pen(PS_SOLID, 1, RGB(0,0,0)) ;
//         pOldPen = dc.SelectObject ( &Pen ) ;
// 
//         if ( (pOldBMP=dc.SelectObject(&bmp)) != NULL )
//         {
//             BYTE *p = (BYTE *)malloc(ImgSize) ;
//             if ( p )
//             {
//                 memset ( p, 255, ImgSize ) ;
//                 bmp.SetBitmapBits( ImgSize, p ) ;
//                 free ( p ) ;
//                 int pNode ;
//                 for ( i=0 ; i<m_nChain ; i++ )
//                 {
//                     if(i>=m_nChainStart[nTree])		nTree++;
// 					pNode = m_pChain[i].pHead ;
//                     for ( j=0 ; j<m_pChain[i].Len ; j++ )
//                     {
//                         v = m_pTree[nTree]->m_pNode[pNode].v ;
//                         int x = v.x & 0x1fffffff ;
//                         if(m_bIsHorLine)
// 						{	dc.MoveTo ( x, v.yvs ) ;
// 							dc.LineTo ( x, v.yve+1 ) ;
// 						}
// 						else
// 						{	dc.MoveTo ( v.yvs, x ) ;
// 							dc.LineTo ( v.yve+1,x ) ;
// 						}
//                         pNode = m_pTree[nTree]->m_pNode[pNode].pRight ;
// 						if(pNode < 0)	break;
//                     }
//                 }
//                 CImage DIB ;
//                 if ( DIB.Initialize(dc.GetSafeHdc(),bmp) )
//                 {
//                     dc.SelectObject ( pOldBMP ) ;
//                     //DIB.Write (FileName) ;
//                 }
//             }
//         }
//         dc.SelectObject ( pOldPen ) ;
//     }
//     return 0 ;
// }

/*************************************************************
/*	Name:		DumpChain
/*	Function:	Dump CHAIN to image file for debugging
/*
/************************************************************/
// int CDirLine::DumpChain ( CImage &Img, int w, int h)
// {
//     int i, j, nTree ;
// 	LineValley v;
//     CBitmap bmp, *pOldBMP ;
// 	nTree = 0;
// 	int ImgSize = (w+31)/32*32/8*h;
// 
//     if ( bmp.CreateBitmap ( w, h, 1, 1, NULL ) )
//     {
//         CDC dc ;
//         dc.CreateCompatibleDC ( &CClientDC( AfxGetMainWnd() ) ) ;
// 
//         CPen  *pOldPen, Pen(PS_SOLID, 1, RGB(0,0,0)) ;
//         pOldPen = dc.SelectObject ( &Pen ) ;
// 
//         if ( (pOldBMP=dc.SelectObject(&bmp)) != NULL )
//         {
//             BYTE *p = (BYTE *)malloc(ImgSize) ;
//             if ( p )
//             {
//                 memset ( p, 255, ImgSize ) ;
//                 bmp.SetBitmapBits( ImgSize, p ) ;
//                 free ( p ) ;
//                 int pNode ;
//                 for ( i=0 ; i<m_nChain ; i++ )
//                 {
//                     if(i>=m_nChainStart[nTree])		nTree++;
// 					pNode = m_pChain[i].pHead ;
//                     for ( j=0 ; j<m_pChain[i].Len ; j++ )
//                     {
//                         v = m_pTree[nTree]->m_pNode[pNode].v ;
//                         int x = v.x & 0x1fffffff ;
//                         if(m_bIsHorLine)
// 						{	dc.MoveTo ( x, v.yvs ) ;
// 							dc.LineTo ( x, v.yve+1 ) ;
// 						}
// 						else
// 						{	dc.MoveTo ( v.yvs, x ) ;
// 							dc.LineTo ( v.yve+1,x ) ;
// 						}
//                         pNode = m_pTree[nTree]->m_pNode[pNode].pRight ;
// 						if(pNode < 0)	break;
//                     }
//                 }
//                 if ( Img.Initialize(dc.GetSafeHdc(),bmp) == FALSE )
// 					return -1;
//             }
//         }
//         dc.SelectObject ( pOldPen ) ;
//     }
//     return 0 ;
// }

/*************************************************************
/*	Name:		DumpChainData
/*	Function:	Dump CHAIN to text file for debugging
/*
/************************************************************/
int CDirLine::DumpChainData ( string  FileName)
{
	FILE *fp;
	fp= fopen(FileName.c_str(), "wt");
	if(fp== NULL)	return -1;
	for(int i=0; i<m_nChain; i++)
	{
		if(m_bIsHorLine)
			fprintf(fp, "(%d, %d)--(%d, %d)  Len=%d r=%f\n", m_pChain[i].xs, m_pChain[i].fYs, m_pChain[i].xe, m_pChain[i].fYe, m_pChain[i].Len, m_pChain[i].r);
		else
			fprintf(fp, "(%d, %d)--(%d, %d)  Len=%d r=%f\n", m_pChain[i].fYs,m_pChain[i].xs,  m_pChain[i].fYe, m_pChain[i].xe, m_pChain[i].Len, m_pChain[i].r);
	}
	fclose(fp);
	return 0 ;
}

/*************************************************************
/*	Name:		DumpChainsLine
/*	Function:	Dump line of CHAINS to image file for debugging
/*	Parameter:	FileName -- Output image file name
/*				w		 -- Image width
/*				h		 -- Image height
/*	Return:		0  -- Succeed
/*				-1 -- Failed
/************************************************************/
// int CDirLine::DumpChainsLine ( CString FileName, int w, int h )
// {
// 	int i;
//     w = (w+31)/32*32 ;
//     int ImgSize = w/8*h ;
//     CBitmap bmp, *pOldBMP ;
//     if ( bmp.CreateBitmap ( w, h, 1, 1, NULL ) )
//     {
//         CDC dc ;
//         dc.CreateCompatibleDC ( &CClientDC( AfxGetMainWnd() ) ) ;
// 
//         CPen  *pOldPen, Pen(PS_SOLID, 1, RGB(0,0,0)) ;
//         pOldPen = dc.SelectObject ( &Pen ) ;
// 
//         if ( (pOldBMP=dc.SelectObject(&bmp)) != NULL )
//         {
//             BYTE *p = (BYTE *)malloc(ImgSize) ;
//             if ( p )
//             {
//                 memset ( p, 255, ImgSize ) ;
//                 bmp.SetBitmapBits( ImgSize, p ) ;
//                 free ( p ) ;
// 
//                 for ( i=0 ; i<m_nChains ; i++ )
//                 {
// 					CHAINS Chains = m_pChains[i] ;
//                     if(m_bIsHorLine)
// 					{	dc.MoveTo ( Chains.xs, Chains.fYs ) ;
// 						dc.LineTo ( Chains.xe, Chains.fYe ) ;
// 					}
// 					else
// 					{	dc.MoveTo ( Chains.fYs, Chains.xs) ;
// 						dc.LineTo ( Chains.fYe, Chains.xe) ;
// 					}
//                 }
//                 CImage DIB ;
//                 if ( DIB.Initialize(dc.GetSafeHdc(),bmp) )
//                 {
//                     dc.SelectObject ( pOldBMP ) ;
//                     DIB.Write (FileName) ;
//                 }
//             }
//         }
//         dc.SelectObject ( pOldPen ) ;
//     }
//     return 0 ;
// }

/*************************************************************
/*	Name:		DumpChains
/*	Function:	Dump CHAINS to image file for debugging
/*	Parameter:	FileName -- Output image file name
/*				w		 -- Image width
/*				h		 -- Image height
/*	Return:		0  -- Succeed
/*				-1 -- Failed
/************************************************************/
// int CDirLine::DumpChains ( CStringA FileName, int w, int h )
// {
// 	int i, j, k;
//     int ImgSize = ((w+31)/32*32)/8*h ;
//     CBitmap bmp, *pOldBMP ;
// 
// 	LineValley v;
// 	int nTree = 0;
//     if ( bmp.CreateBitmap ( w, h, 1, 1, NULL ) )
//     {
//         CDC dc ;
//         dc.CreateCompatibleDC ( &CClientDC( AfxGetMainWnd() ) ) ;
// 
//         CPen  *pOldPen, Pen(PS_SOLID, 1, RGB(0,0,0)) ;
//         pOldPen = dc.SelectObject ( &Pen ) ;
// 
//         if ( (pOldBMP=dc.SelectObject(&bmp)) != NULL )
//         {
//             BYTE *p = (BYTE *)malloc(ImgSize) ;
//             if ( p )
//             {
//                 memset ( p, 255, ImgSize ) ;
//                 bmp.SetBitmapBits( ImgSize, p ) ;
//                 free ( p ) ;
// 
//                 for ( i=0 ; i<m_nChains ; i++ )
//                 {
// 					k = m_pChains[i].pHead ;
// 					while ( k >= 0 )
// 					{	
// 						if( k>=m_nChainStart[nTree] )		nTree++;
// 						int pNode = m_pChain[k].pHead ;
// 						for ( j=0 ; j<m_pChain[k].Len ; j++ )
// 						{
// 							v = m_pTree[nTree]->m_pNode[pNode].v ;
// 							int x = v.x & 0x1fffffff ;
// 							if(m_bIsHorLine)
// 							{	
// 								dc.MoveTo ( x, v.yvs ) ;
// 								dc.LineTo ( x, v.yve+1 ) ;
// 							}
// 							else
// 							{	
// 								dc.MoveTo ( v.yvs, x ) ;
// 								dc.LineTo ( v.yve+1,x ) ;
// 							}
// 							pNode = m_pTree[nTree]->m_pNode[pNode].pRight ;
// 							if(pNode < 0)	break;
// 						}
// 						k = m_pChain[k].pRight ;
// 					}
//                 }
//                 
//                 CImage DIB ;
//                 if ( DIB.Initialize(dc.GetSafeHdc(),bmp) )
//                 {
//                     dc.SelectObject ( pOldBMP ) ;
//                     //DIB.Write (FileName) ;
//                 }
//             }
//         }
//         dc.SelectObject ( pOldPen ) ;
//     }
//     return 0 ;
// }

/*************************************************************
/*	Name:		DumpChainsData
/*	Function:	Dump CHAINS to text file for debugging
/*
/************************************************************/
int CDirLine::DumpChainsData ( string  FileName)
{
	FILE *fp;
	fp= fopen(FileName.c_str(), "wt");
	if(fp== NULL)	return -1;
	for(int i=0; i<m_nChains; i++)
	{
		if(m_pChains[i].Num < 30)	continue;
		ChainsQuality(m_pChains[i]);
		if(m_bIsHorLine)
			fprintf(fp, "(%d, %d)--(%d, %d)  Q=%f\n", m_pChains[i].xs, m_pChains[i].fYs, m_pChains[i].xe, m_pChains[i].fYe, m_pChains[i].Q);
		else
			fprintf(fp, "(%d, %d)--(%d, %d)  Q=%f\n", m_pChains[i].fYs, m_pChains[i].xs,  m_pChains[i].fYe, m_pChains[i].xe, m_pChains[i].Q);
	}
	fclose(fp);
	return 0 ;
}

/*************************************************************
/*	Name:		DumpLine
/*	Function:	Dump LINE to image file for debugging
/*
/************************************************************/
// int CDirLine::DumpLine ( CString FileName, int w, int h )
// {
//     int i ;
//     w = (w+31)/32*32 ;
//     int ImgSize = w/8*h ;
//     CBitmap bmp, *pOldBMP ;
//     if ( bmp.CreateBitmap ( w, h, 1, 1, NULL ) )
//     {
//         CDC dc ;
//         dc.CreateCompatibleDC ( &CClientDC( AfxGetMainWnd() ) ) ;
//         CBrush Brush ( RGB(0,0,0) ) ;
//         if ( (pOldBMP=dc.SelectObject(&bmp)) != NULL )
//         {
//             BYTE *p = (BYTE *)malloc(ImgSize) ;
//             if ( p )
//             {
//                 memset ( p, 255, ImgSize ) ;
//                 bmp.SetBitmapBits( ImgSize, p ) ;
//                 free ( p ) ;
//                 CPen  *pOldPen, *pPen ;
//                 for ( i=0 ; i<m_nLine ; i++ )
//                 {
//                     int PenWidth = (int)(m_pLine[i].Width+0.5) ;
//                     pPen = new CPen(PS_SOLID, PenWidth, RGB(0,0,0)) ;
//                     pOldPen = dc.SelectObject ( pPen ) ;
//                     dc.MoveTo ( m_pLine[i].StPnt.x, m_pLine[i].StPnt.y ) ;
//                     dc.LineTo ( m_pLine[i].EdPnt.x, m_pLine[i].EdPnt.y ) ;
//                     dc.SelectObject ( pOldPen ) ;
//                     delete pPen ;
//                     CRect r ;
//                     r.top = m_pLine[i].StPnt.y - PenWidth/2-5 ;
//                     r.bottom = m_pLine[i].StPnt.y + PenWidth/2+5 ;
//                     r.left = m_pLine[i].StPnt.x - PenWidth/2-5 ;
//                     r.right = m_pLine[i].StPnt.x + PenWidth/2+5 ;
//                     dc.FillRect(&r, &Brush) ;
//                     r.top = m_pLine[i].EdPnt.y - PenWidth/2-5 ;
//                     r.bottom = m_pLine[i].EdPnt.y + PenWidth/2+5 ;
//                     r.left = m_pLine[i].EdPnt.x - PenWidth/2-5 ;
//                     r.right = m_pLine[i].EdPnt.x + PenWidth/2+5 ;
//                     dc.FillRect(&r, &Brush) ;
//                 }
//                 CImage DIB ;
//                 if ( DIB.Initialize(dc.GetSafeHdc(),bmp) )
//                 {
//                     dc.SelectObject ( pOldBMP ) ;
//                     DIB.Write (FileName) ;
//                 }
//             }
//         }
//     }
//     return 0 ;
// }

/*************************************************************
/*	Name:		DumpLineData
/*	Function:	Dump LINE to text file for debugging
/*
/************************************************************/
int CDirLine::DumpLineData(string FileName)
{
	FILE *fp;
	fp= fopen(FileName.c_str(), "wt");
	if(fp== NULL)	return -1;
	for(int i=0; i<m_nLine; i++)
		fprintf(fp, "Line:%2d  (%4d, %4d)------>(%4d, %4d) Angle=%5.2f Q=%5.2f\n", i, m_pLine[i].StPnt.x, m_pLine[i].StPnt.y, m_pLine[i].EdPnt.x, m_pLine[i].EdPnt.y, m_pLine[i].Angle, m_pLine[i].Q);
	fclose(fp);
	return 0;
}

/*********************************************************************
/*	Name:		DumpConnComp
/*	Function:	Dump Connected Components to image file for debugging
/*
/********************************************************************/
// int CDirLine::DumpConnComp ( CString  FileName, int w, int h)
// {
//     int i, j, x, nTree, nTotal, nInitialNode ;
// 	int *pConnComp;
// 	LineValley v;
//     CBitmap bmp, *pOldBMP ;
// 	nTree = 0;
//     w = (w+31)/32*32 ;
//     int ImgSize = w/8*h ;
// 
//     if ( bmp.CreateBitmap ( w, h, 1, 1, NULL ) )
//     {
//         CDC dc ;
//         dc.CreateCompatibleDC ( &CClientDC( AfxGetMainWnd() ) ) ;
// 
//         CPen  *pOldPen, Pen(PS_SOLID, 1, RGB(0,0,0)) ;
//         pOldPen = dc.SelectObject ( &Pen ) ;
// 
//         if ( (pOldBMP=dc.SelectObject(&bmp)) != NULL )
//         {
//             BYTE *p = (BYTE *)malloc(ImgSize) ;
//             if ( p )
//             {
//                 memset ( p, 255, ImgSize ) ;
//                 bmp.SetBitmapBits( ImgSize, p ) ;
//                 free ( p ) ;
//                 for(nTree=0; nTree<=m_nCurTree; nTree++)
// 				{
// 					for ( i=0 ; i<m_pTree[nTree]->m_nTotalConnComps ; i++ )
// 					{
// 						nInitialNode = m_pTree[nTree]->m_pConnComp[i].nInitialNode;
// 						if(m_pTree[nTree]->m_pNode[nInitialNode].v.x &0x80000000)	continue;
// 						pConnComp = m_pTree[nTree]->GetConnComp(m_pTree[nTree]->m_pConnComp[i], &nTotal);
// 						for(j=0; j<nTotal; j++)
//                         {
// 							v = m_pTree[nTree]->m_pNode[pConnComp[j]].v ;
// 							x = v.x & 0x1fffffff ;
// 							if(m_bIsHorLine)
// 							{	dc.MoveTo ( x, v.yvs ) ;
// 								dc.LineTo ( x, v.yve+1 ) ;
// 							}
// 							else
// 							{	dc.MoveTo ( v.yvs, x ) ;
// 								dc.LineTo ( v.yve+1,x ) ;
// 							}
// 						}
// 					}
//                 }
//                 CImage DIB ;
//                 if ( DIB.Initialize(dc.GetSafeHdc(),bmp) )
//                 {
//                     dc.SelectObject ( pOldBMP ) ;
//                     DIB.Write (FileName) ;
//                 }
//             }
//         }
//         dc.SelectObject ( pOldPen ) ;
//     }
// 	FILE *fp;
// 	if(m_bIsHorLine)
// 		fp = fopen("E:\\Result\\HConnComp.dat", "wt");
// 	else
// 		fp = fopen("E:\\Result\\VConnComp.dat", "wt");
// 	for(nTree=0; nTree<=m_nCurTree; nTree++)
// 	{
// 		for ( i=0 ; i<m_pTree[nTree]->m_nTotalConnComps ; i++ )
// 		{
// 			nInitialNode = m_pTree[nTree]->m_pConnComp[i].nInitialNode;
// 			RECT rcBound = m_pTree[nTree]->m_pConnComp[i].rcBound;
// 			fprintf(fp, "L=%d   R=%d   T=%d   B=%d   Ratio=%f\n", rcBound.left, rcBound.right, rcBound.top, rcBound.bottom,
// 					m_pTree[nTree]->m_pConnComp[i].nPixelsNum*1.0/(rcBound.right-rcBound.left+1)/(rcBound.bottom-rcBound.top+1));
// 		}
// 	}
// 	fclose(fp);
//     return 0 ;
// } 

