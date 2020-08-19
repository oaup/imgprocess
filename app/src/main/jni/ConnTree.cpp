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
// File Name:		ConnTree.cpp
// File Function:	1. Extract black pixel runlengths in binary and gray images
//					2. Store extract runlengths.
//					3. Extract connected components with runlengths
//
//				Developed by: Yefeng Zheng
//				First Created: June 2001
//			University of Maryland, College Park
//////////////////////////////////////////////////////////////////////


#include "convertMFC.h"
#include <algorithm>
#include <iostream>
#include "malloc.h"
#include "ConnTree.h"

using namespace std;
//IMPLEMENT_SERIAL(CConnTree, CObject, 0)

CConnTree::CConnTree()
{
    m_pNode = (ConnNode *)NULL ;
    m_nMaxNodes = 0 ;

    m_pEmptHead = -1 ;
    m_pColHead  = (int *)NULL ;
    m_pConnComp = (ConnComp *)NULL ;
    m_nTotalConnComps = 0 ;
	m_nLeftMostX = 0x1fffffff;
	m_pFlag = (DSCC_BYTE*)NULL;
	m_pStack = (int*)NULL;
	m_IsHorConn = -1;
	m_nDepth = 0;
}

CConnTree::~CConnTree()
{
    FreeMem() ;
}
/////////////////////////////////////////////////////////////////////
//	Connectionship testing methods:
/////////////////////////////////////////////////////////////////////

/*******************************************************************
*Name:		Initialize
*Function:	1. Set the image region to analysis
*			2. Initialize the runlength store struct
*
********************************************************************/
int CConnTree::Initialize(DSCC_RECT rcRange)
{
    FreeMem() ;

    if ( rcRange.right < rcRange.left || rcRange.bottom < rcRange.top)
        return FALSE ;
    else
    {
 		m_rcRange = rcRange;
		if(m_IsHorConn == 1)
		{
			m_nDepth = rcRange.right - rcRange.left+1 ;
			m_nLeft = rcRange.left;
		}
		else if(m_IsHorConn == 0)
		{
			m_nDepth = rcRange.bottom - rcRange.top+1;
			m_nLeft = rcRange.top;
		}
		else 
			return FALSE;
		m_pColHead = (int *)malloc( m_nDepth*sizeof(int) ) ;
		if ( m_pColHead == (int *)NULL )
			return FALSE ;
        for ( int i=0 ; i<m_nDepth ; i++ )
                m_pColHead[i] = -2 ;			  
    }
    if ( !AllocNewNodes() )
        return FALSE ;
    return TRUE ;
}

/*******************************************************************
*Name:		FreeMem
*Function:	Free memory allocated
*			
********************************************************************/
int CConnTree::FreeMem()
{
    if ( m_nMaxNodes>0 || m_pNode!=(ConnNode *)NULL )
    {
        free ( m_pNode ) ;
        m_pNode = (ConnNode *)NULL ;
        m_nMaxNodes = 0 ;
    }
    if ( m_nDepth>0 || m_pColHead!=(int *)NULL )
    {
        free ( m_pColHead ) ;
        m_pColHead  = (int *)NULL ;
        m_nDepth = 0 ;
        m_pEmptHead = -1 ;
    }
    if ( m_nTotalConnComps>0 || m_pConnComp!=(ConnComp *)NULL )
    {
        free ( m_pConnComp ) ;
        m_pConnComp = (ConnComp *)NULL ;
        m_nTotalConnComps = 0 ;
    }
	if(m_pFlag != NULL)
	{
		free(m_pFlag);
		m_pFlag = NULL;
	}
	if(m_pStack != NULL)
	{
		free(m_pStack);
		m_pStack = NULL;
	}
	return 0;
}

