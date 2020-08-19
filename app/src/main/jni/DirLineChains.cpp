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
// File Name:		DirLineChains.cpp
// File Function:	Build CHAINS from Directional Single Connected Chain
//
//				Developed by: Yefeng Zheng
//				First Create: June 2001
//			University of Maryland, College Park
//////////////////////////////////////////////////////////////////////////


#include "DirLine.h"
#include <algorithm>


#define	QUEUE_DEPTH			3
#define	PARAM_CHARWIDTH		40

/***************************************************************************
/*	Name:		PixelsBetween
/*	Function:	Number of black pixels between a CHAIN and a CHAINS
/*	Parameter:	Chains -- CHAINS
/*				Chain  -- CHAIN
/*				MaxWidth -- Maximum white gap, changed after calling
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/***************************************************************************/
int CDirLine::PixelsBetween ( CHAINS &Chains, CHAIN &Chain, int &MaxWidth )
{
	int xs, xe, gap, pix=0 ;
    if ( Chains.xe < Chain.xs )
    {
        gap = Chain.xs-Chains.xe-1 ;
        xs = Chains.xe+1;
        xe = Chain.xs-1;
    }
    else if ( Chains.xs > Chain.xe )
    {
        gap = Chains.xs-Chain.xe-1 ;
        xs = Chain.xe+1;
        xe = Chains.xs-1;
    }
    else
        return 0 ;

    int i,pNode, pHead ;
    double w, ys, ye ;

	pHead = xs - m_pTree[m_nCurTree]->m_nLeft;
	w = min(4, (int)Chains.Width);
	MaxWidth = -1;
    for ( i=xs ; i<=xe ; i++ )
    {
        ys = fYofChains ( Chains, i ) - w ;
		ye = ys+2*w;
		if ( (pNode=m_pTree[m_nCurTree]->m_pColHead[pHead++]) >= 0 )
            while ( pNode >= 0 )
            {
				LineValley	v = m_pTree[m_nCurTree]->m_pNode[pNode].v;
				if ( v.yve >= ys )
                {
                    if ( v.yvs <= ye )
					{	pix++;		
						MaxWidth = max(MaxWidth, v.yve-v.yvs+1);
					}
                    else
                        break ;
                }
                pNode = m_pTree[m_nCurTree]->m_pNode[pNode].pUnder ;
            }
    }
    return	pix ;
}

/***************************************************************************
/*	Name:		RightMerge
/*	Function:	Merge DSCC on the right side
/*	Parameter:	
/*	Return:		0  -- Succeed
/*				-1 -- Error
/***************************************************************************/

