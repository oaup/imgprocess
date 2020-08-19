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
// File Name:		DirLineDSCC.cpp
// File Function:	Build Directional Single Connected Chain
//
//				Developed by: Yefeng Zheng
//				First Created: June 2001
//			University of Maryland, College Park
//////////////////////////////////////////////////////////////////////////

#include "DirLine.h"
#include <algorithm>

/***************************************************************************
/*	Name:		CalTree
/*	Function:	Build Directional Single Connected Chain from runlengths 
/*	Parameter:	void
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/***************************************************************************/
int CDirLine::CalTree()
{
    int i, Len, pNode, pNext, pThis;
	static int MaxCnt=5000 ;
	if( m_pTree[m_nCurTree]->m_pEmptHead == 0 )
		return 0;
    DSCC_BYTE* nFlag = (DSCC_BYTE*)malloc(m_pTree[m_nCurTree]->m_pEmptHead);
	memset(nFlag,0, m_pTree[m_nCurTree]->m_pEmptHead);

	if(m_pChain == NULL)
	{	MaxCnt = 5000;
		m_pChain = (CHAIN *)malloc ( sizeof(CHAIN)*MaxCnt ) ;
	}

	if(m_pChain == NULL)		
		return -1;
	DSCC_RECT	CurrentRect = m_pTree[m_nCurTree]->m_rcRange;

    for ( i=0 ; i<m_pTree[m_nCurTree]->m_nDepth ; i++ )
    {
        pNode = m_pTree[m_nCurTree]->m_pColHead[i] ;
        while ( pNode >= 0 )
        {
			if(nFlag[pNode] == 0)
			{
                Len = 1 ;
                pThis = pNode ;
                while(1)
                {
					nFlag[pThis] = 1;
					if ( m_pTree[m_nCurTree]->m_pNode[pThis].nRtTotal == 1 )
                    {
						pNext = m_pTree[m_nCurTree]->m_pNode[pThis].pRight ;
						LineValley v = m_pTree[m_nCurTree]->m_pNode[pNext].v;
						if( v.yve-v.yvs >= 100 )
						{
							if( v.yvs == CurrentRect.left || v.yve == CurrentRect.right )
								break;
						}
                        if ( m_pTree[m_nCurTree]->m_pNode[pNext].nLtTotal==1 && m_pTree[m_nCurTree]->m_pNode[pNext].pLeft==pThis ) 
                        {
							Len++ ;
                            pThis = pNext ;
                        }
                        else
                            break ;
                    }
                    else
                        break ;
                } 
				if(m_Param.FilterSmallDSCC)
				{
					if(Len <= 2)	continue;
					if(Len<=4 )
					{	if(m_pTree[m_nCurTree]->m_pNode[pNode].nLtTotal==0 && m_pTree[m_nCurTree]->m_pNode[pThis].nRtTotal==0)	
								continue;
					}
				}

                m_pChain[m_nChain].pHead = pNode ;
                m_pChain[m_nChain].pTail = pThis ;
                m_pChain[m_nChain].pLeft = m_pChain[m_nChain].pRight = -1 ;
                m_pChain[m_nChain++].Len = Len ;
                if ( m_nChain == MaxCnt )
                {
				    MaxCnt += 3000;
					m_pChain = (CHAIN *)realloc ( m_pChain, sizeof(CHAIN)*MaxCnt ) ;
					if(m_pChain == NULL) 	
						return -1;
				}
            }
            pNode = m_pTree[m_nCurTree]->m_pNode[pNode].pUnder ;
        }
    }

    free(nFlag);	
    for ( i=m_nOldChain ; i<m_nChain ; i++ )
		ChainStatics ( m_pChain[i] ) ;
	return 0 ;
}

/***************************************************************************
/*	Name:		SetDetectParams
/*	Function:	Set parameters for building Directional Single Connected 
/*				Chain from runlengths 
/*	Parameter:	nIsHorLine	-- TRUE  Detect horizontal lines
/*							   FALSE Detect vertical lines
/*				Param -- Parameters
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/***************************************************************************/
int CDirLine::SetDetectParams ( int bIsHorLine, PARAMETER &Param )
{
    m_bIsHorLine = bIsHorLine ;
	m_bParamsSet = TRUE;
    m_Param = Param ;
    return 0 ;
}