/*******************************************************************
*Name:		IsLeftConnected
*Function:	Test if pNode2 is left connected of PNode1
*			
********************************************************************/
int n1[4000], n2[4000] ;
int CConnTree::IsLeftConnected ( int pNode1, int pNode2 )
{
    int f1, f2, i, j, k ;
    int *p1, *p2, *tp ;
    int x1 = m_pNode[pNode1].v.x & 0x1fffffff ;
    int x2 = m_pNode[pNode2].v.x & 0x1fffffff ;

    if ( x1 > x2 )
    {
        p1 = n1, p2 = n2 ;
        p1[0] = pNode1 ; 
        f1 = 1 ; 
        for ( k=x1 ; k>x2 ; k-- )
        {
            f2 = 0 ;
            p2[f2] = -1 ;
            for ( i=0 ; i<f1 ; i++ )
            {
                int total = m_pNode[p1[i]].nLtTotal ;
                int pNode = m_pNode[p1[i]].pLeft ;
                j = 0 ;
                if ( f2 > 0 )
                    for ( ; j<total ; j++, pNode=m_pNode[pNode].pUnder )
                        if ( pNode != p2[f2-1] )
                            break ;
                for ( ; j<total ; j++, pNode=m_pNode[pNode].pUnder )
                    p2[f2++] = pNode ;
            }
            if ( f2 == 0 )
                return FALSE ;
            else
            {
                f1 = f2 ;
                tp = p1 ;
                p1 = p2 ;
                p2 = tp ;
            }
        }
        for ( i=0 ; i<f1 ; i++ )
            if ( p1[i] == pNode2 )
                return TRUE ;
        return FALSE ;
    }
    else
        return FALSE ;
}

/*******************************************************************
*Name:		IsRightConnected
*Function:	Test if pNode2 is right connected of PNode1
*			
********************************************************************/
int CConnTree::IsRightConnected ( int pNode1, int pNode2 )
{
    int f1, f2, i, j, k ;
    int *p1, *p2, *tp ;
    int x1 = m_pNode[pNode1].v.x & 0x1fffffff ;
    int x2 = m_pNode[pNode2].v.x & 0x1fffffff ;

    if ( x1 < x2 )
    {
        p1 = n1, p2 = n2 ;
        p1[0] = pNode1 ;
        f1 = 1 ; 
        for ( k=x1 ; k<x2 ; k++ )
        {
            f2 = 0 ;
            p2[f2] = -1 ;
            for ( i=0 ; i<f1 ; i++ )
            {
                int total = m_pNode[p1[i]].nRtTotal ;
                int pNode = m_pNode[p1[i]].pRight ;
                j = 0 ;
                if ( f2 > 0 )
                    for ( ; j<total ; j++, pNode=m_pNode[pNode].pUnder )
                        if ( pNode != p2[f2-1] )
                            break ;
                for ( ; j<total ; j++, pNode=m_pNode[pNode].pUnder )
                    p2[f2++] = pNode ;
            }
            if ( f2 == 0 )
                return FALSE ;
            else
            {
                f1 = f2 ;
                tp = p1 ;
                p1 = p2 ;
                p2 = tp ;
            }
        }
        for ( i=0 ; i<f1 ; i++ )
            if ( p1[i] == pNode2 )
                return TRUE ;
        return FALSE ;
    }
    else
        return FALSE ;
}

/*******************************************************************
*Name:		IsConnected
*Function:	Test if pNode2 and PNode1 are connected
*			
********************************************************************/
int CConnTree::IsConnected ( int pNode1, int pNode2 )
{
    int x1 = m_pNode[pNode1].v.x & 0x1fffffff ;
    int x2 = m_pNode[pNode2].v.x & 0x1fffffff ;

    if ( x1 < x2 )
    {
        if ( IsRightConnected(pNode1, pNode2) )
            return TRUE ;
        else
            return IsLeftConnected(pNode2, pNode1) ; 
    }
    else if ( x2 < x1 )
    {
        if ( IsLeftConnected(pNode1, pNode2) )
            return TRUE ;
        else
            return IsRightConnected(pNode2, pNode1) ; 
    }
    else
        return FALSE ;
}

