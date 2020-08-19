//////////////////////////////////////////////////////////////////////////
// File Name:		FormStructDef.h
// File Function:	1. Definitions for recognition cores
//					2. Definitions for DSCC
//					3. Definitions for form template
//					4. Definitions for form registeration transform
//
//				Developed by: Yefeng Zheng
//				First Created: June 2001
//			University of Maryland, College Park
//					All Right Reserved
///////////////////////////////////////////////////////////////////////////
#ifndef __STRUCTDEF_H__
#define __STRUCTDEF_H__
#include "convertMFC.h"
//#include "Imageobj.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAXSTRIP	100	//Maximum number of strips to split form image in order to speed up the algorithm

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CHAIN, CHAINS... for detecting frame lines
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct  ParameterStruct
{
	int        ValleyGrayDepth;//Valley gray depth, for gray form image recognition
	int        MinVerLineLength;//MinVerLineLength=0 MinHorLineLength=0  Estimate char width automatically
	int        MinHorLineLength;//Esle   use this two parameters
	int		   MaxLineWidth;//Maximum line width
	int	   FilterSmallDSCC;//TRUE -- Filter small DSCC to speed up the algorithm
							   //FALSE -- Do not filter to increase robustness to line broken
	int	   RLS;//TRUE -- Runlength smoothing
	int	   bHasSlantLine;//TRUE -- Has slant lines on the form
							 //FALSE -- No slant lines, delete all slant lines detected
	int		   MaxGap;
	int	   bSceneImg;	//Scene image or document image
}PARAMETER;

typedef struct LineValleyStruct
{
    int x ;//X coordinate
    int ys ;//Start position
    int ye ;//End position
	//For binary yvs=ys, yve=ye
    int yvs ;//Start position of valley
    int yve ;//End position of valley
    DSCC_BYTE EdgeGray ;//Gray level of valley edge
    DSCC_BYTE gray ;//gray level of bottom of valley?
} LineValley ;

typedef struct ConnComponentStruct
{
    int nInitialNode;//Initial runlength
	int nPixelsNum;//Number of black pixels
    DSCC_RECT rcBound ;//Bounding box of connected component
} ConnComp ;

typedef struct ConnNodeStruc
{
    int nLtTotal ;//Total left connected runlength
    int pLeft ;//Left runlength
    int nRtTotal ;//Total right connected runlength
    int pRight ;//Right runlength
    int pUnder ;//Under ConnNode
    int pAbove ;//Above ConnNode
    LineValley v ;//Node information
} ConnNode ;

typedef struct ChainStruct
{
    int pHead ;  // Head node (run-length), etc left most node
    int pTail ;  // Tail node (run-length), etc right most node
//Before merging pLeft and pRight = -1;  
//After merging if a chain form a chains indepently, pLeft=pRight=-2; 
    int pLeft ;  // Left chain
    int pRight ; // Right chain
    int Len ;	//Chain length
    int xs ;	//Start position
    int xe ;	//End position
	int fYs;//fYs = fYofChain(Chain, xs)   2000/3/23
	int fYe;//fYe = fYofChain(Chain, xe)   2000/3/23
    int Num ;	//Number of run length
    double r ;//Straightness
	double Width;//Average width
//Variable to MSE straight line approximation
	double SumX ;//Sum X
    double SumY ;//Sum Y
    double SumXX ;//Sum X*X
	double SumXY ;	//Sum X*Y

	DSCC_RECT   rcBound; //09/03/2002
//Variable to calculate the long and short axis of the chain
//They are different with SumX, SumY, SumXX, and SumXY, which are run length based.
//These new variables are pixel based
//Added on 09/03/2004
	int		NumPixel;	//Number of black pixels
	double	SX;
	double	SY;
	double	SYY;
	double	SXX;
} CHAIN ;

typedef struct ChainsStruct
{
    int pHead ;  // Head chain, etc left most chain
    int pTail ;  // Tail chain, etc right most chain
    int xs ;//Start position
    int xe ;//End position
	int fYs ;//fYs = fYofChains(Chains, xs);        2000/3/23
	int fYe ;//fYe = fYofChains(Chains, xe);       2000/3/23
    int Num ;//Number of run lengths
	int NumPixel; //Number of black pixels
	double r ;//Straightness
    double Q ;//Quality
	double	Angle;//Angle
	double Width;//Average width
	//Variable to MSE straight line approximation
	double SumX;//Sum X
    double SumY;//Sum Y
    double SumXX;//Sum X*X
    double SumXY;//Sum X*Y
} CHAINS ;

typedef struct FormLineStruct
{
    int nIndex ; //nIndex pointer to the Chains
    int nStyle ; // style of line, solid(0), dash(1), dot(2) or virtual(3)
    DSCC_CPoint StPnt ;//Start point
    DSCC_CPoint EdPnt ;//End point
    double Angle ;//Angle
    double Width ;//Average width
    double Q ;//Quality
	int   bSlant ;//Slant or not
//Added Zheng Y.F.   1999-9-2
	int	   nUseType;//0 -- unused   1 -- rectangle cell    2 -- other cell type
} FORMLINE ;

//Structure used to sort CHAIN
typedef struct IntChainStruct
{
    int n ;//CHAIN NO
    int pNext ;//Next CHAIN
} INTCHAIN ;

#endif // __STRUCTDEF_H__