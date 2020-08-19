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
// File NAME:		Tools.cpp
// File Function:	Commnon Functions For Other Modules
//
//				Developed by: Yefeng Zheng
//				University of Maryland, College Park
/////////////////////////////////////////////////////////////////////////


#include <math.h>
#include "tools.h"


unsigned char
lz[8]={ 0xff,0x7f,0x3f,0x1f,0x0f,0x07,0x03,0x01 },//Left mask
rz[8]={ 0x00,0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe };//Right mask
unsigned char lut[256]={0,0,0};	//Test how many black pixels in one byte

////////////////////////////////////////////////////////////////////////////
//	File name manipulation functions
///////////////////////////////////////////////////////////////////////////

/******************************************************************************
/*	Name:		FileExist
/*	Function:	Test if a file exists or not
/*	Parameter:	strFileName -- File name
/*	Return:		TRUE  -- Exist
/*				FALSE -- Not exist
/*****************************************************************************/
// int	FileExist( CString strFileName )
// {
// 	CFileFind a;
// 	int	bFileExist = a.FindFile(strFileName, 0);
// 	a.Close();
// 	return bFileExist;
// }

/******************************************************************************
/*	Name:		DirExist
/*	Function:	Test if a directory exists or not
/*	Parameter:	csDir -- Directory
/*	Return:		TRUE  -- Exist
/*				FALSE -- Not exist
/*****************************************************************************/
// int	DirExist( wstring csDir )
// {
// 	CFileFind a;
// 	int	bDirExist = a.FindFile(csDir+"\\*.*", 0);
// 	a.Close();
// 	return bDirExist;
// }

/******************************************************************************
/*	Name:		GetPathName
/*	Function:	Get path name from a file name string
/*	Parameter:	strFileName -- File name
/*	Return:		Path name
/*
/*****************************************************************************/
string GetPathName ( string strFileName )
{
    int pos, l ;
    l = strFileName.length() ;
    if ( (pos=strFileName.find('\\')) >= 0 )
        return strFileName.substr (0, pos+1 ) ;
    else if ( (pos=strFileName.find(':')) >= 0 )
        return strFileName.substr (0, pos+1 ) ;
    else
        return ".\\";
}	

/******************************************************************************
/*	Name:		GetFileName
/*	Function:	Get file name without path from a file name string
/*	Parameter:	strFileName -- File name string
/*	Return:		File name
/*
/*****************************************************************************/
string GetFileName ( string strFileName )
{
    int pos, l ;
    l = strFileName.length() ;
    if ( (pos=strFileName.find('\\')) >= 0 )
        return strFileName.substr ( pos+1 ) ;
    else if ( (pos=strFileName.find(':')) >= 0 )
        return strFileName.substr ( pos+1) ;
    else
        return strFileName;
}

/******************************************************************************
/*	Name:		GetFileNameWithoutExt
/*	Function:	Get file name without path from a file name string, without extension
/*	Parameter:	strFileName -- File name string
/*	Return:		File name
/*****************************************************************************/
string GetFileNameWithoutExt ( string strFileName )
{
    int pos, l ;
	string csFileName;
    l = strFileName.length() ;
    if ( (pos=strFileName.find('\\')) >= 0 )
        csFileName = strFileName.substr( pos+1 ) ;
    else if ( (pos=strFileName.find(':')) >= 0 )
        csFileName = strFileName.substr ( pos+1) ;
    else
        csFileName = strFileName;

	pos = csFileName.find( '.' );
	if( pos >= 0 )
		return csFileName.substr(0, pos );
	else
		return csFileName;
}

/******************************************************************************
/*	Name:		ChangeFileExt
/*	Function:	Change file extension
/*	Parameter:	dest -- Destination file name
/*				src  -- Source file name
/*				ext  -- File extension wanted 
/*	Return:		0  -- Correct
/*				-1 -- Error
/*
/*****************************************************************************/
int ChangeFileExt ( char *dest, char *src, char *ext )
{
    strcpy ( dest, src ) ;
    for ( int i=strlen(dest)-1 ; i>=0 ; i-- )
    {
        if ( dest[i] == '.' )
        {
            dest[i] = '\0' ;
            break ;
        }
        else if ( dest[i] == '\\' )
            break ;
    }
    strcat ( dest, "." ) ;
    strcat ( dest, ext ) ;
    return 0 ;
}

/******************************************************************************
/*	Name:		ChangeFileExt
/*	Function:	Change file extension, another interface
/*	Parameter:	dest -- Destination file name
/*				src  -- Source file name
/*				ext  -- File extension wanted 
/*	Return:		0  -- Correct
/*				-1 -- Error
/*
/*****************************************************************************/
int ChangeFileExt ( string	&dest, string src, char *ext )
{
    int	DotPos = src.find( '.' );
	if( DotPos != -1 )
	{
		dest = src.substr(0, DotPos+1 );
		dest.insert( DotPos+1, ext );
	}
	if( DotPos == -1 )
		dest = src + ext;
    return 0 ;
}

/******************************************************************************
/*	Name:		GetFileExt
/*	Function:	Get file extension
/*	Parameter:	FileName -- File name
/*	Return:		File extension
/*
/*****************************************************************************/
string GetFileExtension ( string FileName )
{
    int pos ;
    if ( (pos=FileName.find('.')) >= 0 )
        return FileName.substr(pos+1) ;
    else
        return "" ;
}