int CDirLine::RightMerge ( INTCHAIN *pHeadXTbl, int *pHeadXIndex, int &SeedChains )
{
    int i, j, k, m, p, xl, xe ;
    CHAINS Chains = m_pChains[SeedChains] ;
    xe = Chains.xe ;
    xl =m_pTree[m_nCurTree]->GetLeftMostX() ;
	int T = min(2*PARAM_CHARWIDTH, max(2*(Chains.xe-Chains.xs+1), PARAM_CHARWIDTH ));

    double Dist, DistQueue[QUEUE_DEPTH] ;
    int BestChain[QUEUE_DEPTH] ;//QUEUE_DEPTH=3
    for ( i=0 ; i<QUEUE_DEPTH ; i++ )
	{   
		DistQueue[i]=1e100 ;
		BestChain[i]=-1 ;
	}

	double y=Chains.SumY/Chains.Num;
	double dy=15;
    for ( i=xe+1-xl ; i<min(xe+T-xl, m_pTree[m_nCurTree]->m_nDepth) ; i++ )
    {
        if ( i+xl-xe > DistQueue[QUEUE_DEPTH-1] )
            break ;
        p = pHeadXIndex[i] ;//find the best suitable chain ready for merge
        while ( p >= 0 )
        {
            j = pHeadXTbl[p].n ;
            if ( m_pChain[j].pLeft == -1 && m_pChain[j].pRight == -1)//2000-3-15
            {
                if( fabs(m_pChain[j].fYs - fYofChains(Chains, m_pChain[j].xs))<dy )//??
				{
					Dist =  ChainDistance ( SeedChains, j, DistQueue[QUEUE_DEPTH-1] ) ;
					for ( k=0 ; k<QUEUE_DEPTH ; k++ )
					{	//Insert the Dist into DistQueue array
						if ( Dist < DistQueue[k] )
						{
							for ( m=k+1 ; m<QUEUE_DEPTH ; m++ )
								DistQueue[m]=DistQueue[m-1], BestChain[m]=BestChain[m-1] ;
							DistQueue[k] = Dist ;
							BestChain[k] = j ;
							break ;
						}
                    }
                }
            }
            p = pHeadXTbl[p].pNext ;//Get next chain
        }
    }
    for ( k=0 ; k<QUEUE_DEPTH ; k++ )
    {
        if ( DistQueue[k] > 1e99 )
            break ;
		if (m_pChain[BestChain[k]].Width > 8 && m_pChain[BestChain[k]].Width > 2*Chains.Width)	continue;
        int gap = m_pChain[BestChain[k]].xs-Chains.xe-1 ;
		double MErr = sqrt ( (DistQueue[k]-gap)/2 ) ;
        double MWidth = Chains.Width;
		if( m_pChain[BestChain[k]].Len < 20 && MErr >= max((double)3, MWidth))		continue;
		if( m_pChain[BestChain[k]].Len >= 20&& MErr >= 1.5*MWidth)	continue;

		int width, gap2;
		int pix = PixelsBetween(Chains, m_pChain[BestChain[k]], width);
		gap2 = gap-pix;
		if (gap <= m_Param.MaxGap)//2015.3.19  lizhao
		{
			if (gap2 <= 1)
				return AddChain(SeedChains, BestChain[k]);
			if (pix <= 1 && gap < m_Param.MaxGap)
				return	AddChain(SeedChains, BestChain[k]);
			if ((width >= 0 && width < 2 * Chains.Width && gap2 < m_Param.MaxGap) || (width >= 2 * Chains.Width && gap < m_Param.MaxGap / 2))
				return AddChain(SeedChains, BestChain[k]);
		}
	}
		
	return -1 ;
}

/***************************************************************************
/*	Name:		LeftMerge
/*	Function:	Merge DSCC on the left side
/*	Parameter:	
/*	Return:		0  -- Succeed
/*				-1 -- Error
/***************************************************************************/
int CDirLine::LeftMerge ( INTCHAIN *pTailXTbl, int *pTailXIndex, int &SeedChains )
{
    int i, j, k, m, p, xl, xs ;
    CHAINS Chains = m_pChains[SeedChains] ;
    xs = Chains.xs ;
    xl =m_pTree[m_nCurTree]->GetLeftMostX() ;

	int T;
	T = min(2*PARAM_CHARWIDTH, max(2*(Chains.xe-Chains.xs+1), PARAM_CHARWIDTH ));
    
	double Dist, DistQueue[QUEUE_DEPTH] ;
    int BestChain[QUEUE_DEPTH] ;
    for ( i=0 ; i<QUEUE_DEPTH ; i++ )
	{   
		DistQueue[i]=1e100 ;
		BestChain[i]=-1 ;
	}
	double y=Chains.SumY/Chains.Num;
	double dy=15;
    for ( i=xs-1-xl ; i>=max(xs-T-xl, 0) ; i-- )
    {
        if ( xs-(i+xl) > DistQueue[QUEUE_DEPTH-1] )
            break ;
        p = pTailXIndex[i] ;
        while ( p >= 0 )
        {
            j = pTailXTbl[p].n ;
            if ( m_pChain[j].pRight == -1 && m_pChain[j].pLeft == -1)//2000-3-15
            {
                if( fabs(m_pChain[j].fYe - fYofChains(Chains, m_pChain[j].xe))<dy )//??
				{
					Dist =  ChainDistance ( SeedChains, j, DistQueue[QUEUE_DEPTH-1] ) ;
					for ( k=0 ; k<QUEUE_DEPTH ; k++ )
					{
						if ( Dist < DistQueue[k] )
						{
							for ( m=k+1 ; m<QUEUE_DEPTH ; m++ )
								DistQueue[m]=DistQueue[m-1], BestChain[m]=BestChain[m-1] ;
							DistQueue[k] = Dist ;
							BestChain[k] = j ;
							break ;
						}
                    }
                }
            }
            p = pTailXTbl[p].pNext ;
        }
    }
    for ( k=0 ; k<QUEUE_DEPTH ; k++ )
    {
        if ( DistQueue[k] > 1e99 ) 
            break ;
		if ( m_pChain[BestChain[k]].Width > 8 && m_pChain[BestChain[k]].Width > 2*Chains.Width)	continue;
        int gap = Chains.xs-m_pChain[BestChain[k]].xe-1 ;
        double MErr = sqrt ( DistQueue[k] - gap )  ;
        double MWidth = Chains.Width;
		if( m_pChain[BestChain[k]].Len < 20 && MErr >= std::max((double)3, MWidth))		continue;
		if( m_pChain[BestChain[k]].Len >= 20&& MErr >= 1.5*MWidth)	continue;

		int width, gap2;
		int pix = PixelsBetween(Chains, m_pChain[BestChain[k]], width);
		gap2 = gap-pix;
		if (gap < m_Param.MaxGap)//2015.3.19 lizhao  
		{
			if (gap2 <= 1)
				return AddChain(SeedChains, BestChain[k]);
			if (pix <= 1 && gap < m_Param.MaxGap)
				return	AddChain(SeedChains, BestChain[k]);
			if ((width >= 0 && width < 2 * Chains.Width && gap2 < m_Param.MaxGap) || (width >= 2 * Chains.Width && gap < m_Param.MaxGap / 2))
				return AddChain(SeedChains, BestChain[k]);
		}
	}
	return -1 ;
}