/***************************************************************************
/*	Name:		SetDefaultDetectParams
/*	Function:	Set default parameters for building Directional Single Connected 
/*				Chain from runlengths 
/*	Parameter:	
/*	Return:		0  -- Succeed
/*				-1 -- Error
/***************************************************************************/
int CDirLine::SetDefaultDetectParams ( )
{
	m_bParamsSet = TRUE;
    m_Param.ValleyGrayDepth = 30;
	m_Param.MinVerLineLength = 30;
	m_Param.MinHorLineLength = 50;
	m_Param.MaxLineWidth = 15;
	m_Param.FilterSmallDSCC = TRUE;
	m_Param.RLS = FALSE;
	m_Param.bHasSlantLine = FALSE;
	m_Param.MaxGap = 15;
    return 0 ;
}
/***************************************************************************
/*	Name:		BuildConnTree
/*	Function:	Extract runlengths 
/*	Parameter:	Img -- Form image
/8				rRange -- Image region to analysed
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/***************************************************************************/
int CDirLine::BuildConnTree ( CImage& Img, DSCC_RECT rRange )
{
	if( m_bParamsSet == FALSE )
		SetDefaultDetectParams();
    int  ImgType = Img.GetType() ;
    if ( ImgType!=DIB_1BIT && ImgType!=DIB_8BIT && ImgType!= DIB_24BIT)
        return -2 ;
    int w = Img.GetWidth() ;
    int h = Img.GetHeight() ;
    int wb = Img.GetLineSizeInBytes() ;
	int	bUpSideDown = Img.GetOrientation();
    if ( rRange.top<0 || rRange.top>=h || rRange.bottom<0 || rRange.bottom>=h || rRange.bottom<rRange.top ||
         rRange.left<0 || rRange.left>=w || rRange.right<0 || rRange.right>=w || rRange.right<rRange.left )
        return -3 ;

    int j, k, st, ed, span, valleys ;

    if ( m_bIsHorLine )
    {
        st = rRange.left ;
        ed = rRange.right ;
        span = rRange.bottom-rRange.top+1 ;
    }
    else
    {
        st = rRange.top ;
        ed = rRange.bottom ;
        span = rRange.right-rRange.left+1 ;
    }

    if ( !m_pTree[m_nCurTree]->Initialize(rRange) )
        return -1 ;

    int *t = (int *)malloc(sizeof(int)*span+16) ;
    if ( t == (int *)NULL ) return -1;
    int *s = (int *)malloc(sizeof(int)*span+16) ;
    if ( s == (int *)NULL ) { free(t); return -1 ; }
    LineValley *v = (LineValley *)malloc(sizeof(LineValley)*span*2+16) ;
    if ( v == (LineValley *)NULL ) { free(s); free(t); return -1; }

    DSCC_BYTE *p = Img.LockRawImg() ;
    if ( p==(DSCC_BYTE *)NULL || wb<=0 )
	{ free(v); free(s); free(t); return -1; }
	
    for ( j=st ; j<=ed ; j++ )
    {
		if ( ImgType == DIB_1BIT )
        {
            if ( m_bIsHorLine )
			{   
				valleys = GetColumnRunLength ( p, wb, h, j, rRange.top, rRange.bottom, v, bUpSideDown ) ;
				if(m_Param.RLS)
					ColRunLenSmooth(p, wb, h, j, v, valleys);
			}
			else
			{   valleys = GetRowRunLength ( p, wb, h, j, rRange.left, rRange.right, v, bUpSideDown) ;
 				if(m_Param.RLS)	
					RowRunLenSmooth(p, wb, h, j, v, valleys);
			}
		} 
		else if ( ImgType == DIB_8BIT )
        {
            if ( m_bIsHorLine )
            {
		        AquireHorLineData ( p, wb, h, j, rRange.top, rRange.bottom, s, ImgType, 0, bUpSideDown ) ;
				valleys = ValleyDetect ( s, rRange.top, rRange.bottom, v, t, m_Param.ValleyGrayDepth, m_Param.MaxLineWidth ) ;
            }
            else
            {
		        AquireVerLineData ( p, wb, h, j, rRange.left, rRange.right, s, ImgType, 0, bUpSideDown ) ;
				valleys = ValleyDetect ( s, rRange.left, rRange.right, v, t, m_Param.ValleyGrayDepth, m_Param.MaxLineWidth ) ;
            }
        }
        else if ( ImgType == DIB_24BIT)
		{
		   if( m_bIsHorLine)
		   {
				AquireHorLineData ( p, wb, h, j, rRange.top, rRange.bottom, s, ImgType, 0, bUpSideDown ) ;
				valleys = ValleyDetect ( s, rRange.top, rRange.bottom, v, t, m_Param.ValleyGrayDepth, m_Param.MaxLineWidth ) ;
            }
            else
            {
		        AquireVerLineData ( p, wb, h, j, rRange.left, rRange.right, s, ImgType, 0, bUpSideDown ) ;
				valleys = ValleyDetect ( s, rRange.left, rRange.right, v, t, m_Param.ValleyGrayDepth, m_Param.MaxLineWidth ) ;
            }
		}
        else
            break ;

        for ( k=0 ; k<valleys ; k++ )
		{
			v[k].x = j ;
			int vx = j;
			if( ImgType != DIB_1BIT )	continue;
			if( m_bIsHorLine )
			{
				if( v[k].ye == rRange.bottom && v[k].ye < h-1 )
				{
					DSCC_BYTE mask = 128>>(vx%8) ;
					while(v[k].ye < h-1 )
					{
						DSCC_BYTE	DownLineByte = *( p+(h-1-v[k].ye-1)*wb+vx/8 );
						if ( (DownLineByte & mask) == 0 )
								break;
						v[k].ye++;
					}
				}
				if( v[k].ys == rRange.top && v[k].ys > 0 )
				{
					DSCC_BYTE mask = 128>>(vx%8) ;
					while(v[k].ys > 0 )
					{
						DSCC_BYTE	UpLineByte = *( p+(h-1-v[k].ys+1)*wb+vx/8 );
						if( (UpLineByte & mask) == 0 )	break;
						v[k].ys --;
					}
				}
			}
			else
			{ 
				if( v[k].ye == rRange.right && v[k].ye < w-1 )
				{
					DSCC_BYTE	mask = 128>>( (v[k].ye+1)%8 );
					while( v[k].ye < w-1 )
					{
						DSCC_BYTE	RightPixelByte = *( p+(h-1-vx)*wb+(v[k].ye+1)/8 );
						if ( (RightPixelByte & mask) == 0 )	break;
						v[k].ye++;
					}
				}
				if( v[k].ys == rRange.left && v[k].ys > 0 )
				{
					DSCC_BYTE	mask = 128>>( (v[k].ys-1)%8 );
					while(v[k].ys > 0 )
					{
						DSCC_BYTE	LeftPixelByte = *( p+(h-1-vx)*wb+(v[k].ys-1)/8 );
						if( (LeftPixelByte & mask) == 0 )		break;
						v[k].ys--;
					}
				}
			}
		}
		m_pTree[m_nCurTree]->AddNewCol ( v, valleys, j ) ;
	}
     
	free(v); 
	free(s); 
	free(t); 
	Img.UnlockRawImg();
    return 0 ;
}