/*******************************************************************
*Name:		IsLeftConnected2
*Function:	Test if pNode2 is left connected of PNode1 within the
*			indicated region
*			
********************************************************************/
int CConnTree::IsLeftConnected2 ( int pNode1, int pNode2, int & width, int &gap )
{
    int f1, f2, i, j, k ;
    int *p1, *p2, *tp ;
    int x1 = m_pNode[pNode1].v.x & 0x1fffffff ;
    int x2 = m_pNode[pNode2].v.x & 0x1fffffff ;
	int top, down;
	width = 1;
	gap = x1-x2;
    if ( x1 > x2 )
    {
        p1 = n1, p2 = n2 ;
        p1[0] = pNode1 ; 
        f1 = 1 ; 
        for ( k=x1 ; k>x2 ; k-- )
        {
            f2 = 0 ;
            p2[f2] = -1 ;
			top = 0x1fffffff;		down = -1;		
            for ( i=0 ; i<f1 ; i++ )
            {
                top = min(m_pNode[p1[i]].v.ys, top);
				down = max(m_pNode[p1[i]].v.ye, down);
				gap = min((m_pNode[p1[i]].v.x & 0x1fffffff)-x2, gap);

				int total = m_pNode[p1[i]].nLtTotal ;
                int pNode = m_pNode[p1[i]].pLeft ;
                j = 0 ;
                if ( f2 > 0 )
                    for ( ; j<total ; j++, pNode=m_pNode[pNode].pUnder )
                        if ( pNode != p2[f2-1] )
                            break ;
                for ( ; j<total ; j++, pNode=m_pNode[pNode].pUnder )
                    p2[f2++] = pNode ;
            }
            if ( f2 == 0 )		break;
            else
            {
                f1 = f2 ;
                tp = p1 ;
                p1 = p2 ;
                p2 = tp ;
            }
			if(down != -1)		width = max(width, down-top+1);
        }
        for ( i=0 ; i<f1 ; i++ )
            if ( p1[i] == pNode2 )
                return 1 ;
//---------------------------------------------------------------------------------
		int x3 = x2+gap-1;
		p1[0] = pNode2 ; 
        f1 = 1 ; 
		for(k=x2; k<=x3; k++)
		{
		    f2 = 0 ;
            p2[f2] = -1 ;
			top = 0x1fffffff;		down = -1;		
            for ( i=0 ; i<f1 ; i++ )
            {
                top = min(m_pNode[p1[i]].v.ys, top);
				down = max(m_pNode[p1[i]].v.ye, down);
				gap = min(x3-(m_pNode[p1[i]].v.x & 0x1fffffff), gap);

				int total = m_pNode[p1[i]].nRtTotal ;
                int pNode = m_pNode[p1[i]].pRight ;
                j = 0 ;
                if ( f2 > 0 )
                    for ( ; j<total ; j++, pNode=m_pNode[pNode].pUnder )
                        if ( pNode != p2[f2-1] )
                            break ;
                for ( ; j<total ; j++, pNode=m_pNode[pNode].pUnder )
                    p2[f2++] = pNode ;
            }
            if ( f2 == 0 )
                return  0;
            else
            {
                f1 = f2 ;
                tp = p1 ;
                p1 = p2 ;
                p2 = tp ;
            }
			if(down != -1)		width = max(width, down-top+1);
		}
		return 0;
	}
    else
	{	gap = 0;
		width = 1;
        return 0 ;
	}
}