/***************************************************************************
/*	Name:		MergeChains
/*	Function:	Merge DSCC to get CHAINS
/*	Parameter:	void
/*	Return:		0  -- Succeed
/*				-1 -- Error
/***************************************************************************/
int CDirLine::MergeChains()
{
	if( m_nChain == m_nOldChain )
		return -1;
	INTCHAIN *pHeadXTbl = (INTCHAIN *)malloc ( sizeof(INTCHAIN) * (m_nChain-m_nOldChain) ) ;
	if(pHeadXTbl == NULL)	return -1;
    int *pHeadXIndex = (int *)malloc ( sizeof(int) * m_pTree[m_nCurTree]->m_nDepth ) ;
	if(pHeadXIndex == NULL)	return -1;
    SortChainHead ( pHeadXTbl, pHeadXIndex ) ;//Speed up the search process

    INTCHAIN *pTailXTbl = (INTCHAIN *)malloc ( sizeof(INTCHAIN) * (m_nChain-m_nOldChain) ) ;
	if(pTailXTbl == NULL)	return -1;
    int *pTailXIndex = (int *)malloc ( sizeof(int) * m_pTree[m_nCurTree]->m_nDepth ) ;
	if(pTailXIndex == NULL)	return -1;
    SortChainTail ( pTailXTbl, pTailXIndex ) ;//Speed up the search process

    INTCHAIN *pLenTbl = (INTCHAIN *)malloc ( sizeof(INTCHAIN) * (m_nChain-m_nOldChain) ) ;
	if(pLenTbl == NULL)	return -1;
    int *pLenIndex = (int *)malloc ( sizeof(int) * (m_pTree[m_nCurTree]->m_nDepth+1) ) ;
	if(pLenIndex == NULL)	return -1;
    SortChainLen ( m_pTree[m_nCurTree]->m_nDepth, pLenTbl, pLenIndex ) ;//Sort Seed Chain

    int i, j ;
	int nOldChains = m_nChains;
    static int MaxCnt = 1000 ;
	if(m_pChains == NULL)
		m_pChains = (CHAINS *)malloc ( sizeof(CHAINS)*MaxCnt ) ;

	if(m_pChains == NULL)
		return -1;
   
    for ( i=m_pTree[m_nCurTree]->m_nDepth ; i>=0 ; i-- )
    {   
        int p = pLenIndex[i] ;
        while ( p >= 0 )
        {
			j = pLenTbl[p].n ;
            CHAIN Chain = m_pChain[j] ;
            if ( Chain.pRight==-1 && Chain.pLeft==-1 && Chain.Num>=Chain.Len/2 )
            {
                    InitChains ( m_pChains[m_nChains], Chain, j ) ;
                    int SeedChains =  m_nChains++ ;
                    if ( m_nChains == MaxCnt )
                    {
                        MaxCnt += 1000 ;
						m_pChains = (CHAINS *)realloc ( m_pChains, sizeof(CHAINS)*MaxCnt ) ;
						if(m_pChains == NULL )  	return -1;
                    } 
                  while (  RightMerge ( pHeadXTbl, pHeadXIndex, SeedChains ) == 0 ) ;
                  while (  LeftMerge (pTailXTbl, pTailXIndex, SeedChains ) == 0 ) ;

				  m_pChains[SeedChains].Angle = GetAngle( DSCC_CPoint( m_pChains[SeedChains].xs, m_pChains[SeedChains].fYs), 
														  DSCC_CPoint( m_pChains[SeedChains].xe, m_pChains[SeedChains].fYe) );
				  
				  if(m_pChain[j].pLeft<0 && m_pChain[j].pRight<0)
					  if(m_pChain[j].xe-m_pChain[j].xs<30)
							m_nChains--; 
					  else
					  {    m_pChain[j].pLeft = -2;			m_pChain[j].pRight = -2;}
            }
            p = pLenTbl[p].pNext ;
        }
    }
   DSCC_RECT   rcRange = m_pTree[m_nCurTree]->m_rcRange;
   for(i=nOldChains; i<m_nChains; i++)//Delete chains in overlaped area which are extracted twice
   {
		if( m_nCurTree != 0 )
		{
			if(m_bIsHorLine && m_nCurTree> 0 && m_pChains[i].fYs < rcRange.top+10 && m_pChains[i].fYe < rcRange.top+10)
			{	DeleteChains(i);
				i--;
			}
			else if(!m_bIsHorLine && m_nCurTree> 0 && m_pChains[i].fYs < rcRange.left+10 && m_pChains[i].fYe < rcRange.left+10)
			{	DeleteChains(i);
				i--;
			}
		}
		if( m_nCurTree != m_nStrip-1 )
		{
			if(m_bIsHorLine && rcRange.bottom < m_rcBoundRange.bottom-20 && m_pChains[i].fYs > rcRange.bottom-10 && m_pChains[i].fYe > rcRange.bottom-10)
			{	DeleteChains(i);
				i--;
			}
			else if(!m_bIsHorLine && rcRange.right < m_rcBoundRange.right-20 && m_pChains[i].fYs  > rcRange.right-10 && m_pChains[i].fYe > rcRange.right-10)
			{	DeleteChains(i);
				i--;
			}
		}
   }
	free ( pLenIndex ) ;
    free ( pLenTbl ) ;
    free ( pTailXIndex ) ;
    free ( pTailXTbl ) ;
    free ( pHeadXIndex ) ;
    free ( pHeadXTbl ) ;
    return 0 ;
}

