#ifndef _CONVERTMFCTOSTANDARDC_H__
#define _CONVERTMFCTOSTANDARDC_H__


// Standard constants
#undef FALSE
#undef TRUE
#undef NULL

#define FALSE   0
#define TRUE    1
#define NULL    0
#define MAX_PATH          260

typedef unsigned long DSCC_DWORD;
typedef DSCC_DWORD   DSCC_COLORREF;
typedef unsigned short      DSCC_WORD;
typedef unsigned char DSCC_BYTE;

#define DSCC_RGB(r,g,b)          ((DSCC_COLORREF)(((DSCC_BYTE)(r)|((DSCC_WORD)((DSCC_BYTE)(g))<<8))|(((DSCC_DWORD)(DSCC_BYTE)(b))<<16)))



typedef struct DSCC_tagPOINT {
	long  x;
	long  y;
} DSCC_POINT;

typedef struct DSCC_tagSIZE {
	long        cx;
	long        cy;
} DSCC_SIZE;

class DSCC_CPoint :
	public DSCC_tagPOINT {
public:
	// Constructors

	// create an uninitialized point
	DSCC_CPoint() throw();
	// create from two integers
	DSCC_CPoint(
		int initX,
		int initY) throw();
	// create from another point
	DSCC_CPoint(DSCC_POINT initPt) throw();
	// create from a size
	DSCC_CPoint(DSCC_SIZE initSize) throw();

	void SetPoint(
		int X,
		int Y) throw();
	int operator==(DSCC_POINT point) const throw();
	int operator!=(DSCC_POINT point) const throw();
	void operator+=(DSCC_SIZE size) throw();
	void operator-=(DSCC_SIZE size) throw();
	void operator+=(DSCC_POINT point) throw();
	void operator-=(DSCC_POINT point) throw();

	// Operators returning DSCC_CPoint values
	DSCC_CPoint operator+(DSCC_SIZE size) const throw();
	DSCC_CPoint operator-(DSCC_SIZE size) const throw();
	DSCC_CPoint operator-() const throw();
	DSCC_CPoint operator+(DSCC_POINT point) const throw();
};
	typedef struct DSCC_tagRECT {
		long    left;
		long    top;
		long    right;
		long    bottom;
	} DSCC_RECT;
	class DSCC_CRect :
		public DSCC_tagRECT {
		// Constructors
	public:
		// uninitialized rectangle
		DSCC_CRect() throw();
		// from left, top, right, and bottom
		DSCC_CRect(
			int l,
			int t,
			int r,
			int b) throw();

		// from a point and size
		DSCC_CRect(
			DSCC_POINT point,
			DSCC_SIZE size) throw();
		// from two points
		DSCC_CRect(
			DSCC_POINT topLeft,
			DSCC_POINT bottomRight) throw();

		// Attributes (in addition to RECT members)

		// retrieves the width
		int Width() const throw();
		// returns the height
		int Height() const throw();
		// returns the size
		// reference to the top-left point
		DSCC_CPoint& TopLeft() throw();
		// reference to the bottom-right point
		DSCC_CPoint& BottomRight() throw();
		// const reference to the top-left point
		const DSCC_CPoint& TopLeft() const throw();
		// const reference to the bottom-right point
		const DSCC_CPoint& BottomRight() const throw();



	};


	inline DSCC_CPoint::DSCC_CPoint() throw() {
		x = 0;
		y = 0;
	}

	inline DSCC_CPoint::DSCC_CPoint(
		int initX,
		int initY) throw() {
		x = initX;
		y = initY;
	}

	inline DSCC_CPoint::DSCC_CPoint(DSCC_POINT initPt) throw() {
		*(DSCC_POINT*)this = initPt;
	}

	inline DSCC_CPoint::DSCC_CPoint(DSCC_SIZE initSize) throw() {
		*(DSCC_SIZE*)this = initSize;
	}


	inline void DSCC_CPoint::SetPoint(
		int X,
		int Y) throw() {
		x = X;
		y = Y;
	}
	inline int DSCC_CPoint::operator==(DSCC_POINT point) const throw() {
		return (x == point.x && y == point.y);
	}

	inline int DSCC_CPoint::operator!=(DSCC_POINT point) const throw() {
		return (x != point.x || y != point.y);
	}

	inline void DSCC_CPoint::operator+=(DSCC_SIZE size) throw() {
		x += size.cx;
		y += size.cy;
	}

	inline void DSCC_CPoint::operator-=(DSCC_SIZE size) throw() {
		x -= size.cx;
		y -= size.cy;
	}

	inline void DSCC_CPoint::operator+=(DSCC_POINT point) throw() {
		x += point.x;
		y += point.y;
	}

	inline void DSCC_CPoint::operator-=(DSCC_POINT point) throw() {
		x -= point.x;
		y -= point.y;
	}

	inline DSCC_CPoint DSCC_CPoint::operator+(DSCC_SIZE size) const throw() {
		return DSCC_CPoint(x + size.cx, y + size.cy);
	}

	inline DSCC_CPoint DSCC_CPoint::operator-(DSCC_SIZE size) const throw() {
		return DSCC_CPoint(x - size.cx, y - size.cy);
	}

	inline DSCC_CPoint DSCC_CPoint::operator-() const throw() {
		return DSCC_CPoint(-x, -y);
	}

	inline DSCC_CPoint DSCC_CPoint::operator+(DSCC_POINT point) const throw() {
		return DSCC_CPoint(x + point.x, y + point.y);
	}

	//CRect
	inline DSCC_CRect::DSCC_CRect() throw() {
		left = 0;
		top = 0;
		right = 0;
		bottom = 0;
	}

	inline DSCC_CRect::DSCC_CRect(
		int l,
		int t,
		int r,
		int b) throw() {
		left = l;
		top = t;
		right = r;
		bottom = b;
	}


	inline DSCC_CRect::DSCC_CRect(
		DSCC_POINT point,
		DSCC_SIZE size) throw() {
		right = (left = point.x) + size.cx;
		bottom = (top = point.y) + size.cy;
	}

	inline DSCC_CRect::DSCC_CRect(
		DSCC_POINT topLeft,
		DSCC_POINT bottomRight) throw() {
		left = topLeft.x;
		top = topLeft.y;
		right = bottomRight.x;
		bottom = bottomRight.y;
	}

	inline int DSCC_CRect::Width() const throw() {
		return right - left;
	}

	inline int DSCC_CRect::Height() const throw() {
		return bottom - top;
	}


	inline DSCC_CPoint& DSCC_CRect::TopLeft() throw() {
		return *((DSCC_CPoint*)this);
	}

	inline DSCC_CPoint& DSCC_CRect::BottomRight() throw() {
		return *((DSCC_CPoint*)this + 1);
	}

	inline const DSCC_CPoint& DSCC_CRect::TopLeft() const throw() {
		return *((DSCC_CPoint*)this);
	}

	inline const DSCC_CPoint& DSCC_CRect::BottomRight() const throw() {
		return *((DSCC_CPoint*)this + 1);
	}


#endif//#define __CONVERTMFCTOSTANDARDC_H__