/*******************************************************************
*Name:		IsRightConnected2
*Function:	Test if pNode2 is right connected of PNode1 within the
*			indicated region
*			
********************************************************************/
int CConnTree::IsRightConnected2 ( int pNode1, int pNode2, int&width, int&gap )
{
    int f1, f2, i, j, k ;
    int *p1, *p2, *tp ;
    int x1 = m_pNode[pNode1].v.x & 0x1fffffff ;
    int x2 = m_pNode[pNode2].v.x & 0x1fffffff ;
	int top, down;
	width = 1;
	gap = x2-x1;
    if ( x1 < x2 )
    {
        p1 = n1, p2 = n2 ;
        p1[0] = pNode1 ;
        f1 = 1 ; 
		for ( k=x1 ; k<x2 ; k++ )
        {
            f2 = 0 ;
            p2[f2] = -1 ;
			top = 0x1fffffff;	down = -1;
            for ( i=0 ; i<f1 ; i++ )
            {
				top = min(top, m_pNode[p1[i]].v.ys);
				down = max(down, m_pNode[p1[i]].v.ye);
				gap = min(gap, x2-(m_pNode[p1[i]].v.x & 0x1fffffff));
                int total = m_pNode[p1[i]].nRtTotal ;
                int pNode = m_pNode[p1[i]].pRight ;
                j = 0 ;
                if ( f2 > 0 )
                    for ( ; j<total ; j++, pNode=m_pNode[pNode].pUnder )
                        if ( pNode != p2[f2-1] )
                            break ;
                for ( ; j<total ; j++, pNode=m_pNode[pNode].pUnder )
                    p2[f2++] = pNode ;
            }
            if ( f2 == 0 )		break ;
            else
            {
                f1 = f2 ;
                tp = p1 ;
                p1 = p2 ;
                p2 = tp ;
            }
			if(down > 0)	width = max(width, down-top+1); 
        }
        for ( i=0 ; i<f1 ; i++ )
            if ( p1[i] == pNode2 )
                return 1 ;
//--------------------------------------------------------------------------
		int x3 = x2-gap+1;
	    p1[0] = pNode2 ;
        f1 = 1 ; 
		for(k=x2; k>=x3; k--)
        {
            f2 = 0 ;
            p2[f2] = -1 ;
			top = 0x1fffffff;	down = -1;
            for ( i=0 ; i<f1 ; i++ )
            {
				top = min(top, m_pNode[p1[i]].v.ys);
				down = max(down, m_pNode[p1[i]].v.ye);
				gap = min(gap, (m_pNode[p1[i]].v.x & 0x1fffffff) - x3);
                int total = m_pNode[p1[i]].nLtTotal ;
                int pNode = m_pNode[p1[i]].pLeft ;
                j = 0 ;
                if ( f2 > 0 )
                    for ( ; j<total ; j++, pNode=m_pNode[pNode].pUnder )
                        if ( pNode != p2[f2-1] )
                            break ;
                for ( ; j<total ; j++, pNode=m_pNode[pNode].pUnder )
                    p2[f2++] = pNode ;
            }
            if ( f2 == 0 )
                return 0 ;
            else
            {
                f1 = f2 ;
                tp = p1 ;
                p1 = p2 ;
                p2 = tp ;
            }
			if(down > 0)	width = max(width, down-top+1); 
		}		
        return 0;
    }
    else
	{	gap = 0;
		width = 1;
        return 0 ;
	}
}

/*******************************************************************
*Name:		IsConnected2
*Function:	Test if pNode2 and PNode1 are connected within the
*			indicated region
*			
********************************************************************/
int CConnTree::IsConnected2 ( int pNode1, int pNode2, int &width, int & gap )
{
    int x1 = m_pNode[pNode1].v.x & 0x1fffffff ;
    int x2 = m_pNode[pNode2].v.x & 0x1fffffff ;
    if ( x1 < x2 )
        return IsRightConnected2(pNode1, pNode2, width, gap);
    else if ( x2 < x1 )
        return IsLeftConnected2(pNode1, pNode2, width, gap);
    else
        return 0 ;
}

/////////////////////////////////////////////////////////////////////
//	Methods for Creating and storing runlengths
////////////////////////////////////////////////////////////////////