/***************************************************************************
/*	Name:		MergeChains
/*	Function:	Merge two CHAINSs
/*	Parameter:	Chains1 -- CHAINS 1, changed after calling
/*				Chains2 -- CHAINS 2
/*	Return:		0  -- Succeed
/*				-1 -- Error
/***************************************************************************/
int CDirLine::MergeChains ( CHAINS &Chains1, CHAINS &Chains2 )
{
    if ( Chains1.xe < Chains2.xe )
    {
        if(m_pChain[Chains1.pTail].pLeft == Chains2.pHead || 
			m_pChain[Chains2.pHead].pRight == Chains1.pTail)
				return -1;//2000-3-14
		m_pChain[Chains1.pTail].pRight = Chains2.pHead ;
        m_pChain[Chains2.pHead].pLeft = Chains1.pTail ;
        Chains1.pTail = Chains2.pTail ;
        Chains1.xe = Chains2.xe ;
    }
    else if ( Chains1.xs > Chains2.xs )
    {
        if(m_pChain[Chains1.pHead].pRight == Chains2.pTail ||
			m_pChain[Chains2.pTail].pLeft == Chains1.pHead )
				return -1;//2000-3-14
		m_pChain[Chains1.pHead].pLeft = Chains2.pTail ;
        m_pChain[Chains2.pTail].pRight = Chains1.pHead ;
        Chains1.pHead = Chains2.pHead ;
        Chains1.xs = Chains2.xs ;
    }
    else
        return -1 ;
    
    Chains1.SumX	+= Chains2.SumX ;
    Chains1.SumY	+= Chains2.SumY ;
    Chains1.SumXX	+= Chains2.SumXX ;
    Chains1.SumXY	+= Chains2.SumXY ;
	Chains1.Width	= (Chains1.Width * Chains1.Num + Chains2.Width*Chains2.Num) / (Chains1.Num + Chains2.Num);
    Chains1.Num		+= Chains2.Num ;
	Chains1.NumPixel+= Chains2.NumPixel;
	Chains1.fYs		= (int)fYofChains(Chains1, Chains1.xs);
	Chains1.fYe		= (int)fYofChains(Chains1, Chains1.xe);
	return 0 ;
}