/***************************************************************************
/*	Name:		AquireHorLineData
/*	Function:	Read a vertical image line data to build horizontal DSCC
*	Parameter:	p -- Pointer to image
/*				wb -- Width of the whole image in bytes
/*				h -- Height of the whole image
/*				n -- X offset
/*				start -- Start line
/*				end -- End line
/*				buf -- Buffer for read data
/*				ImgType -- Image type
/*				Channel -- Channel for Color image
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/***************************************************************************/
int CDirLine::AquireHorLineData ( DSCC_BYTE *p, int wb, int h, int n, int start, int end, int *buf, int ImgType, int Channel, int bUpSideDown )
{
    int i ;
	if ( ImgType == DIB_1BIT )
	{
		DSCC_BYTE mask = 128>>(n%8) ;
		if( bUpSideDown )
		{
			p += wb*(h-1-start) + n/8 ;   // DIB: upside down
			for ( i=start ; i<=end ; i++, p-=wb )
				if ( *p & mask )
					*buf++ = 0 ;
				else
					*buf++ = 255 ;
		}
		else
		{
			p += wb*start + n/8;
			for ( i=start ; i<=end ; i++, p+=wb )
				if ( *p & mask )
					*buf++ = 0 ;
				else
					*buf++ = 255 ;
		}
	}  
	else if ( ImgType == DIB_8BIT )
    {
		if( bUpSideDown )
		{
			p += wb*(h-1-start) + n ;   // DIB: upside down
			for ( i=start ; i<=end ; i++, p-=wb )
				*buf++ = *p ;
		}
		else
		{//Not upside down
			p += wb*start + n;
			for( i=start; i<=end ; i++, p+=wb )
				*buf++ = *p;
		}
    }
    else if ( ImgType == DIB_24BIT)
	{
		if( bUpSideDown )
		{
			p += wb*(h-1-start) + 3*n;
			for ( i=start; i<=end ; i++, p-=wb )
				*buf++ = *(p+Channel);
		}
		else
		{
			p += wb*start + 3*n;
			for ( i=start; i<=end ; i++, p+=wb )
				*buf++ = *(p+Channel);
		}
	}
	else
        return -1 ;
    return 0 ;
}