/*******************************************************************
*Name:		AllocNewNodes
*Function:	Allocate new nodes to store runlengths
*			
********************************************************************/
int CConnTree::AllocNewNodes()
{
    ConnNode *pNewNodes = (ConnNode *)realloc (m_pNode, (m_nMaxNodes+5000)*sizeof(ConnNode) ) ;
    if ( pNewNodes == (ConnNode *)NULL )
        return FALSE ;
    else
    {
        m_pNode = pNewNodes ;
		int i = m_nMaxNodes;
        for (  ; i<m_nMaxNodes+5000 ; i++ )
        {
            m_pNode[i].pAbove = i-1 ;
            m_pNode[i].pUnder = i+1 ;
            m_pNode[i].nRtTotal = 0 ;
            m_pNode[i].pRight = -1 ;
            m_pNode[i].nLtTotal = 0 ;
            m_pNode[i].pLeft = -1 ;
        }
        m_pNode[i-1].pUnder = -1 ;
        m_pNode[m_nMaxNodes].pAbove = -1 ;
        if ( m_pEmptHead >= 0 )
        {
            int pNode = m_pEmptHead ;
            while ( m_pNode[pNode].pUnder >= 0 )
                pNode = m_pNode[pNode].pUnder ;
            m_pNode[pNode].pUnder = m_nMaxNodes ;
            m_pNode[m_nMaxNodes].pAbove = pNode ;
        }
        else
        {
            m_pEmptHead = 0 ;
        }
        m_nMaxNodes += 5000 ;
        return TRUE ;
    }
}

/*******************************************************************
*Name:		MakeLeftConn
*Function:	Adjust the left connectionship of the runlength to
*			other runlengths
*			
********************************************************************/
int CConnTree::MakeLeftConn ( int nColNum )
{
    if ( nColNum<0 || nColNum>=m_nDepth )
        return -1 ;

    int p, pLeft ;

    if ( (p=m_pColHead[nColNum]) < 0 )
        return -2 ;

    if ( nColNum == 0 )
    {
        while ( p >= 0 )
        {
            m_pNode[p].nLtTotal = 0 ;
            m_pNode[p].pLeft = -1 ;
            p = m_pNode[p].pUnder ;
        }
    }
    else if ( (pLeft=m_pColHead[nColNum-1]) >= 0 )
    {
        while ( p >= 0 )
        {
            int bFirstIn = TRUE ;
            while ( pLeft >= 0 )
            {
                if ( m_pNode[pLeft].v.yve >= m_pNode[p].v.yvs )
                {
                    if ( m_pNode[pLeft].v.yvs <= m_pNode[p].v.yve )
                    {
                        if ( bFirstIn )
                        {
                            m_pNode[p].nLtTotal = 0 ;
                            m_pNode[p].pLeft = pLeft ;
                            bFirstIn = FALSE ;
                        }
                        m_pNode[p].nLtTotal++ ;
                    }
                    if ( m_pNode[pLeft].v.yve >= m_pNode[p].v.yve )
                        break ;
                }
                pLeft = m_pNode[pLeft].pUnder ;
            }
            p = m_pNode[p].pUnder ;
        }
    }
    return 0 ;
}

/*******************************************************************
*Name:		MakeRightConn
*Function:	Adjust the right connectionship of the runlength to 
*			other runlengths
*			
********************************************************************/
int CConnTree::MakeRightConn ( int nColNum )
{
    if ( nColNum<0 || nColNum>=m_nDepth )
        return -1 ;

    int p, pRight ;

    if ( (p=m_pColHead[nColNum]) < 0 )
        return -2 ;

    if ( nColNum == m_nDepth-1 )
    {
        while ( p >= 0 )
        {
            m_pNode[p].nRtTotal = 0 ;
            m_pNode[p].pRight = -1 ;
            p = m_pNode[p].pUnder ;
        }
    }
    else if ( (pRight=m_pColHead[nColNum+1]) >= 0 )
    {
        while ( p >= 0 )
        {
            int bFirstIn = TRUE ;
            while ( pRight >= 0 )
            {
                if ( m_pNode[pRight].v.yve >= m_pNode[p].v.yvs )
                {
                    if ( m_pNode[pRight].v.yvs <= m_pNode[p].v.yve )
                    {
                        if ( bFirstIn )
                        {
                            m_pNode[p].nRtTotal = 0 ;
                            m_pNode[p].pRight = pRight ;
                            bFirstIn = FALSE ;
                        }
                        m_pNode[p].nRtTotal++ ;
                    }
                    if ( m_pNode[pRight].v.yve >= m_pNode[p].v.yve )
                        break ;
                }
                pRight = m_pNode[pRight].pUnder ;
            }
            p = m_pNode[p].pUnder ;
        }
    }
    return 0 ;
}

