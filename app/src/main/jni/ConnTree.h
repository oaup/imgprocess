/////////////////////////////////////////////////////////////////
// File Name:		ConnTree.h
// File Function:	Interface for CConnTree class
//
//				Developed by: Yefeng Zheng
//				First Created: June 2001
//			University of Maryland, College Park
//					All Right Reserved
//////////////////////////////////////////////////////////////////////

#ifndef __CONNTREE_H__
#define __CONNTREE_H__

#include "convertMFC.h"
#include "FormStructDef.h"


class CConnTree //: public CObject
{
//DECLARE_SERIAL(CConnTree)
public:
	int		m_IsHorConn;//Indicate horizontal or vertical runlengths
	DSCC_RECT	m_rcRange;//The region to analysed

//Variable used to store runlengths
	int		m_nDepth;
    int		*m_pColHead;//Column head pointer.
    int		m_pEmptHead;//Empty column head
    int		m_nMaxNodes;//Maximum nodes has been allocated. Variable used to allocate memory dynamically.
	int		m_nLeftMostX;
	int		m_nLeft;
	ConnNode *m_pNode;//Runlengths extracted

//Variable used to extract connected components
	DSCC_BYTE	*m_pFlag;//Indicate the runlength used or not
	int		*m_pStack;//Stack used to store runlength ready for processing
	ConnComp *m_pConnComp;//Connected components
    int		m_nTotalConnComps;//Toal connected components extracted

public :
    CConnTree() ;
    ~CConnTree() ;

    int	Initialize(DSCC_RECT rcRange) ;
    int		FreeMem();//Free memory used by CConnTree

//Methods for testing nodes connectionship
    int	IsConnected ( int pNode1, int pNode2) ;//Test if two nodes connected
	int		IsConnected2( int pNode1, int pNode2, int &width, int &gap ) ;//Test if two nodes neighboring within the specific region
    int	IsRightConnected ( int pNode1, int pNode2 ) ;//If right connected
	int		IsRightConnected2 ( int pNode1, int pNode2, int&width, int&gap ) ;//If right connected
    int	IsLeftConnected ( int pNode1, int pNode2 ) ;//If left connected
	int		IsLeftConnected2 ( int pNode1, int pNode2, int&width, int&gap );//If left connected

//Methods for Creating and storing runlengths
    int	AllocNewNodes() ;//Allocate new nodes
    int		CopyValley ( LineValley *v, int valleys ) ;//Create a new runlengths
    int		AddNewCol ( LineValley *v, int valleys, int nColNum ) ;//Add a new runlength
    int		MakeRightConn ( int nColNum ) ;//Store the new runlegth and adjust its right connectionship with other runlengths
    int		MakeLeftConn ( int nColNum ) ;//Store the new runlegth and adjust its left connectionship with other runlengths
    int		GetLeftMostX() ;//Return the most left ordinate m_nLeftMostX
	
//Connected Components extraction methods
    int		GetAllConnComps() ;//Get all connected components
    int		GetConnComp ( ConnComp &cc, int nInitial) ;//Get one connected component.
	int		GetConnComp ( ConnComp &cc, int nInitial, DSCC_BYTE *pFlag);//Get one connected componet.
    int		*GetConnComp ( ConnComp &cc, int *_nTotalNodes ) ;//Get one connected component.
} ;

#endif // __CONNTREE_H__