/***************************************************************************
/*	Name:		SortChainHead
/*	Function:	Sort DSCC according to head to accelerate the merging process
/*	Parameter:	
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/***************************************************************************/
int CDirLine::SortChainHead ( INTCHAIN *pTbl, int *pIndex )
{
    int i, n, xl, x ;

    n = m_pTree[m_nCurTree]->m_nDepth ;
    for ( i=0 ; i<n ; i++ )
        pIndex[i] = -1 ;
    xl = m_pTree[m_nCurTree]->GetLeftMostX() ;
    for ( i=0; i<m_nChain-m_nOldChain ; i++ )
    {
        x = m_pTree[m_nCurTree]->m_pNode[m_pChain[i+m_nOldChain].pHead].v.x & 0x1fffffff ;
        n = x - xl ;
        pTbl[i].n = i+m_nOldChain ;
        pTbl[i].pNext = pIndex[n] ;
        pIndex[n] = i ;
    }
    return 0 ;
}

/***************************************************************************
/*	Name:		SortChainTail
/*	Function:	Sort DSCC according to tail to accelerate the merging process
/*	Parameter:	
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/***************************************************************************/
int CDirLine::SortChainTail ( INTCHAIN *pTbl, int *pIndex )
{
    int i, n, xl, x ;

    n = m_pTree[m_nCurTree]->m_nDepth ;
    for ( i=0 ; i<n ; i++ )
        pIndex[i] = -1 ;
    xl = m_pTree[m_nCurTree]->GetLeftMostX() ;
    for ( i=0 ; i<m_nChain-m_nOldChain ; i++ )
    {
        x = m_pTree[m_nCurTree]->m_pNode[m_pChain[i+m_nOldChain].pTail].v.x & 0x1fffffff ;
        n = x - xl ;
        pTbl[i].n = i+m_nOldChain ;
        pTbl[i].pNext = pIndex[n] ;
        pIndex[n] = i ;
    }
    return 0 ;
}

/***************************************************************************
/*	Name:		SortChainLen
/*	Function:	Sort DSCC according to length to accelerate the merging process
/*	Parameter:	
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/***************************************************************************/
int CDirLine::SortChainLen ( int MaxLen, INTCHAIN *pTbl, int *pIndex )
{
    int i, n ;

    for ( i=0 ; i<=MaxLen ; i++ )
        pIndex[i] = -1 ;
    for ( i=0 ; i<m_nChain-m_nOldChain ; i++ )
        if ( (n=m_pChain[i+m_nOldChain].xe-m_pChain[i+m_nOldChain].xs+1) <= MaxLen )
        {
            pTbl[i].n = i+m_nOldChain ;
            pTbl[i].pNext = pIndex[n] ;
            pIndex[n] = i ;
        }
    return 0 ;
}