/******************************************************************************
/*	Name:		GetDllName
/*	Function:	Get DLL file name from a string
/*	Parameter:	strFileName -- A string
/*	Return:		DLL file name
/*
/*****************************************************************************/
string GetDllName( string strFileName )
{
	//Zheng Y.F.    Aug 25th
	//Parse "PostProcesser" section of CELL definition. Get DLL name "E:\\Zhengyf\\PostProc.dll".
	//Sample: "E:\Zhengyf\PostProc.dll->Function1"
	int pos;
	if( (pos=strFileName.find("->")) >=0)
		return strFileName.substr (0, pos );
	else
		return "";
}

/******************************************************************************
/*	Name:		GetDllFuncName
/*	Function:	Get function name of the DLL
/*	Parameter:	strFileName -- A string
/*	Return:		Function name
/*
/*****************************************************************************/
string GetDllFuncName ( string strFileName )
{
	//Zheng Y.F.   Aug 25th
	//Parse "PostProcesser" section of CELL definition. Get DLL Function "Function1".
	//Sample: "E:\Zhengyf\PostProc.dll->Function1"
	int pos;
	if( (pos = strFileName.find("->")) >= 0 )
		return	strFileName.substr ( pos+2 );
	else 
		return "PostProc";
}

//////////////////////////////////////////////////////////////////////////////////////
//	Data manipulation functions
///////////////////////////////////////////////////////////////////////////////////////

/*************************************************************************************
/*	Name:		SortData
/*	Function:	Sort data ascendly or decendly, interface for 'int' type
/*	Parameter:	Data -- Data
/*				Num  -- Number of data
/*				bIncrease -- TRUE  ascendly
/*							 FALSE descendly	
/*	Return:		void
/*
/**************************************************************************************/
void SortData( int  Data[], int Num, int bIncrease )
{
	int i, j, tmp;	
	if( !bIncrease )
	{
		int Max;
		for( i=0; i<Num; i++ )
		{
			Max = i;
			for( j=i+1; j<Num; j++ )
				if( Data[j] > Data[Max] )
					Max = j;
			if( Max != i )
			{
				tmp = Data[i];
				Data[i] = Data[Max];
				Data[Max] = tmp;
			}
		}
	}
	else
	{
		int Min;
		for( i=0; i<Num; i++ )
		{
			Min = i;
			for( j=i+1; j<Num; j++ )
				if( Data[j] < Data[Min] )
					Min = j;
			if( Min != i )
			{
				tmp = Data[i];
				Data[i] = Data[Min];
				Data[Min] = tmp;
			}
		}
	}
	return ;
}

/*************************************************************************************
/*	Name:		SortData
/*	Function:	Sort data ascendly or decendly, interface for 'double' type
/*	Parameter:	Data -- Data
/*				Num  -- Number of data
/*				bIncrease -- TRUE  ascendly
/*							 FALSE descendly	
/*	Return:		void
/*
/**************************************************************************************/
void SortData( double  Data[], int Num, int bIncrease )
{
	int i, j;
	double tmp;
	if( !bIncrease )
	{
		int Max;
		for( i=0; i<Num; i++ )
		{
			Max = i;
			for( j=i+1; j<Num; j++ )
				if( Data[j] > Data[Max] )
					Max = j;
			if( Max != i )
			{
				tmp = Data[i];
				Data[i] = Data[Max];
				Data[Max] = tmp;
			}
		}
	}
	else
	{
		int Min;
		for( i=0; i<Num; i++ )
		{
			Min = i;
			for( j=i+1; j<Num; j++ )
				if( Data[j] < Data[Min] )
					Min = j;
			if( Min != i )
			{
				tmp = Data[i];
				Data[i] = Data[Min];
				Data[Min] = tmp;
			}
		}
	}
	return ;
}

/*************************************************************************************
/*	Name:		SortData
/*	Function:	Sort data ascendly or decendly, interface for 'float' type
/*	Parameter:	Data -- Data
/*				Num  -- Number of data
/*				bIncrease -- TRUE  ascendly
/*							 FALSE descendly	
/*	Return:		void
/*
/**************************************************************************************/
void SortData( float  Data[], int Num, int bIncrease )
{
	int i, j;
	float tmp;
	if( !bIncrease )
	{
		int Max;
		for( i=0; i<Num; i++ )
		{
			Max = i;
			for( j=i+1; j<Num; j++ )
				if( Data[j] > Data[Max] )
					Max = j;
			if( Max != i )
			{
				tmp = Data[i];
				Data[i] = Data[Max];
				Data[Max] = tmp;
			}
		}
	}
	else
	{
		int Min;
		for( i=0; i<Num; i++ )
		{
			Min = i;
			for( j=i+1; j<Num; j++ )
				if( Data[j] < Data[Min] )
					Min = j;
			if( Min != i )
			{
				tmp = Data[i];
				Data[i] = Data[Min];
				Data[Min] = tmp;
			}
		}
	}
	return ;
}

/*************************************************************************************
/*	Name:		GetMidValue
/*	Function:	Get the median value of an array, interface for 'int' type
/*	Parameter:	Value -- Data
/*				nTotal -- Number of data
/*	Return:		Middel value
/*
/**************************************************************************************/
int GetMidValue ( int *Value, int nTotal )
{
    int i, j ;
    int *Greater = new int[nTotal] ;

    for ( i=0 ; i<nTotal ; i++ )
        Greater[i] = 0 ;

    for ( i=0 ; i<nTotal-1 ; i++ )
        for ( j=i+1 ; j<nTotal ; j++ )
            if ( Value[i] != Value[j] )
            {
                if ( Value[i] > Value[j] )
                    Greater[i]++ ;
                else
                    Greater[j]++ ;
            }

    int Mid = Value[0] ;
    for ( i=0 ; i<nTotal ; i++ )
        if ( Value[i] < Mid )
            Mid = Value[i] ;
    for ( i=0 ; i<nTotal ; i++ )
        if ( Value[i]>Mid && Greater[i]<=nTotal/2 )
            Mid = Value[i] ;
    delete Greater ;
    return Mid ;
}

