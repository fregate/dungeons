
#pragma once

#include <wtypes.h>

class CRect
{
public:
	long    left;
	long    top;
	long    right;
	long    bottom;

	// Constructors
public:
	// uninitialized rectangle
	CRect() throw()
	{
		left = 0;
		top = 0;
		right = 0;
		bottom = 0;
	}
	// from left, top, right, and bottom
	CRect(
		int l,
		int t,
		int r,
		int b) throw()
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}

	// copy constructor
	CRect(const CRect& srcRect) throw()
	{
		left = srcRect.left;
		top = srcRect.top;
		right = srcRect.right;
		bottom = srcRect.bottom;
	}

	// from a pointer to another rect
	CRect(CRect* lpSrcRect) throw()
	{
		left = lpSrcRect->left;
		top = lpSrcRect->top;
		right = lpSrcRect->right;
		bottom = lpSrcRect->bottom;
	}

	// Attributes (in addition to RECT members)

	// retrieves the width
	long Width() const throw()
	{
		return right - left;
	}
	// returns the height
	long Height() const throw()
	{
		return bottom - top;
	}

	// returns TRUE if rectangle has no area
	bool IsRectEmpty() const throw()
	{
		return top == 0 && left == top && right == top && bottom == top;
	}

	// Operations

	// set rectangle from left, top, right, and bottom
	void SetRect(int x1, int y1, int x2, int y2) throw()
	{
		left = x1;
		top = y1;
		right = x2;
		bottom = y2;
	}

	// empty the rectangle
	void SetRectEmpty() throw()
	{
		left = right = top = bottom = 0;
	}

	// TRUE if exactly the same as another rectangle
	bool EqualRect(const CRect&) const throw();

	// Inflate rectangle's width and height by
	// x units to the left and right ends of the rectangle
	// and y units to the top and bottom.
	void InflateRect(long x, long y) throw()
	{
		left -= x;
		top -= y;
		right += x;
		bottom += y;
	}
	// Inflate rectangle's width and height by
	// size.cx units to the left and right ends of the rectangle
	// and size.cy units to the top and bottom.
	void InflateRect(long l, long t, long r, long b) throw();

	// deflate the rectangle's width and height without
	// moving its top or left
	void DeflateRect(long x, long y) throw();
	void DeflateRect(long l, long t, long r, long b) throw();

	// translate the rectangle by moving its top and left
	void OffsetRect(long x, long y) throw();

	void NormalizeRect() throw();

	// absolute position of rectangle
	void MoveToY(long y) throw();
	void MoveToX(long x) throw();
	void MoveToXY(long x, long y) throw();

	bool Intersect(const CRect& r2, CRect& c) const
	{
		RECT rrr1, rrr2, rrr3;
		::SetRect(&rrr1, left, top, right, bottom);
		::SetRect(&rrr2, r2.left, r2.top, r2.right, r2.bottom);

		bool bres = ::IntersectRect(&rrr3, &rrr1, &rrr2) == 1;
		c.SetRect(rrr3.left, rrr3.top, rrr3.right, rrr3.bottom);

		return bres;
	}

	bool InRect(Vec v)
	{
		return (left < v.x && v.x < right) && (top < v.y && v.y < bottom);
	}

	// 	// set this rectangle to intersection of two others
	// 	BOOL IntersectRect(
	// 		_In_ LPCRECT lpRect1,
	// 		_In_ LPCRECT lpRect2) throw();
	// 
	// 	// set this rectangle to bounding union of two others
	// 	BOOL UnionRect(
	// 		_In_ LPCRECT lpRect1,
	// 		_In_ LPCRECT lpRect2) throw();
	// 
	// 	// set this rectangle to minimum of two others
	// 	BOOL SubtractRect(
	// 		_In_ LPCRECT lpRectSrc1,
	// 		_In_ LPCRECT lpRectSrc2) throw();

	// Additional Operations
	// 	void operator=(_In_ const RECT& srcRect) throw();
	// 	BOOL operator==(_In_ const RECT& rect) const throw();
	// 	BOOL operator!=(_In_ const RECT& rect) const throw();
	// 	void operator+=(_In_ POINT point) throw();
	// 	void operator+=(_In_ SIZE size) throw();
	// 	void operator+=(_In_ LPCRECT lpRect) throw();
	// 	void operator-=(_In_ POINT point) throw();
	// 	void operator-=(_In_ SIZE size) throw();
	// 	void operator-=(_In_ LPCRECT lpRect) throw();
	// 	void operator&=(_In_ const RECT& rect) throw();
	// 	void operator|=(_In_ const RECT& rect) throw();
	// 
	// 	// Operators returning CRect values
	// 	CRect operator+(_In_ POINT point) const throw();
	// 	CRect operator-(_In_ POINT point) const throw();
	// 	CRect operator+(_In_ LPCRECT lpRect) const throw();
	// 	CRect operator+(_In_ SIZE size) const throw();
	// 	CRect operator-(_In_ SIZE size) const throw();
	// 	CRect operator-(_In_ LPCRECT lpRect) const throw();
	// 	CRect operator&(_In_ const RECT& rect2) const throw();
	// 	CRect operator|(_In_ const RECT& rect2) const throw();
	// 	CRect MulDiv(
	// 		_In_ int nMultiplier,
	// 		_In_ int nDivisor) const throw();
};
