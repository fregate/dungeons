
#pragma once

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
	CRect()
	{
		left = 0;
		top = 0;
		right = 0;
		bottom = 0;
	}
	// from left, top, right, and bottom
	CRect(int l, int t, int r, int b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}

	// copy constructor
	CRect(const CRect& srcRect)
	{
		left = srcRect.left;
		top = srcRect.top;
		right = srcRect.right;
		bottom = srcRect.bottom;
	}

	// from a pointer to another rect
	CRect(CRect* lpSrcRect)
	{
		left = lpSrcRect->left;
		top = lpSrcRect->top;
		right = lpSrcRect->right;
		bottom = lpSrcRect->bottom;
	}

	// Attributes (in addition to RECT members)

	// retrieves the width
	long Width() const
	{
		return right - left;
	}
	// returns the height
	long Height() const
	{
		return bottom - top;
	}

	// returns TRUE if rectangle has no area
	bool IsRectEmpty() const
	{
		return top == 0 && left == top && right == top && bottom == top;
	}

	// Operations

	// set rectangle from left, top, right, and bottom
	void SetRect(int x1, int y1, int x2, int y2)
	{
		left = x1;
		top = y1;
		right = x2;
		bottom = y2;
	}

	// empty the rectangle
	void SetRectEmpty()
	{
		left = right = top = bottom = 0;
	}

	// Inflate rectangle's width and height by
	// x units to the left and right ends of the rectangle
	// and y units to the top and bottom.
	void InflateRect(long x, long y)
	{
		left -= x;
		top -= y;
		right += x;
		bottom += y;
	}

	bool Intersect(const CRect& r2, CRect& intersection) const
	{
		if (InRect({ r2.left, r2.top }))
		{
			intersection.SetRect(r2.left, r2.top, right, bottom);
			return true;
		}

		if (InRect({ r2.right, r2.top }))
		{
			intersection.SetRect(left, r2.top, r2.right, bottom);
			return true;
		}

		if (InRect({ r2.right, r2.bottom }))
		{
			intersection.SetRect(left, top, r2.right, r2.bottom);
			return true;
		}

		if (InRect({ r2.left, r2.bottom }))
		{
			intersection.SetRect(r2.left, top, right, r2.bottom);
			return true;
		}

		intersection.SetRectEmpty();
		return false;
	}

	bool InRect(const Vec& v) const
	{
		return (left < v.x && v.x < right) && (top < v.y && v.y < bottom);
	}
};