/***************************************************************************
/*	Name:		AddChain
/*	Function:	Add a CHAIN to a CHAINS
/*	Parameter:	nChains -- CHAINS NO.
/*				nChain -- CHAIN NO.
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/***************************************************************************/
int CDirLine::AddChain ( int &nChains, int nChain )
{
	if( nChains < 0 || nChains >= m_nChains || nChain < 0 || nChain >= m_nChain )
		return -1;
	if ( m_pChain[nChain].pLeft<0 && m_pChain[nChain].pRight<0 )
    {
        CHAINS tmpChains ;
        InitChains ( tmpChains, m_pChain[nChain], nChain ) ;
        MergeChains ( m_pChains[nChains], tmpChains ) ;
        return 0 ;
    }
    else if ( m_pChain[nChain].pLeft<0 || m_pChain[nChain].pRight<0 )
    {
        int nDelChains = InWhichChains ( nChain ) ;
        if ( nDelChains >= 0 )
        {
            MergeChains ( m_pChains[nChains], m_pChains[nDelChains] ) ;
            return 0 ;
        }
        else
            return -2 ;
    }
    else
        return -1 ;
}

/***************************************************************************
/*	Name:		DeleteChains
/*	Function:	Delete a CHAINS
/*	Parameter:	nDelChains -- CHAINS NO to delete
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/***************************************************************************/
int CDirLine::DeleteChains ( int nDelChains )
{
    if ( nDelChains>=0 && nDelChains<m_nChains )
    {
        for ( int i=nDelChains ; i<m_nChains-1 ; i++ )
            m_pChains[i] = m_pChains[i+1] ;
        m_nChains-- ;
        return 0 ;
    }
    else
        return -1 ;
}

/***************************************************************************
/*	Name:		ChainDistance
/*	Function:	Distance of two CHAINS
/*	Parameter:	Chains1 -- CHAINS 1
/*				Chains2 -- CHAINS 2
/*				Max -- ???
/*	Return:		Distance of two CHAINS
/*
/***************************************************************************/
double CDirLine::ChainDistance ( CHAINS &Chains1, CHAINS &Chains2, double Max )
{
    int Len1 =  Chains1.xe-Chains1.xs+1 ;
    int Len2 =  Chains2.xe-Chains2.xs+1 ;
	LineValley  v;

    CHAINS comChains ;
	comChains = Chains1;	
    int num, MaxNum, pNode, nChain, gap ;
    double x, Distance, dy, MaxDy ;

    MaxNum = Chains2.xe-Chains2.xs+1 ;
    Distance = 0 ;
    num = 0 ;
    Max *=  MaxNum ;
    MaxDy = PARAM_CHARWIDTH ;

    if ( Chains2.xe < Chains1.xs )   // at the left side
    {
        x = Chains2.xe ;
        gap = Chains1.xs-Chains2.xe-1 ;
        nChain = Chains2.pTail ;
        while ( Chains2.xe-x<MaxNum && nChain>=0 )
        {
            pNode = m_pChain[nChain].pTail ;
            while ( Chains2.xe-x<MaxNum && pNode>=0 )
            {
                v = m_pTree[m_nCurTree]->m_pNode[pNode].v ;
                if ( ! (v.x & 0xc0000000 ) )
                {
                    x = v.x & 0x1fffffff ;
                    dy = (v.yvs+v.yve)/2-fYofChains(comChains, x) ;
                    Distance += dy*dy;
                    if ( Distance > Max )
                        return 1.7e308 ;
                    num++ ;
                }
                if ( pNode != m_pChain[nChain].pHead )
                    pNode = m_pTree[m_nCurTree]->m_pNode[pNode].pLeft ;
                else
                    break ;
            }
            if ( nChain != Chains2.pHead )
                nChain = m_pChain[nChain].pLeft ;
            else
                break ;
        }
    }
    else if  ( Chains2.xs > Chains1.xe )  // at the right side
    {
        x = Chains2.xs ;
        gap = Chains2.xs-Chains1.xe-1 ;
        nChain = Chains2.pHead ;
        while ( x-Chains2.xs<MaxNum && nChain>=0 )
        {
            pNode = m_pChain[nChain].pHead ;
            while ( x-Chains2.xs<MaxNum && pNode>=0 )
            {
                v = m_pTree[m_nCurTree]->m_pNode[pNode].v ;
                if ( ! (v.x & 0xc0000000 ) )
                {
                    x = v.x & 0x1fffffff ;
                    dy = (v.yvs+v.yve)/2-fYofChains(comChains, x) ;
                    Distance += dy*dy ;
                    if ( Distance > Max )
                        return 1.7e308 ;
                    num++ ;
                }
                if ( pNode != m_pChain[nChain].pTail )
                    pNode = m_pTree[m_nCurTree]->m_pNode[pNode].pRight ;
                else
                    break ;
            }
            if ( nChain != Chains2.pTail )
                nChain = m_pChain[nChain].pRight ;
            else
                break ;
        }
    }
    else
        return 1.7e308 ;

    if ( num > 0 )
        return Distance/num + gap ;
    else
        return 1.7e308 ;
}