/*******************************************************************
*Name:		AddNewCol
*Function:	Add new column of runlengths
*			
********************************************************************/
int CConnTree::AddNewCol ( LineValley *v, int valleys, int nColNum )
{
	if ( nColNum<m_nLeft || nColNum>=m_nDepth+m_nLeft )
        return -1 ;
    
    if ( valleys == 0 )
        m_pColHead[nColNum-m_nLeft] = -1 ;
    else
    {
		if(nColNum < m_nLeftMostX)	m_nLeftMostX = nColNum;
		nColNum-=m_nLeft;
		m_pColHead[nColNum] = m_pEmptHead ;
        CopyValley ( v, valleys ) ;
        MakeLeftConn ( nColNum ) ;
        MakeRightConn ( nColNum-1 ) ;
        MakeRightConn ( nColNum ) ;
        MakeLeftConn ( nColNum+1 ) ;
    }
    return 0 ;
}


/*******************************************************************
*Name:		CopyValley
*Function:	Copy the runlength struct
*			
********************************************************************/
int CConnTree::CopyValley ( LineValley *v, int valleys )
{
    if ( valleys <= 0 )
        return -1 ;

    int pLast ;
    for ( int i=0 ; i<valleys ; i++ )
    {
        m_pNode[m_pEmptHead].v = v[i] ;
        m_pNode[m_pEmptHead].nLtTotal = 0 ;
        m_pNode[m_pEmptHead].pLeft = -1 ;
        m_pNode[m_pEmptHead].nRtTotal = 0 ;
        m_pNode[m_pEmptHead].pRight = -1 ;
        if ( m_pNode[m_pEmptHead].pUnder < 0 )
            AllocNewNodes() ;
        pLast = m_pEmptHead ;
        m_pEmptHead = m_pNode[m_pEmptHead].pUnder ;
    }
    m_pNode[pLast].pUnder = -1 ;
    m_pNode[m_pEmptHead].pAbove = -1 ;
    return 0 ;
}
/*******************************************************************
*Name:		GetLeftMostX
*Function:	Return the most left ordinate 
*			
********************************************************************/
int CConnTree::GetLeftMostX()
{
    return m_nLeftMostX;
}

/////////////////////////////////////////////////////////////////////
// Methods to extract connected components
/////////////////////////////////////////////////////////////////////

/*******************************************************************
*Name:		GetAllConnComps
*Function:	Extract all connected components in the image
*			
********************************************************************/
int CConnTree::GetAllConnComps()
{
    if(m_pEmptHead <= 0)	return -1;
	if ( m_nTotalConnComps>0 && m_pConnComp != (ConnComp *)NULL )
    {
        free ( m_pConnComp ) ;
        m_pConnComp = (ConnComp *)NULL ;
        m_nTotalConnComps = 0 ;
    }
	if(m_pFlag != NULL)
		free(m_pFlag);
	if ( (m_pFlag=(DSCC_BYTE *)malloc(m_pEmptHead*sizeof(DSCC_BYTE))) == (DSCC_BYTE *)NULL )
			return -1 ;
	memset ( m_pFlag, 0, m_pEmptHead*sizeof(DSCC_BYTE) ) ;

    int i;
    int nCnt = 0 ;
    int nMax = 1000 ;
	m_pConnComp = (ConnComp *)realloc( m_pConnComp, nMax*sizeof(ConnComp) ) ;
	if(m_pConnComp == NULL)  return -1;

    i = 0 ;
    while ( i < m_pEmptHead )
    {
        if ( !m_pFlag[i] )
        {
            if ( GetConnComp(m_pConnComp[nCnt++], i, m_pFlag) != 0 )
                return -2 ;
            if ( nCnt == nMax )
            {
                nMax += 1000 ;
                m_pConnComp = (ConnComp *)realloc( m_pConnComp, nMax*sizeof(ConnComp) ) ;
				if(m_pConnComp == NULL)  return -1;
            }
        }
        i++ ;
    }
    m_nTotalConnComps = nCnt ;
    m_pConnComp = (ConnComp *)realloc( m_pConnComp, nCnt*sizeof(ConnComp) ) ;
	if(m_pConnComp == NULL)   return -1;

    return 0 ;
}


