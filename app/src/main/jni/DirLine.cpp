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
// File Name:		DirLine.cpp
// File Function:	Detect form frame lines
//
//				Developed by: Yefeng Zheng
//				First Created: June 2001
//			University of Maryland, College Park
//////////////////////////////////////////////////////////////////////////


#include "FormStructDef.h"
#include "DirLine.h"
#include "Tools.h"
#include "math.h"
#include "ImageTool.h"


#define GAPBETWEENLINE  5
//IMPLEMENT_SERIAL(CDirLine, CObject, 0)

///////////////////////////////////////////////////////////////////////////////////
//	Methods to detect form line
///////////////////////////////////////////////////////////////////////////////////
CDirLine::CDirLine()
{
    m_pChain = (CHAIN *)NULL ;
    m_nChain = 0 ;
	m_nOldChain = 0;
    m_pChains = (CHAINS *)NULL ;
    m_nChains = 0 ;
    m_pLine = (FORMLINE *)NULL ;
    m_nLine = 0 ;
		
	m_nCurTree = 0;
	m_pTree = NULL;
	memset(m_nChainStart,0, sizeof(int)*MAXSTRIP);
	m_nStrip = 1;
	m_Param.MaxGap = 15;
	m_bParamsSet = FALSE;
}

CDirLine::~CDirLine()
{
    FreeMem();
}

/****************************************************************************
/*	Name:		FreeMem
/*	Function:	Free memory
/*	Parameter:	void
/*	return:		0 -- Succeed
/*				1 -- Error
/***************************************************************************/
int CDirLine::FreeMem()
{
	if( m_pTree != NULL)
		for(int i = 0; i <= m_nCurTree; i++) 
			delete m_pTree[i]; 
	free(m_pTree);
	m_pTree = NULL;
	m_nCurTree = 0;
	if ( m_nLine>0 || m_pLine!=(FORMLINE *)NULL )
	{
			free ( m_pLine ) ;
			m_pLine = (FORMLINE *)NULL ;
			m_nLine = 0 ;
    }
    if ( m_nChains>0 || m_pChains!=(CHAINS *)NULL )
    {
        free ( m_pChains ) ;
        m_pChains = (CHAINS *)NULL ;
        m_nChains = 0 ;
    }
    if ( m_nChain>0 || m_pChain!=(CHAIN *)NULL )
    {
        free ( m_pChain ) ;
        m_pChain = (CHAIN *)NULL ;
		m_nOldChain = 0;
        m_nChain = 0 ;
    }
    return 0 ;
}

/****************************************************************************
/*	Name:		ChainsToFORMLINE
/*	Function:	Get a frame line from a CHAINS
/*	Parameter:	Line -- A new frame line
/*				Chains -- CHAINS
/*	return:		0 -- Succeed
/*				1 -- Error
/*
/***************************************************************************/
int CDirLine::ChainsToFORMLINE ( FORMLINE &Line, CHAINS &Chains )
{
    int xs, xe, ys, ye;

    if ( m_bIsHorLine )
    {
        xs = Chains.xs ;
        xe = Chains.xe ;
		ys = Chains.fYs;
		ye = Chains.fYe;
	}
    else
    {
        ys = Chains.xs ;
        ye = Chains.xe ;
		xs = Chains.fYs;
		xe = Chains.fYe;
	}
    Line.StPnt = DSCC_CPoint(xs, ys) ;
    Line.EdPnt = DSCC_CPoint(xe, ye) ;
    Line.Angle = GetAngle ( Line.StPnt, Line.EdPnt ) ;
    Line.Width = Chains.Width ;
    Line.Q = Chains.Q ;
	Line.nUseType = 0;
	Line.nStyle = 0;
	Line.bSlant= FALSE;
    return 0 ;
}