/***************************************************************************
/*	Name:		ChainDistance
/*	Function:	Distance of a CHAIN to a CHAINS
/*	Parameter:	nChains -- CHAINS NO
/*				nChain -- CHAIN  NO
/*				Max -- ???
/*	Return:		Distance of a CHAIN to a CHAINS
/*
/***************************************************************************/
double CDirLine::ChainDistance ( int nChains, int nChain, double Max )
{
	if ( m_pChain[nChain].pLeft<0 && m_pChain[nChain].pRight<0 )
    {
        CHAINS tmpChains ;
        InitChains ( tmpChains, m_pChain[nChain], nChain ) ;
        return ChainDistance ( m_pChains[nChains], tmpChains, Max ) ;
    }
    else
        return ChainDistance ( m_pChains[nChains], m_pChains[InWhichChains(nChain)], Max ) ;
}

/***************************************************************************
/*	Name:		ChainsQuality
/*	Function:	Calculate quality of a CHAINS
/*	Parameter:	Chains -- CHAINS
/*	Return:		Quality of a CHAINS
/*
/***************************************************************************/
double CDirLine::ChainsQuality ( CHAINS &Chains )
{
	double gap, gap_paddings, sum_num, len, weight ;
	double sum_r;
    int segs, nChain, nNext ;

    gap = gap_paddings  = sum_num = 0 ;
	sum_r = 0;
    len = Chains.xe-Chains.xs+1 ;
    weight = segs = 0 ; 
    nChain = Chains.pHead ;
    while ( nChain >= 0 )
    {
        CHAIN Chain = m_pChain[nChain] ;
		sum_num += Chain.Num+2;//The start and end Run-Lengths are removed, so add 2 to detect dotted lines. 2000/4/22
        sum_r += Chain.r*Chain.Len ;
        weight += Chain.Len ;
        nNext = Chain.pRight ;
		int width = 0;
		int gap2 = 0;
        if ( nNext>=0)
		{	
			if(IsConnected2(nChain, nNext, width, gap2)==0)
			{
				if(width < 2*Chains.Width)
				{	gap +=gap2;
					sum_num += m_pChain[nNext].xs - Chain.xe - 1-gap2;
				}
				else	gap += m_pChain[nNext].xs - Chain.xe - 1 ;
				segs++ ;
			}
			else if(width < 2*Chains.Width)		
				sum_num += m_pChain[nNext].xs - Chain.xe-1;
		}
        if ( nChain == Chains.pTail )		break ;
        else
            nChain = nNext ;
    }

    double fgap ;
    if ( segs == 0 )
        fgap = 1 ;
    else
		fgap = sqrt(1-gap/segs/len);

	double	sum_dxdy, sum_dxdx, sum_dydy, dx, dy ;
	sum_dxdy = sum_dxdx = sum_dydy = 0;
    double  ax = (Chains.SumX+Chains.SumY)/Chains.Num ;
	double  ay = (Chains.SumY-Chains.SumX)/Chains.Num ;
	int		x, y, nTree;
	LineValley  v;
	
    int j = Chains.pHead ;
	nTree = 0;
	while(j >= m_nChainStart[nTree] && nTree<MAXSTRIP)	nTree++;
    while ( j >= 0 )
    {
        int pNode = m_pChain[j].pHead ;
		while ( pNode >= 0 )
        {
            v = m_pTree[nTree]->m_pNode[pNode].v ;
            x = v.x & 0x1fffffff ;
            if ( !(v.x & 0xc0000000))
            {   
                y = (v.yvs+v.yve)/2 ;
                dx=x+y-ax ; dy=y-x-ay ;
                sum_dxdx+=dx*dx ; sum_dydy+=dy*dy ; sum_dxdy+=dx*dy ;
            }
          if ( pNode != m_pChain[j].pTail )
                pNode = m_pTree[nTree]->m_pNode[pNode].pRight ;
            else
                break ;
        }
        j = m_pChain[j].pRight ;
    }
	double tmp = sqrt(sum_dxdx*sum_dydy);
	if(tmp<1e-8)
		Chains.r = 1.0;
	else
		Chains.r = fabs ( sum_dxdy / tmp ) ;

	Chains.Angle = GetAngle(DSCC_CPoint(Chains.xs, Chains.fYs), DSCC_CPoint(Chains.xe, Chains.fYe) );
	if(fabs(Chains.Angle) < PI*30/180)
		Chains.Q = sum_r/weight * Chains.r * Chains.r * fgap;
	else
		Chains.Q = fgap;

	if(segs == 0)//�����Ժõ�ֱ��
		Chains.Q = Chains.Q * pow(sum_num/(len-gap), 0.25)  ;
	else if(segs <= 2)
		Chains.Q = Chains.Q * pow(sum_num/(len-gap), 0.333);
	else//���ѵ�ֱ��
		Chains.Q = Chains.Q * pow(sum_num/(len-gap), 0.5) ;

	return Chains.Q ;
}