/*******************************************************************
*Name:		GetConnComp
*Function:	Get connected component containing the indicate runlength
*			Called outside CConnTree class
*
********************************************************************/
int CConnTree::GetConnComp ( ConnComp &cc, int nInitial)
{
    cc.nInitialNode = nInitial ;
    cc.rcBound.left = cc.rcBound.right = m_pNode[nInitial].v.x & 0x1fffffff ;
    cc.rcBound.top = m_pNode[nInitial].v.yvs ;
    cc.rcBound.bottom = m_pNode[nInitial].v.yve ;
	cc.nPixelsNum = 0;
	DSCC_BYTE *pFlag;

	if ( (pFlag=(DSCC_BYTE *)malloc(m_pEmptHead*sizeof(DSCC_BYTE))) == (DSCC_BYTE *)NULL )
			return -1 ;
	memset ( pFlag, 0, m_pEmptHead*sizeof(DSCC_BYTE) ) ;
	if(m_pStack == NULL)
		if ( (m_pStack=(int *)malloc(m_pEmptHead*sizeof(int))) == (int *)NULL )
		{	free(pFlag);
			return -1;
		}

    int i, p, pThis, pNode, x  ;

    p=0 ;
    m_pStack[p++] = nInitial ;
    pFlag[nInitial] = 1 ;
    while ( p )
    {
        pThis = m_pStack[p-1] ;
        for ( i=0, pNode=m_pNode[pThis].pLeft ; i<m_pNode[pThis].nLtTotal ; i++, pNode=m_pNode[pNode].pUnder )
            if ( !pFlag[pNode] )
                goto push ;
        for ( i=0, pNode=m_pNode[pThis].pRight ; i<m_pNode[pThis].nRtTotal ; i++, pNode=m_pNode[pNode].pUnder )
            if ( !pFlag[pNode] )
                goto push ;
        p-- ; 
		continue ;
push:;
        x = m_pNode[pNode].v.x  & 0x1fffffff ;
        if ( x < cc.rcBound.left )
            cc.rcBound.left = x ;
        else if ( x > cc.rcBound.right )
            cc.rcBound.right = x ;
        if ( m_pNode[pNode].v.yvs < cc.rcBound.top )
            cc.rcBound.top = m_pNode[pNode].v.yvs ;
        if ( m_pNode[pNode].v.yve > cc.rcBound.bottom )
            cc.rcBound.bottom = m_pNode[pNode].v.yve ;
		cc.nPixelsNum += m_pNode[pNode].v.ye - m_pNode[pNode].v.ys+1;  
        pFlag[pNode] = 1 ;
        m_pStack[p++] = pNode ;
    }
	free(pFlag);
    return 0 ;
}

