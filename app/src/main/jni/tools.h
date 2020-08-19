/////////////////////////////////////////////////////////////////////////
// File NAME:		Tools.h
// File Function:	Commnon functions for other modules
//
//				Developed by: Yefeng Zheng
//				First Created: June 2001
//			University of Maryland, College Park
//					All Right Reserved
/////////////////////////////////////////////////////////////////////////

#ifndef __TOOLS_H__
#define __TOOLS_H__
#include "convertMFC.h"
#include <iostream>

using namespace std;

#ifndef	PI
#define PI  3.1415926535
#endif

typedef struct TransParamStruc
{
	//Type==0  Linear transform, use dx, dy, Center, Angle to locate a form cell
	//Type==1 Bi-linear transform, use x0, y0, a,b,c,d,e,f to locate a form cell
	int		Type;	

	//Linear transform
	int		dx, dy;
	DSCC_POINT	Center;
	double	Angle;	
	//Bi-linear transform
	double x0, y0; 
	double a,b,c,d,e,f;

} TRANSPARAM;
////////////////////////////////////////////////////////////////////////////////
//	File name manipulation functions
////////////////////////////////////////////////////////////////////////////////
//Change file extension
int			ChangeFileExt ( char *dest, char *src, char *ext ) ;
//Change file extension, another interface
int			ChangeFileExt ( string	&dest, string src, char *ext );
//Get file extension
string		GetFileExtension ( string FileName ) ;
//Get file name from a string
string	GetFileName ( string strFileName );
//Get file name (without extension) from a string
string GetFileNameWithoutExt ( string strFileName );
//Get file path from a string
string		GetPathName ( string strFileName );
//Get DLL name from a string
string		GetDllName( string strFileName );
//Get function name of a DLL file from a string
string		GetDllFuncName ( string strFileName );
//Test if a file exist or not
//int		FileExist( CString strFileName );
//Test if a directory exist or not
//int		DirExist( wstring csDir );
///////////////////////////////////////////////////////////////////////////////
//	Sort data
//////////////////////////////////////////////////////////////////////////////
//Sort data for data type 'int'
void		SortData( int  Value[], int Num, int bIncrease );
//Sort data for data type 'double'
void		SortData( double  Value[], int Num, int bIncrease );
//Sort data for data type 'float'
void		SortData( float  Value[], int Num, int bIncrease );
//Get the median vaule of an array for data type 'double'
double		GetMidValue ( double *Value, int nTotal ) ;
//Get the median vaule of an array for data type 'int'
int			GetMidValue ( int *Value, int nTotal ) ;
//Get the median vaule of an array for data type 'double' with a quick algorithm
//Warning: it will change the array.
double		GetMidValue_Quick(double *arr, int n);

////////////////////////////////////////////////////////////////////////////////
//	Position relationship of points and lines
////////////////////////////////////////////////////////////////////////////////
//Shift a point
DSCC_POINT		ShiftPoint ( DSCC_POINT p, int dx, int dy );
//Transform a point using transform parameters
DSCC_POINT		TransformPoint(DSCC_POINT p, TRANSPARAM Param);
//Calculate the angle of a line formed by two points
double		GetAngle ( DSCC_POINT Start, DSCC_POINT End ) ;
//Get distance of two points
double		GetDistance ( const DSCC_POINT &pnt1, const DSCC_POINT &pnt2 ) ;
//Get distance of two points, interface for 'double' type
double		GetDistance ( double x1, double y1, double x2, double y2 ) ;
//Get distance of one point to a line formed by two points
double		GetDistance ( const DSCC_POINT &Pnt,  const DSCC_POINT &StPnd, const DSCC_POINT &EdPnt);
//Get cross point of two lines
int			GetCrossPoint ( double x1, double y1, double x2, double y2, double _x1, double _y1, double _x2, double _y2,  DSCC_POINT &CrossPnt);
//Get cross point of two lines
DSCC_POINT		GetCrossPoint(DSCC_POINT StPnt1, DSCC_POINT EdPnt1, DSCC_POINT StPnt2, DSCC_POINT EdPnt2);
//Get cross point one point to a line formed by two points
DSCC_POINT		GetCrossPoint(DSCC_POINT &pnt, DSCC_POINT &StPnt, DSCC_POINT &EdPnt);

///////////////////////////////////////////////////////////////////////////////////////
//	Matrix operation
////////////////////////////////////////////////////////////////////////////////////////
//Inverse a matrix
int		Inv(double a[], int n);
//Transpose a matrix
int		TransMatrix( double* A, int M, int N, double* B );
//Calculate feature mean
int		Mean( double **Feature, int Num, int Dimension, double *MeanFeature );
//Calculate feature mean
int		Mean( double *Feature, int Num, double &MeanFeature );
//Calculate variance matrix of feature
int		Var( double **Feature, int Num, int Dimension, double *Var, double *MeanFeature );
//Calculate variance matrix of feature
int		Var( double *Feature, int Num, double &Var, double &MeanFeature );
//Calculate determinate of a matrix
double	det(double a[], int n);
//Matrix multiplication
double	MatrixMultiply( double *a, double *b, double *c, int m, int n, int k);

///////////////////////////////////////////////////////////////////////////////
//	Other functions
////////////////////////////////////////////////////////////////////////////////
//Get four positions from a string
int			ParsePosition ( string s, int &left, int &top, int &right, int &bottom, int type );
//Get a point position from a string 
int			ParseCenter( string s, int &x, int &y);
//Inialize lut[] to calucate the number of black pixels in a byte
void		BlackPixelsInByte();
//Search peaks
int			SearchPeak( double *Hist, int nHist, int *&LeftPeak, int *&RightPeak, int &nPeak);
//Search valleys
int			SearchValley( double *Hist, int nHist, int *&LeftValley, int *&RightValley, int &nValley);
//Calculate peak width, do not consider background noise
int			GetPeakWidth2( double *Hist, int nHist, int nPeakPos );
//Calculate peak width
int			GetPeakWidth( double *Hist, int nHist, int nPeakPos );
//Calculate peak height
int			GetPeakHeight( double *Hist, int nHist, int nPeakPos );



#endif // __TOOLS_H__