/***************************************************************************
/*	Name:		IsConnected
/*	Function:	Test if two CHAINs are connected
/*	Parameter:	c1 -- CHAIN 1
/*				c2 -- CHAIN 2
/*				Max -- ???
/*	Return:		TRUE -- Connected
/*				FALSE -- Not connected
/*
/***************************************************************************/
int CDirLine::IsConnected ( int c1, int c2 )
{
    int nTree = 0;
	while(c1>=m_nChainStart[nTree])		nTree++;
 	int  bConnected = FALSE;
	if ( m_pChain[c1].xe < m_pChain[c2].xs )
        bConnected =  m_pTree[nTree]->IsConnected ( m_pChain[c1].pTail, m_pChain[c2].pHead ) ;
	else
        bConnected =  m_pTree[nTree]->IsConnected ( m_pChain[c1].pHead, m_pChain[c2].pTail ) ;
	return bConnected;
}

/***************************************************************************
/*	Name:		IsConnected2
/*	Function:	Test if two CHAINs are connected within a tolerance
/*	Parameter:	c1 -- CHAIN 1
/*				c2 -- CHAIN 2
/*				width -- ???
/*				gap -- ???
/*	Return:		TRUE -- Connected
/*				FALSE -- Not connected
/*
/***************************************************************************/
int CDirLine::IsConnected2(int c1, int c2, int &width, int &gap)
{
    int nTree = 0;
	while(c1>=m_nChainStart[nTree])		nTree++;
	if ( m_pChain[c1].xe < m_pChain[c2].xs )
        return m_pTree[nTree]->IsConnected2 ( m_pChain[c1].pTail, m_pChain[c2].pHead, width, gap ) ;
	else
        return m_pTree[nTree]->IsConnected2 ( m_pChain[c1].pHead, m_pChain[c2].pTail, width, gap ) ;
}

/***************************************************************************
/*	Name:		InitChains
/*	Function:	Initialze a CHAINS structure with a CHAIN
/*	Parameter:	Chains -- CHAINS to initialzie
/*				Chain -- CHAIN to be copied to CHAINS
/*				nChain -- CHAIN NO
/*	Return:		0 
/*
/***************************************************************************/
int CDirLine::InitChains ( CHAINS &Chains, CHAIN &Chain, int nChain )
{
    Chains.SumX		= Chain.SumX ;
    Chains.SumY		= Chain.SumY ;
    Chains.SumXX	= Chain.SumXX ;
    Chains.SumXY	= Chain.SumXY ;
    Chains.Num		= Chain.Num ;
	Chains.NumPixel = Chain.NumPixel;
	Chains.r		= Chain.r;
    Chains.xs		= Chain.xs;
	Chains.xe		= Chain.xe ;
	Chains.fYs		= Chain.fYs;
	Chains.fYe		= Chain.fYe;

    Chains.pHead = Chains.pTail = nChain ;
	Chains.Width = Chain.Width;
	Chains.Q=1;
    return 0 ;
}