/*******************************************************************
*Name:		GetConnComp
*Function:	Get connected component containing the indicate runlength.
*			Called by CConnTree::GetAllConnComp()			
*
********************************************************************/
int CConnTree::GetConnComp ( ConnComp &cc, int nInitial, DSCC_BYTE *pFlag)
{
    cc.nInitialNode = nInitial ;
    cc.rcBound.left = cc.rcBound.right = m_pNode[nInitial].v.x & 0x1fffffff ;
    cc.rcBound.top = m_pNode[nInitial].v.yvs ;
    cc.rcBound.bottom = m_pNode[nInitial].v.yve ;
	cc.nPixelsNum = 0;

	if(m_pStack == NULL)
		if ( (m_pStack=(int *)malloc(m_pEmptHead*sizeof(int))) == (int *)NULL )
	        return -1;

    int i, p, pThis, pNode, x  ;

    p=0 ;
    m_pStack[p++] = nInitial ;
    pFlag[nInitial] = 1 ;
    while ( p )
    {
        pThis = m_pStack[p-1] ;
        for ( i=0, pNode=m_pNode[pThis].pLeft ; i<m_pNode[pThis].nLtTotal ; i++, pNode=m_pNode[pNode].pUnder )
            if ( !m_pFlag[pNode] )
                goto push ;
        for ( i=0, pNode=m_pNode[pThis].pRight ; i<m_pNode[pThis].nRtTotal ; i++, pNode=m_pNode[pNode].pUnder )
            if ( !pFlag[pNode] )
                goto push ;
        p-- ; continue ;
push:;
        x = m_pNode[pNode].v.x  & 0x1fffffff ;
        if ( x < cc.rcBound.left )
            cc.rcBound.left = x ;
        else if ( x > cc.rcBound.right )
            cc.rcBound.right = x ;
        if ( m_pNode[pNode].v.yvs < cc.rcBound.top )
            cc.rcBound.top = m_pNode[pNode].v.yvs ;
        if ( m_pNode[pNode].v.yve > cc.rcBound.bottom )
            cc.rcBound.bottom = m_pNode[pNode].v.yve ;
		cc.nPixelsNum += m_pNode[pNode].v.ye - m_pNode[pNode].v.ys+1;  
        pFlag[pNode] = 1 ;
        m_pStack[p++] = pNode ;
    }
    return 0 ;
}

/*******************************************************************
*Name:		GetConnComp
*Function:	Get connected component containing the indicate runlength.
*			Return the total runlengths contained and the runlength
*			array.
*			Called outside CConnTree class
*
********************************************************************/
int *CConnTree::GetConnComp ( ConnComp &cc, int *_nTotalNodes )
{
    int i, p, pThis, pNode, nTotalNodes, nCntMax, *pCCNode;

    *_nTotalNodes = 0 ;
	if(m_pFlag == NULL)
		if ( (m_pFlag=(DSCC_BYTE *)malloc(m_pEmptHead*sizeof(DSCC_BYTE))) == (DSCC_BYTE *)NULL )
			return (int *)NULL ;
    memset ( m_pFlag, 0, m_pEmptHead*sizeof(DSCC_BYTE) ) ;
	if(m_pStack == NULL)
		if ( (m_pStack=(int *)malloc(m_pEmptHead*sizeof(int))) == (int *)NULL )
	        return (int *)NULL ;
    
	nCntMax = 100 ;
    if ( (pCCNode=(int *)malloc(nCntMax*sizeof(int))) == (int *)NULL )
        return (int *)NULL ;
    
    p=0 ;
    nTotalNodes = 0 ;
    m_pStack[p++] = pCCNode[nTotalNodes++] = cc.nInitialNode ;
    m_pFlag[cc.nInitialNode] = 1 ;
    while ( p )
    {
        pThis = m_pStack[p-1] ;
        for ( i=0, pNode=m_pNode[pThis].pLeft ; i<m_pNode[pThis].nLtTotal ; i++, pNode=m_pNode[pNode].pUnder )
            if ( !m_pFlag[pNode] )
                goto push ;
        for ( i=0, pNode=m_pNode[pThis].pRight ; i<m_pNode[pThis].nRtTotal ; i++, pNode=m_pNode[pNode].pUnder )
            if ( !m_pFlag[pNode] )
                goto push ;
        p--; continue ;
push:;
        m_pFlag[pNode] = 1 ;
        m_pStack[p++] = pCCNode[nTotalNodes++] = pNode ;
        if ( nTotalNodes == nCntMax )
        {
            nCntMax += 100 ;
            if ( (pCCNode=(int *)realloc(pCCNode, nCntMax*sizeof(int))) == (int *)NULL )
                return (int *)NULL ;
        }
    }
    if ( (pCCNode=(int *)realloc(pCCNode, nTotalNodes*sizeof(int))) == (int *)NULL )
        return (int *)NULL ;
    *_nTotalNodes = nTotalNodes ;
    return pCCNode ;
}