/***************************************************************************
/*	Name:		AquireVerLineData
/*	Function:	Read a horizontal image line data to build vertical DSCC
/*	Parameter:	p -- Pointer to image
/*				wb -- Width of the whole image in bytes
/*				h -- Height of the whole image
/*				n -- Line
/*				start -- Start X ordinate
/*				end -- End X ordinate
/*				buf -- Buffer for read data
/*				ImgType -- Image type
/*				Channel -- Channel for Color image
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/***************************************************************************/
int CDirLine::AquireVerLineData ( DSCC_BYTE *p, int wb, int h, int n, int start, int end, int *buf, int ImgType, int Channel, int bUpSideDown )
{
    int i, j ;
	if ( ImgType == DIB_1BIT )
    {
        DSCC_BYTE mask[8] = {128,64,32,16,8,4,2,1} ;
        DSCC_BYTE tmp ;
		if( bUpSideDown )
			p += wb*(h-1-n) + start/8 ;
		else
			p += start/8 ;
		for ( i=start, j=start%8, tmp=*p++ ; i<=end ; i++ )
			{
				if ( tmp & mask[j] )
					*buf++ = 0 ;
				else
					*buf++ = 255 ;
				j++ ;
				if ( j == 8 )
				{
					tmp= *p++ ;
					j = 0 ;
				}
			}
    }
	else if ( ImgType == DIB_8BIT )
    {
		if( bUpSideDown )
			p += wb*(h-1-n) + start ;   // DIB: upside down
		else 
			p += wb*n + start;
        for ( i=start ; i<=end ; i++ )
            *buf++ = *p++ ;
    }
	else if( ImgType == DIB_24BIT )
	{
		if( bUpSideDown )
			p += wb*(h-1-n) + 3*start;
		else
			p += wb*n + 3*start;
		for ( i=start; i<=end; i++)
		{
			*buf++ = *(p+Channel);
			p += 3;
		}
	}
    else
        return -1 ;

    return 0 ;
}

/***************************************************************************
/*	Name:		GetColumnRunLength
/*	Function:	Build column runlength
/*	Parameter:
/*	Return:		0  -- Succeed
/*				-1 -- Error
/***************************************************************************/
int CDirLine::GetColumnRunLength ( DSCC_BYTE *p, int w, int h, int column, int start, int end, LineValley *valley, int bUpSideDown )
{
    int i, Valleys=0 ;
    int bInValley ;

    DSCC_BYTE mask = 128>>(column%8) ;
	if( bUpSideDown )
		p += w*(h-1-start) + column/8 ;   // DIB: upside down
	else
		p += w*start + column/8;

    for ( i=start, bInValley=FALSE ; i<=end ; i++ )
    {
        if ( *p & mask )
        {
            if ( !bInValley )
            {
                valley->ys = valley->yvs = i ;
                valley->EdgeGray = 255 ;
                valley->gray = 0 ;
                bInValley = TRUE ;
            }
        }
        else
        {
            if ( bInValley )
            {
                valley->ye = valley->yve = i-1 ;
                valley++ ;
                Valleys++ ;
                bInValley = FALSE ;
            }
        }
		if( i == end )	break;
		if( bUpSideDown )	p-=w;
		else				p+=w;
    }
    if ( bInValley )
    {
        valley->ye = valley->yve = i-1 ;
        Valleys++ ;
    }
    return Valleys ;
}