/*************************************************************************************
/*	Name:		GetMidValue
/*	Function:	Get the median value of an array, interface for 'double' type
/*	Parameter:	Value -- Data
/*				nTotal -- Number of data
/*	Return:		Middel value
/*
/**************************************************************************************/
double GetMidValue ( double *Value, int nTotal )
{
    int i, j ;
    int *Greater = new int[nTotal] ;

    for ( i=0 ; i<nTotal ; i++ )
        Greater[i] = 0 ;

    for ( i=0 ; i<nTotal-1 ; i++ )
        for ( j=i+1 ; j<nTotal ; j++ )
            if ( Value[i] != Value[j] )
            {
                if ( Value[i] > Value[j] )
                    Greater[i]++ ;
                else
                    Greater[j]++ ;
            }

    double Mid = Value[0] ;
    for ( i=1 ; i<nTotal ; i++ )
        if ( Value[i] < Mid )
            Mid = Value[i] ;
    for ( i=0 ; i<nTotal ; i++ )
        if ( Value[i]>Mid && Greater[i]<=nTotal/2 )
            Mid = Value[i] ;
    delete Greater ;
    return Mid ;
}

/*
 *  This Quickselect routine is based on the algorithm described in
 *  "Numerical recipes in C", Second Edition,
 *  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
 *  This code by Nicolas Devillard - 1998. Public domain.
 /*	Warning: the routine will change the value in the array!!!
 */
double GetMidValue_Quick(double *arr, int n)
{
#define ELEM_SWAP(a,b) { register double t=(a);(a)=(b);(b)=t; }
    int low, high ;
    int median;
    int middle, ll, hh;

    low = 0 ; high = n-1 ; median = (low + high) / 2;
    for (;;) {
        if (high <= low) /* One element only */
            return arr[median] ;

        if (high == low + 1) {  /* Two elements only */
            if (arr[low] > arr[high])
                ELEM_SWAP(arr[low], arr[high]) ;
            return arr[median] ;
        }

    /* Find median of low, middle and high items; swap into position low */
    middle = (low + high) / 2;
    if (arr[middle] > arr[high])    ELEM_SWAP(arr[middle], arr[high]) ;
    if (arr[low] > arr[high])       ELEM_SWAP(arr[low], arr[high]) ;
    if (arr[middle] > arr[low])     ELEM_SWAP(arr[middle], arr[low]) ;

    /* Swap low item (now in position middle) into position (low+1) */
    ELEM_SWAP(arr[middle], arr[low+1]) ;

    /* Nibble from each end towards middle, swapping items when stuck */
    ll = low + 1;
    hh = high;
    for (;;) {
        do ll++; while (arr[low] > arr[ll]) ;
        do hh--; while (arr[hh]  > arr[low]) ;

        if (hh < ll)
        break;

        ELEM_SWAP(arr[ll], arr[hh]) ;
    }

    /* Swap middle item (in position low) back into correct position */
    ELEM_SWAP(arr[low], arr[hh]) ;

    /* Re-set active partition */
    if (hh <= median)
        low = ll;
        if (hh >= median)
        high = hh - 1;
    }
}

////////////////////////////////////////////////////////////////////////////////
//	Position relationship of points and lines
////////////////////////////////////////////////////////////////////////////////

/*************************************************************************************
/*	Name:		ShiftPoint
/*	Function:	Shift a point with offset
/*	Parameter:	p -- Original point
/*				dx -- X shift
/*				dy -- Y shift
/*	Return:		Shifted point
/*
/**************************************************************************************/
DSCC_POINT ShiftPoint ( DSCC_POINT p, int dx, int dy )
{
    DSCC_POINT pr ;
    pr.x = p.x + dx ;
    pr.y = p.y + dy ;
    return pr ;
}

/*************************************************************************************
/*	Name:		TransformPoint
/*	Function:	Transform a point cardinary with shift and rotate parameter
/*	Parameter:	p -- Original point
/*				Param -- Transform parameters
/*	Return:		Transformed parameter
/*
/**************************************************************************************/
DSCC_POINT TransformPoint ( DSCC_POINT p, TRANSPARAM Param )
{
	DSCC_POINT pr ;
	if(Param.Type == 0)
	{
		p.x += Param.dx ;
		p.y += Param.dy ;
		double dx = p.x - Param.Center.x ;
		double dy = p.y - Param.Center.y ;
		double r = sqrt (dx*dx+dy*dy) ;
		double a = atan2 ( -dy, dx ) ;
		pr.x = (int) (Param.Center.x+r*cos(a+Param.Angle)) ;
		pr.y = (int) (Param.Center.y-r*sin(a+Param.Angle)) ;
	}
	else
	{
		pr.x = (long)(Param.a*p.x + Param.b*p.y + Param.x0 + Param.e*p.x*p.y+0.5);
		pr.y = (long)(Param.c*p.x + Param.d*p.y + Param.y0 + Param.f*p.x*p.y+0.5);
	}
	return pr;
}

/*************************************************************************************
/*	Name:		GetAngle
/*	Function:	Calculate the angle of the line formed by two points
/*	Parameter:	Start -- Starting point
/*				End -- Ending point
/*	Return:		Angle of the line
/*
/**************************************************************************************/
double GetAngle ( DSCC_POINT Start, DSCC_POINT End )
{
    if ( End.x != Start.x )
    {
        double atg = atan ( ((double)(Start.y-End.y)) / ((double)(End.x-Start.x)) ) ;
        if ( End.x > Start.x )
            return atg ;
        else
        {
            if ( End.y < Start.y )
                return atg+PI ;
            else
                return atg-PI ;
        }
    }
    else
    {
        if ( End.y < Start.y )
            return PI/2 ;
        else if ( End.y > Start.y )
            return -PI/2 ;
        else
            return 0 ;
    }
}

/*************************************************************************************
/*	Name:		GetDistance
/*	Function:	Get distance of two points, interface for 'doulbe'
/*	Parameter:	x1 -- X ordinate of point 1
/*				y1 -- Y ordinate of point 1
/*				x2 -- X ordinate of point 2
/*				y2 -- Y ordinate of point 2
/*	Return:		Distance of two points
/*
/**************************************************************************************/
double GetDistance ( double x1, double y1, double x2, double y2 )
{
    double dx = x1 - x2;
    double dy = y1 - y2 ;
    return sqrt( dx*dx + dy*dy ) ;
}

/*************************************************************************************
/*	Name:		GetDistance
/*	Function:	Get distance of two points
/*	Parameter:	pnt1 -- Point 1
/*				pnt2 -- Point 2
/*	Return:		Distance of two points
/*
/**************************************************************************************/
double GetDistance ( const DSCC_POINT &pnt1, const DSCC_POINT &pnt2 )
{
    int dx = pnt1.x - pnt2.x ;
    int dy = pnt1.y - pnt2.y ;
    return sqrt((double)( dx*dx + dy*dy ));
}

/*************************************************************************************
/*	Name:		GetDistance
/*	Function:	Get the distance from a point to a line formed by two points
/*	Parameter:	pnt	-- Point
/*				StPnt -- Starting point of the line
/*				EdPnt -- Ending point of the line
/*	Return:		Distance of a point to a line
/*
/**************************************************************************************/
double GetDistance(const DSCC_POINT &pnt, const DSCC_POINT &StPnt,const DSCC_POINT &EdPnt)
{
	double a,b;
	if(EdPnt.x == StPnt.x) 
		return (int)fabs((float)(pnt.x - StPnt.x));
	if(EdPnt.y == StPnt.y)
		return (int)fabs((float)(pnt.y - StPnt.y));
	a = (EdPnt.y - StPnt.y)*1.0/(EdPnt.x - StPnt.x);
	b = StPnt.y - a*StPnt.x;
	DSCC_POINT pnt2;
	double  x2, y2;
	x2 = (pnt.x + a*pnt.y -a*b)*1.0/(a*a+1);
	y2 = a*x2 + b;
	pnt2.x = (long)x2;
	pnt2.y = (long)y2;
	return GetDistance(pnt,pnt2);
}

/*************************************************************************************
/*	Name:		GetCrossPoint
/*	Function:	Get cross point of a point to a line formed by two points
/*	Parameter:	pnt	-- Point
/*				StPnt -- Starting point of the line
/*				EdPnt -- Ending point of the line
/*	Return:		Cross point of a point to a line
/*
/**************************************************************************************/
DSCC_POINT GetCrossPoint(DSCC_POINT &pnt, DSCC_POINT &StPnt, DSCC_POINT &EdPnt)
{
	DSCC_POINT pnt2;
	if(EdPnt.x == StPnt.x) 
	{
		pnt2.x = EdPnt.x;
		pnt2.y = pnt.y;
		return pnt2;
	}
	if(EdPnt.y == StPnt.y)
	{
		pnt2.x = pnt.x;
		pnt2.y = EdPnt.y;
		return pnt2;
	}
	double a,b;
	a = (EdPnt.y - StPnt.y)*1.0/(EdPnt.x - StPnt.x);
	b = StPnt.y - a*StPnt.x;
	double  x2, y2;
	x2 = (pnt.x + a*pnt.y -a*b)*1.0/(a*a+1);
	y2 = a*x2 + b;
	pnt2.x = (long)x2;
	pnt2.y = (long)y2;
	return pnt2;
}

/*************************************************************************************
/*	Name:		GetCrossPoint
/*	Function:	Get cross point of two lines
/*	Parameter:	x1	-- X ordinate of starting point of line 1
/*				y1	-- Y ordinate of starting point of line 1
/*				x2	-- X ordinate of ending point of line 1
/*				y2	-- Y ordinate of ending point of line 1
/*				_x1	-- X ordinate of starting point of line 2
/*				_y1	-- Y ordinate of starting point of line 2
/*				_x2	-- X ordinate of ending point of line 2
/*				_y2	-- Y ordinate of ending point of line 2
/*				CrossPnt -- Cross point
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*
/**************************************************************************************/
int GetCrossPoint ( double x1, double y1, double x2, double y2, 
	double _x1, double _y1, double _x2, double _y2,  DSCC_POINT &CrossPnt)
{
    double dx, _dx, dy, _dy, x, y ;
    dx=x2-x1 ; dy=y2-y1 ; _dx=_x2-_x1 ; _dy=_y2-_y1 ;
  	if(_dx*dy == dx*_dy)	return -1;
	if( dx == 0)
	{
		x = x1;
		y = _y1+(x1-_x1)*_dy / _dx;
	}
	else
	{
		x = ( dx*_dx*(_y1-y1) - dx*_dy*_x1 + _dx*dy*x1 ) / (_dx*dy - dx*_dy) ;
		y = (dy*(x-x1)+y1*dx) / dx ;
	}
	CrossPnt.x=(int)(x + 0.5) ; CrossPnt.y=(int)(y + 0.5) ;  
    return 0 ;
}