/***************************************************************************
/*	Name:		GetRowRunLength
/*	Function:	Build row runlength
/*	Parameter:
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/***************************************************************************/
int CDirLine::GetRowRunLength ( DSCC_BYTE *p, int w, int h, int row, int start, int end, LineValley *valley, int bUpSideDown )
{
    int i, j, Valleys=0 ;
    int bInValley ;

    DSCC_BYTE mask[8] = {128,64,32,16,8,4,2,1} ;
    DSCC_BYTE tmp ;
	if( bUpSideDown )
		p += w*(h-1-row) + start/8 ;
	else
		p += w*row + start/8 ;

    for ( i=start, j=start%8, bInValley=FALSE, tmp=*p++ ; i<=end ; i++ )
    {
        if ( tmp & mask[j] )
        {
            if ( !bInValley )
            {
                valley->ys = valley->yvs = i ;
                valley->EdgeGray = 255 ;
                valley->gray = 0 ;
                bInValley = TRUE ;
            }
        }
        else
        {
            if ( bInValley )
            {
                valley->ye = valley->yve = i-1 ;
                valley++ ;
                Valleys++ ;
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
        valley->ye = valley->yve = i-1 ;
        Valleys++ ;
    }
    return Valleys ;
}

/***************************************************************************
/*	Name:		ColRunLenSmooth
/*	Function:	Column runlength smoothing
/*	Parameter:
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/***************************************************************************/
int CDirLine::ColRunLenSmooth(DSCC_BYTE *p, int wb, int h, int col, LineValley *valley, int& Valleys)
{
	int T1 = 3; 
	int T2 = 6;
	int MinLen = 20;

    DSCC_BYTE mask = 128>>(col%8) ;
	int i=0, j;
	while(i<Valleys-1)
	{
		if(valley[i+1].ys-valley[i].ye <= T1 || 
		(valley[i+1].ys-valley[i].ye<=T2 && valley[i].ye-valley[i].ys > MinLen && valley[i+1].ye-valley[i+1].ys > MinLen) )
		{
			for(j=valley[i].ye+1; j<=valley[i+1].ys-1; j++)
				*(p+wb*(h-1-j) + col/8) |= mask;
			valley[i].ye = valley[i+1].ye;
			valley[i].yve = valley[i+1].yve;
			for(j=i+1; j<Valleys-1; j++)
				valley[j] = valley[j+1];
			Valleys--;
			continue;
		}
		i++;
	}
	return 0;
}

/***************************************************************************
/*	Name:		RowRunLenSmooth
/*	Function:	Row runlength smoothing
/*	Parameter:
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/***************************************************************************/
int CDirLine::RowRunLenSmooth(DSCC_BYTE *p, int wb, int h, int row,LineValley *valley, int& Valleys)
{
	int T1 = 3; 
	int T2 = 6;
	int MinLen = 20;

	DSCC_BYTE mask[8] = {128,64,32,16,8,4,2,1} ;
	p+=wb*(h-1-row);
	int i=0, j;
	while(i<Valleys-1)
	{
		if(valley[i+1].ys-valley[i].ye <= T1 || 
		(valley[i+1].ys-valley[i].ye<=T2 && valley[i].ye-valley[i].ys > MinLen && valley[i+1].ye-valley[i+1].ys > MinLen) )
		{
//			for(j=valley[i].ye; j<=valley[i+1].ys-1; j++) //Change the raw image.
//				*(p+j/8) |= mask[j%8];
			valley[i].ye = valley[i+1].ye;
			valley[i].yve = valley[i+1].yve;
			for(j=i+1; j<Valleys-1; j++)
				valley[j] = valley[j+1];
			Valleys--;
			continue;
		}
		i++;
	}
	return 0;
}

/***************************************************************************
/*	Name:		ValleyDetect
/*	Function:	Detect valley on a image line for binary/gray image
/*	Parameter:
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/***************************************************************************/
int CDirLine::ValleyDetect ( int *p, int start, int end, LineValley *valley, int *s, int Depth, int MaxLen )
{//0 --- black             255 --- white
    int dire, InValley ;
    int i, j, nadir_pos, vsp, vep, vs, nadir, mid, t, l ;
    int Valleys = 0 ;
    int sp = 0 ;

    nadir = vs = l = *p ;
    nadir_pos = vsp = start ;
    dire = FALSE ;
    InValley = FALSE ;

    for ( i=start ; i<=end ; i++, p++ )  // DIB: upside down
    {
        t = *p ;
        if ( InValley )
            s[sp++] = t ;
        if ( dire )     // downward
        {
            if ( t > l )
            {
                if ( l < nadir )
                {
                    nadir = l ;
                    nadir_pos = i-1 ;
                }
                if ( !InValley )
                {
                    if ( vs-l > Depth )
                    {
                        InValley = TRUE ;
                        for ( j=i-vsp ; j>=0 ; j-- )
                            s[sp++] = *(p-j) ;
                    }
                }
                dire = FALSE ;
            }
            else
            {
                if ( !InValley )
                    if ( i-vsp > MaxLen  )
                        if ( vs-t < Depth )
                        {
                            vsp = i-1 ;
                            vs = l ;
                            nadir_pos= i ;
                            nadir = t ;
                        }
            }
        }
        else    // upward
        {
            if ( t < l )
            {
                if ( !InValley )
                {
                    mid = (nadir+vs*2)/3 ;
                    if ( l>=mid )
                    {
                        vsp = i-1 ;
                        vs = l ;
                        nadir = t ;
                        nadir_pos = i ;
                    }
                }
                else
                {
                    mid = (nadir+vs*2)/3 ;
                    if ( l>=mid )
                    {
                        InValley = FALSE ;
                        valley->ys = vsp ;
                        valley->ye = i-1 ;
                        int count=0 ;
                        mid = max(l,vs)-Depth ;
                        for ( j=sp-2 ; j>=0 ; j-- )
                            if ( s[j] <= mid )
                            {
                                vep = vsp+j ;
                                break ;
                            }
                            else
                            {
                                count++ ;
                                if ( count == MaxLen )
                                {
                                    valley->ye -= MaxLen ;
                                    count = 0 ;
                                }
                            }
                        mid = vs-Depth ;
                        for ( j=0 ; j<sp ; j++ )
                            if ( s[j] <= mid )
                            {
                                vsp += j ;
                                break ;
                            }
                        valley->yvs = vsp ;
                        valley->yve = vep ;
                        valley->EdgeGray = max(l,vs) ;
                        valley->gray = nadir ;
                        valley++ ;

                        Valleys ++ ;
                        vsp = i-1 ;
                        vs = l ;
                        nadir = t ;
                        nadir_pos = i ;
                        sp = 0 ;
                    }
                }
                dire = TRUE ;
            }
        }
        l = t ;
    }

    if ( InValley )
    {
        valley->ys = vsp ;
        valley->ye = i-1 ;
        mid = max(l,vs)-Depth ;
        for ( j=sp-2 ; j>=0 ; j-- )
            if ( s[j] <= mid )
            {
                vep = vsp+j ;
                break ;
            }
        mid = vs-Depth ;
        for ( j=0 ; j<sp ; j++ )
            if ( s[j] <= mid )
            {
                vsp += j ;
                break ;
            }
        valley->yvs = vsp ;
        valley->yve = vep ;
        valley->EdgeGray = max(l,vs) ;
        valley->gray = nadir ;
        Valleys ++ ;
    }
    return Valleys ;
}

/***************************************************************************
/*	Name:		ChainStatics
/*	Function:	Calculate some statistical properties of a CHAIN
/*	Parameter:  Chain -- CHAIN
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/***************************************************************************/
int  _x[25000];
int CDirLine::ChainStatics( CHAIN &Chain )
{
    double sum_x, sum_ys, sum_ye, sum_xx, sum_xys, sum_xye ;
    double sum_dxdy, sum_dxdx, sum_dydy, ax, ay, dx, dy /*sum_err, err*/ ;
    int    *x, *ys, *ye, *w, *cntWider ;
    int Len, pNode, j, num, numpixel ;
	double  averWidth;

    Len = Chain.Len ;
    if ( Len <= 5000 )
        x=_x ;
    else
    {    x = (int *)malloc ( sizeof(int)*5*Len ) ;
		if(x==NULL)		return -1;
	}
    ys = x+Len ;
    ye = ys+Len ;
    w = ye+Len ;
    cntWider = w+Len ;
	averWidth = 0;
	num = 0;

//Initialize the bounding box of the CHAIN	
	Chain.rcBound.top		= 10000;
	Chain.rcBound.left		= 10000;
	Chain.rcBound.bottom	= -1;
	Chain.rcBound.right		= -1;


    pNode = Chain.pHead ;
    for ( j=0 ; j<Len ; j++ )
    {
        x[j] = m_pTree[m_nCurTree]->m_pNode[pNode].v.x & 0x1fffffff ;
        ys[j] = m_pTree[m_nCurTree]->m_pNode[pNode].v.yvs ;
        ye[j] = m_pTree[m_nCurTree]->m_pNode[pNode].v.yve ;
        w[j] = ye[j]-ys[j]+1 ; 
		if(w[j]<m_Param.MaxLineWidth)
		{	
			averWidth += w[j];
			num++;
		}
		cntWider[j]=0 ;
        pNode = m_pTree[m_nCurTree]->m_pNode[pNode].pRight ;

		//Update the bounding box of the CHAIN
		Chain.rcBound.left	= min( Chain.rcBound.left, (long)x[j] );
		Chain.rcBound.right = max(Chain.rcBound.right, (long)x[j]);
		Chain.rcBound.top = min(Chain.rcBound.top, (long)ys[j]);
		Chain.rcBound.bottom = max(Chain.rcBound.bottom, (long)ye[j]);
    }
	if(num!=0)	averWidth = averWidth /num;
	else		averWidth = 3;

    sum_x=sum_ys=sum_ye=sum_xx=sum_xys=sum_xye=0 ;
    num = numpixel = 0 ;
 	Chain.Width = 0;

	double sx, sxx, sy, syy;
	sx=sy=sxx=syy=0;
    for ( j=0; j<Len; j++ )
    {    
		if ( w[j] <= 2*averWidth || Len < 20 )
        {
 		    Chain.Width += ye[j]-ys[j]+1;
            sum_x	+= x[j] ;     
			sum_xx	+=x[j]*x[j] ;	
			sum_ys	+= ys[j] ;     
			sum_ye	+= ye[j] ;
			sum_xys	+=x[j]*ys[j] ; 
			sum_xye	+=x[j]*ye[j] ;
            num++ ;

			//Calculate new variables 09/03/2004
			int nn = ye[j]-ys[j]+1;
			sx	+= 1.0*x[j]*nn;
			sxx += 1.0*x[j]*x[j]*nn;
			sy  += 1.0*(ys[j]+ye[j])*nn/2;
			numpixel += nn;
			for( int k=ys[j]; k<=ye[j]; k++ )
				syy += 1.0*k*k;		
        }
	}
	Chain.NumPixel = numpixel;
	Chain.SX	= sx;
	Chain.SY	= sy;
	Chain.SXX	= sxx;
	Chain.SYY	= syy;

	Chain.Num	= num ;
	Chain.SumX	= sum_x ;				
	Chain.SumXX	= sum_xx ;	
    Chain.SumY	= (sum_ys+sum_ye)/2 ;  
	Chain.SumXY	= (sum_xys+sum_xye)/2 ;
    Chain.xs	= x[0] ;
    Chain.xe	= x[Len-1];
    Chain.fYs	= (int)fYofChain(Chain, Chain.xs); 
	Chain.fYe	= (int)fYofChain(Chain, Chain.xe);

    if(num==0)
		ax = ay =0;
	else
	{	ax = (Chain.SumX + Chain.SumY)/num ;
		ay = (Chain.SumY - Chain.SumX)/num ;
		Chain.Width/=num;
		averWidth = Chain.Width;
	}
    sum_dxdx = sum_dydy = sum_dxdy = 0 ;
	m_pTree[m_nCurTree]->m_pNode[Chain.pHead].v.x |= 0xc0000000;
	m_pTree[m_nCurTree]->m_pNode[Chain.pTail].v.x |= 0xc0000000;
    pNode = Chain.pHead ;
    pNode = m_pTree[m_nCurTree]->m_pNode[pNode].pRight ; //Add 2000/4/10
	Chain.Width = 0;
	num = 0;
	double Err = 0;
	for ( j=1 ; j<Len-1 ; j++ )//The start and end run-length may be odd, do not add them  2000/3/23
    {   
		if ( w[j] <= 1.5*averWidth )//2000-3-15
		{
            dx=x[j]+(ys[j]+ye[j])/2-ax ; dy=(ys[j]+ye[j])/2-x[j]-ay ;
            sum_dxdx+=dx*dx ; sum_dydy+=dy*dy ; sum_dxdy+=dx*dy ;
			Chain.Width += w[j];
			num++;
			if( Len < 50 )
				Err += fabs((ys[j]+ye[j])/2-fYofChain(Chain, x[j]));
		}
		else
            m_pTree[m_nCurTree]->m_pNode[pNode].v.x |= 0xc0000000 ;//Ã«´Ì
        pNode = m_pTree[m_nCurTree]->m_pNode[pNode].pRight ;
    }
	if( Err > 1.5*num )	//2000-7-1
	{
		Chain.pLeft = -2;
		Chain.pRight = -2;
	}

	if(num == 0)	Chain.Width = averWidth;
	else			Chain.Width = Chain.Width/num;
    if ( Len <= 8 || sum_dxdx<1e-7 || sum_dydy<1e-7 )
        Chain.r = 1 ;
    else
        Chain.r = fabs ( sum_dxdy/sqrt(sum_dxdx*sum_dydy) );

	if( Len>8 && Chain.Width >= 8)//2000-3-6
	{
		double Angle = ::GetAngle(DSCC_CPoint(Chain.xs, Chain.fYs), DSCC_CPoint(Chain.xe, Chain.fYe));
		Chain.Width = Chain.Width*cos(Angle);
	}
    if ( Len > 5000 ) free(x) ;
    return 0 ;
}

/**********************************************************************************
/*	Name:		InWhichTree
/*	Function:	Get the CConnTree which the CHAIN belong to
/*	Parameter:	nChain -- CHAIN NO
/*	return:		-1 -- Error
/*				Others -- The ConnTree which the CHAIN belong to
/*
/*********************************************************************************/
int CDirLine::InWhichTree(int nChain)
{
    int	nTree = 0;
	while(nChain >= m_nChainStart[nTree] && nTree<MAXSTRIP)	nTree++;
	if(nTree < MAXSTRIP)
		return nTree;
	else
		return -1;
}

/**********************************************************************************
/*	Name:		InWhichChains
/*	Function:	Get the CHAINS which the CHAIN belong to
/*	Parameter:	nChain -- CHAIN NO
/*	return:		-1 -- Error
/*				Others -- The CHAINS which the CHAIN belong to
/*
/*********************************************************************************/
int CDirLine::InWhichChains ( int nChain )
{
    if(m_pChain[nChain].pLeft == -1 && m_pChain[nChain].pRight == -1)	return -1;
	int nLeft = nChain ;
    while ( m_pChain[nLeft].pLeft >= 0 )
        nLeft = m_pChain[nLeft].pLeft ;
    int nRight = nChain ;
    while ( m_pChain[nRight].pRight >= 0 )
        nRight = m_pChain[nRight].pRight ;
    for ( int i=0 ; i<m_nChains ; i++ )
        if ( m_pChains[i].pHead==nLeft && m_pChains[i].pTail==nRight )
            return i ;
    return -1 ;
}