/*************************************************************************************
/*	Name:		GetCrossPoint
/*	Function:	Get cross point of two lines
/*	Parameter:	StPnt1 -- Starting point of line 1
/*				EdPnt1 -- Ending point of line 1
/*				StPnt3	-- Starting point of line 2
/*				EdPnt2	-- Ending point of line 2
/*	Return:		Cross point
/*
/**************************************************************************************/
DSCC_POINT GetCrossPoint(DSCC_POINT StPnt1, DSCC_POINT EdPnt1, DSCC_POINT StPnt2, DSCC_POINT EdPnt2)
{
    DSCC_POINT	CrossPnt;  
	CrossPnt.x = -1;
	CrossPnt.y = -1;
	double dx, _dx, dy, _dy, x, y ;
  	dx = EdPnt1.x - StPnt1.x;
	dy = EdPnt1.y - StPnt1.y;
	_dx = EdPnt2.x - StPnt2.x;
	_dy = EdPnt2.y - StPnt2.y;
	
	if(_dx*dy == dx*_dy)	return  CrossPnt;
	if( dx == 0)
	{
		x = StPnt1.x;
		y = StPnt2.y+(StPnt1.x-StPnt2.x)*_dy / _dx;
	}
	else
	{
		x = ( dx*_dx*(StPnt2.y - StPnt1.y) - dx*_dy*StPnt2.x + _dx*dy*StPnt1.x ) / (_dx*dy - dx*_dy) ;
		y = (dy*(x-StPnt1.x)+StPnt1.y*dx) / dx ;
	}
	CrossPnt.x=(int)(x + 0.5) ; CrossPnt.y=(int)(y + 0.5) ;  
    return CrossPnt ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//	Other function
///////////////////////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
/*	Name:		ParsePosition
/*	Function:	Get four position from a string
/*	Parameter:	s -- Input string
/*				left -- Left position, changed after calling
/*				top -- Top position, changed after calling
/*				right -- Right position, changed after calling
/*				bottom -- Bottom position, changed after calling
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int ParsePosition ( string s, int &left, int &top, int &right, int &bottom, int type )
{//type=0 --- Line     type=1 --- cell	type=2 --- do not sort
    int n, m ; 
    m=0 ;
    if ( (n=s.find(',',m))<=0)
        return -1 ;
    left = atoi ( s.substr(m,n-m).c_str() ) ;
    m=n+1 ;
    if ( (n=s.find(',',m))<=0)
        return -2 ;
    top = atoi ( s.substr(m,n-m).c_str() ) ;
    m=n+1 ;
    if ( (n=s.find(',',m))<=0)
        return -3 ;
    right = atoi ( s.substr(m,n-m).c_str() ) ;
    if ( n == s.length()-1 )
        return -4 ;
    bottom = atoi ( s.substr(1+n).c_str() ) ;
	int tmp;
	if( type == 0 )
	{
		if( (abs(left-right) > abs(top-bottom) && (left > right) ) ||
			(abs(left-right) < abs(top-bottom) && (top > bottom) ) )
		{
				tmp = left;		left = right;	right = tmp;
				tmp = top;		top	 = bottom;	bottom = tmp;
		}
	}
    return 0 ;
}

/******************************************************************************
/*	Name:		ParseCenter
/*	Function:	Get a point position from a string
/*	Parameter:	s -- Input string
/*				x -- X ordinate of the point, changed after calling
/*				Y -- Y ordinate of the point, changed after calling
/*	Return:		0  -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int ParseCenter( string s, int &x, int &y)
{	int pos1, pos2;
	pos1 = 0;
	if ( (pos2 = s.find(',', pos1)) <= 0)	return -1;
	x = atoi ( s.substr(pos1, pos2 - pos1).c_str());
	y = atoi ( s.substr( pos2+1 ).c_str());
	return 0;
}

/******************************************************************************
/*	Name:		BlackPixelsIsInByte
/*	Function:	Initialize array lut[0-255], which calculate the number of
/*				black pixels in a byte
/*	Parameter:	void
/*	Return:		void
/*****************************************************************************/
void BlackPixelsInByte()
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

///////////////////////////////////////////////////////////////////////////////////////
//	Matrix operation
////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************
/*	Name:		MatrixMultiply
/*	Function:	Multiply two matrices
/*	Parameter:	a -- Matrix m*n
/*				b -- Matrix n*k
/*				c -- Matrix a*b, it is m*k
/*				m -- Dimension 
/*				n -- Dimension
/*				k -- Dimension
/*	Return:		0 -- Succeed
/*				-1 -- Error
/*****************************************************************************/
double	MatrixMultiply( double *a, double *b, double *c, int m, int n, int k)
{
	int i, j, l;
	for( i=0; i<m; i++ )
	{
		for( j=0; j<k; j++ )
		{
			c[i*k+j] = 0;
			for( l=0; l<n; l++ )
				c[i*k+j] += a[i*n+l]*b[l*k+j];
		}
	}
	return 0;
}

/******************************************************************************
/*	Name:		Inv
/*	Function:	Inverse a matrix
/*	Parameter:	a -- Matrix, changed after inversion
/*				n -- Dimension of the matrix
/*	Return:		0 -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int Inv(double a[], int n)
{//Invert Matrix 
	int *is,*js,i,j,k,l,u,v;
    double d,p;
    is=(int*)malloc(n*sizeof(int));
    js=(int*)malloc(n*sizeof(int));
    for (k=0; k<=n-1; k++)
      { d=0.0;
        for (i=k; i<=n-1; i++)
        for (j=k; j<=n-1; j++)
          { l=i*n+j; p=fabs(a[l]);
            if (p>d) { d=p; is[k]=i; js[k]=j;}
          }
        if (d+1.0==1.0)
          { 
			free(is); free(js); printf("err**not inv\n");
            return -1;
          }
        if (is[k]!=k)
          for (j=0; j<=n-1; j++)
            { u=k*n+j; v=is[k]*n+j;
              p=a[u]; a[u]=a[v]; a[v]=p;
            }
        if (js[k]!=k)
          for (i=0; i<=n-1; i++)
            { u=i*n+k; v=i*n+js[k];
              p=a[u]; a[u]=a[v]; a[v]=p;
            }
        l=k*n+k;
        a[l]=1.0/a[l];
        for (j=0; j<=n-1; j++)
          if (j!=k)
            { u=k*n+j; a[u]=a[u]*a[l];}
        for (i=0; i<=n-1; i++)
          if (i!=k)
            for (j=0; j<=n-1; j++)
              if (j!=k)
                { u=i*n+j;
                  a[u]=a[u]-a[i*n+k]*a[k*n+j];
                }
        for (i=0; i<=n-1; i++)
          if (i!=k)
            { u=i*n+k; a[u]=-a[u]*a[l];}
      }
    for (k=n-1; k>=0; k--)
      { if (js[k]!=k)
          for (j=0; j<=n-1; j++)
            { u=k*n+j; v=js[k]*n+j;
              p=a[u]; a[u]=a[v]; a[v]=p;
            }
        if (is[k]!=k)
          for (i=0; i<=n-1; i++)
            { u=i*n+k; v=i*n+is[k];
              p=a[u]; a[u]=a[v]; a[v]=p;
            }
      }
    free(is); free(js);
    return 0;
}

/******************************************************************************
/*	Name:		TransMatrix
/*	Function:	Transpose a matrix
/*	Parameter:	A -- Matrix
/*				M -- Row Dimension
/*				N -- Column Dimension
/*				B -- Transposed matrix
/*	Return:		0 -- Succeed
/*****************************************************************************/
int	TransMatrix( double* A, int M, int N, double* B )
{
	int	 i, j;
	for( i=0; i<M; i++ )
		for( j=0; j<N; j++ )
			B[j*M+i] = A[i*N+j];
	return 0;
}
/******************************************************************************
/*	Name:		Mean
/*	Function:	Calculate feature mean
/*	Parameter:	Feature -- Feature vectors
/*				Num -- Feature vector number
/*				MeanFeature -- Mean feature
/*	Return:		0 -- Succeed
/*****************************************************************************/
int	Mean( double *Feature, int Num, double &MeanFeature )
{
	if( Num <= 0 )	return -1;
	MeanFeature = 0;
	int i;
	for(i=0; i<Num; i++ )
		MeanFeature += Feature[i];
	MeanFeature /= Num;
	return 0;
}

/******************************************************************************
/*	Name:		Var
/*	Function:	Calculate variance matrix of feature
/*	Parameter:	Feature -- Feature vectors
/*				Num -- Feature vector number
/*				MeanFeature -- Mean feature
/*				Var -- Variance
/*	Return:		0 -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int Var( double *Feature, int Num, double &Var, double &MeanFeature )
{
	int	i;
	Var = 0;
	for( i=0; i<Num; i++ )
		Var += ( Feature[i]-MeanFeature ) * (Feature[i]-MeanFeature );
	Var = sqrt( Var/Num );
	return 0;
}

/******************************************************************************
/*	Name:		Mean
/*	Function:	Calculate feature mean
/*	Parameter:	Feature -- Feature vectors
/*				Num -- Feature vector number
/*				Dimension -- Feature dimension
/*				MeanFeature -- Mean feature
/*	Return:		0 -- Succeed
/*****************************************************************************/
int	Mean( double **Feature, int Num, int Dimension, double *MeanFeature )
{
	if( Num <= 0 )	return -1;
	memset( MeanFeature,0, Dimension*sizeof(double));
	int i, j;
	for(i=0; i<Num; i++ )
		for( j=0; j<Dimension; j++ )
			MeanFeature[j] += Feature[i][j];
	for( i=0; i<Dimension; i++ )
		MeanFeature[i] /= Num;
	return 0;
}

/******************************************************************************
/*	Name:		Var
/*	Function:	Calculate variance matrix of feature
/*	Parameter:	Feature -- Feature vectors
/*				Num -- Feature vector number
/*				Dimension -- Feature dimension
/*				MeanFeature -- Mean feature
/*				Var -- Variance matrix
/*	Return:		0 -- Succeed
/*				-1 -- Error
/*****************************************************************************/
int Var( double **Feature, int Num, int Dimension, double *Var, double *MeanFeature )
{
	int	bHasMean = TRUE;
	if( MeanFeature == NULL )
	{
		bHasMean = FALSE;
		MeanFeature = (double*)malloc(Dimension*sizeof(double) );
		if( MeanFeature == NULL )
		{
			cout << "Memory used up!" << endl;
			return -1;
		}
		Mean( Feature, Num, Dimension, MeanFeature );
	}
	int		i, j, k;
	memset( Var,0, sizeof(double)*Dimension*Dimension);
	for( i=0; i<Num; i++ )
		for( j=0; j<Dimension; j++ )
			for( k=0; k<Dimension; k++ )
				Var[j*Dimension+k] += ( Feature[i][j]-MeanFeature[j] ) * (Feature[i][k]-MeanFeature[k] );
	if( bHasMean == FALSE )
		free( MeanFeature );
	return 0;
}

/******************************************************************************
/*	Name:		det
/*	Function:	Calculate determinate of a matrix
/*	Parameter:	a -- Matrix
/*				n -- Dimension
/*	Return:		Determinate of the matrix
/*****************************************************************************/
double det(double a[], int n)
{
	int i,j,k,is,js,l,u,v;
    double f,det,q,d;
    f=1.0; det=1.0;
    for (k=0; k<=n-2; k++)
      { q=0.0;
        for (i=k; i<=n-1; i++)
        for (j=k; j<=n-1; j++)
          { l=i*n+j; d=fabs(a[l]);
	    if (d>q) { q=d; is=i; js=j;}
          }
        if (q+1.0==1.0)
          { det=0.0; return(det);}
        if (is!=k)
          { f=-f;
            for (j=k; j<=n-1; j++)
              { u=k*n+j; v=is*n+j;
                d=a[u]; a[u]=a[v]; a[v]=d;
              }
          }
        if (js!=k)
          { f=-f;
            for (i=k; i<=n-1; i++)
              { u=i*n+js; v=i*n+k;
                d=a[u]; a[u]=a[v]; a[v]=d;
              }
          }
        l=k*n+k;
        det=det*a[l];
        for (i=k+1; i<=n-1; i++)
          { d=a[i*n+k]/a[l];
            for (j=k+1; j<=n-1; j++)
              { u=i*n+j;
                a[u]=a[u]-d*a[k*n+j];
              }
          }
      }
    det=f*det*a[n*n-1];
    return(det);
}

/******************************************************************************
/*	Name:		SearchPeak
/*	Function:	Search peaks in a histogram
/*	Parameter:	Hist -- Histogram
/*				nHist -- Number of histogram
/*				LeftPeak -- Left peaks
/*				RightPeak -- Right peaks
/*				nPeak -- Number of peaks
/*	Return:		0  -- Succeed
/*				-1 -- Failed
/*****************************************************************************/
int	SearchPeak( double *Hist, int nHist, int *&LeftPeak, int *&RightPeak, int &nPeak)
{
	int	nMaxPeak = 20;
	LeftPeak = (int*)malloc(sizeof(int)*nMaxPeak );
	RightPeak = (int*)malloc(sizeof(int)*nMaxPeak );
	if( LeftPeak == NULL || RightPeak == NULL )
		return -1;
	int		nIncreaseMode = -1; //-1 -- uninitailzed; 0 -- decreasing; 1 -- increasing
	int		i, j;
	nPeak = 0;
	for( i=0; i<nHist-1; i++ )
	{
		if( nIncreaseMode == -1 )
		{
			if( Hist[i+1] < Hist[i] )
			{
				RightPeak[nPeak] = i;
				j = i;
				while( j>0 )
				{
					if( Hist[j-1] < Hist[j] )
						break;
					j--;
				}
				LeftPeak[nPeak] = j;
				nPeak++;
				nIncreaseMode = 0;
			}
			else if( Hist[i+1] > Hist[i] )
				nIncreaseMode = 1;
		}
		else
		{
			if( Hist[i+1] < Hist[i] ) 
			{
				if( nIncreaseMode == 1 )
				{
					RightPeak[nPeak] = i;
					j = i;
					while( j>0 )
					{
						if( Hist[j-1] < Hist[j] )
							break;
						j--;
					}
					LeftPeak[nPeak] = j;
					nPeak++;
					if( nPeak >= nMaxPeak )
					{
						nMaxPeak += 10;
						LeftPeak = (int*)realloc(LeftPeak, sizeof(int)*nMaxPeak );
						RightPeak = (int*)realloc(RightPeak, sizeof(int)*nMaxPeak );
						if( LeftPeak == NULL || RightPeak == NULL )
							return -1;
					}
					nIncreaseMode = 0;
				}
			}
			else if( Hist[i+1] > Hist[i] )
				nIncreaseMode = 1;
		}
	}
	if( nIncreaseMode == 1 )
	{
		RightPeak[nPeak] = nHist-1;
		j = nHist-1;
		while( j>0 )
		{
			if( Hist[j-1] < Hist[j] )
				break;
			j--;
		}
		LeftPeak[nPeak] = j;
		nPeak++;
	}
	return 0;
}

/******************************************************************************
/*	Name:		SearchValley
/*	Function:	Search valleys in a histogram
/*	Parameter:	Hist -- Histogram
/*				nHist -- Number of histogram
/*				LeftValley -- Left valleys
/*				RightValley -- Right valleys
/*				nValley -- Number of valleys
/*	Return:		0  -- Succeed
/*				-1 -- Failed
/*****************************************************************************/
int	SearchValley( double *Hist, int nHist, int *&LeftValley, int *&RightValley, int &nValley)
{
	int	nMaxValley = 20;
	LeftValley = (int*)malloc(sizeof(int)*nMaxValley );
	RightValley = (int*)malloc(sizeof(int)*nMaxValley );
	if( LeftValley == NULL || RightValley == NULL )
		return -1;
	int		nIncreaseMode = -1; //-1 -- uninitailzed; 0 -- decreasing; 1 -- increasing
	int		i, j;
	nValley = 0;
	for( i=0; i<nHist-1; i++ )
	{
		if( nIncreaseMode == -1 )
		{
			if( Hist[i+1] > Hist[i] )
			{
				RightValley[nValley] = i;
				j = i;
				while( j>0 )
				{
					if( Hist[j-1] > Hist[j] )
						break;
					j--;
				}
				LeftValley[nValley] = j;
				nValley++;
				nIncreaseMode = 1;
			}
			else if( Hist[i+1] < Hist[i] )
				nIncreaseMode = 0;
		}
		else
		{
			if( Hist[i+1] > Hist[i] ) 
			{
				if( nIncreaseMode == 0 )
				{
					RightValley[nValley] = i;
					j = i;
					while( j>0 )
					{
						if( Hist[j-1] > Hist[j] )
							break;
						j--;
					}
					LeftValley[nValley] = j;
					nValley++;
					if( nValley >= nMaxValley )
					{
						nMaxValley += 10;
						LeftValley = (int*)realloc(LeftValley, sizeof(int)*nMaxValley );
						RightValley = (int*)realloc(RightValley, sizeof(int)*nMaxValley );
						if( LeftValley == NULL || RightValley == NULL )
							return -1;
					}
					nIncreaseMode = 1;
				}
			}
			else if( Hist[i+1] < Hist[i] )
				nIncreaseMode = 0;
		}
	}
	if( nIncreaseMode == 0 )
	{
		RightValley[nValley] = nHist-1;
		j = nHist-1;
		while( j>0 )
		{
			if( Hist[j-1] > Hist[j] )
				break;
			j--;
		}
		LeftValley[nValley] = j;
		nValley++;
	}
	return 0;
}

/******************************************************************************
/*	Name:		GetPeakWidth2
/*	Function:	Calculate the width of a peak of a histogram, do not consider 
/*				background noise
/*	Parameter:	Hist		-- Histogram
/*				nHist		-- Number of histogram
/*				nPeakPos	-- Peak position
/*	Return:		Peak width
/*****************************************************************************/
int	GetPeakWidth2( double *Hist, int nHist, int nPeakPos )
{
	int nLeft = nPeakPos;
	for( ; nLeft>=0; nLeft-- )
	{
		if( Hist[nLeft] < Hist[nPeakPos]/2 )
			break;
	}
	int nRight = nPeakPos + 1;
	for( ; nRight<nHist; nRight++ )
	{
		if( Hist[nRight] < Hist[nPeakPos]/2 )
			break;
	}
	return nRight-nLeft+1;
}

/******************************************************************************
/*	Name:		GetPeakWidth
/*	Function:	Calculate the width of a peak of a histogram
/*	Parameter:	Hist		-- Histogram
/*				nHist		-- Number of histogram
/*				nPeakPos	-- Peak position
/*	Return:		Peak width
/*****************************************************************************/
int	GetPeakWidth( double *Hist, int nHist, int nPeakPos )
{
	int nLeft = nPeakPos;
	for( ; nLeft>=2; nLeft-- )
	{
		if( Hist[nLeft] < Hist[nLeft-1] && Hist[nLeft] < Hist[nLeft-2] )
			break;
	}
	int nRight = nPeakPos;
	for( ; nRight<nHist-2; nRight++ )
	{
		if( Hist[nRight] < Hist[nRight+1] && Hist[nRight] < Hist[nRight+2] )
			break;
	}
	int i = nPeakPos;
	for( ; i>nLeft; i-- )
	{
		if( Hist[nPeakPos]-Hist[i] > (Hist[nPeakPos]-Hist[nLeft]) / sqrt((double)2.0) )
			break;
	}
	int j = nPeakPos;
	for( ; j<nRight; j++ )
	{
		if( Hist[nPeakPos]-Hist[j] > (Hist[nPeakPos]-Hist[nRight]) / sqrt((double)2.0) )
			break;
	}
	return j-i+1;
}

/******************************************************************************
/*	Name:		GetPeakHeight
/*	Function:	Calculate the height of a peak of a histogram
/*	Parameter:	Hist		-- Histogram
/*				nHist		-- Number of histogram
/*				nPeakPos	-- Peak position
/*	Return:		Peak height
/*****************************************************************************/
int GetPeakHeight( double *Hist, int nHist, int nPeakPos )
{
	int nLeft = nPeakPos;
	for( ; nLeft>=2; nLeft-- )
	{
		if( Hist[nLeft] < Hist[nLeft-1] && Hist[nLeft] < Hist[nLeft-2] )
			break;
	}
	int nRight = nPeakPos;
	for( ; nRight<nHist-2; nRight++ )
	{
		if( Hist[nRight] < Hist[nRight+1] && Hist[nRight] < Hist[nRight+2] )
			break;
	}
	int	nPeakHeight = (int)(2*Hist[nPeakPos] - Hist[nLeft] - Hist[nRight]);
	return nPeakHeight;
}
