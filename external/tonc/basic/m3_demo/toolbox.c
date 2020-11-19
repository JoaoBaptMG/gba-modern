//
// toolbox.c
// 
// Tools source for m3_dmeo
// 
// (20060922-20060922, cearn)
//
// === NOTES ===
// * This is a _small_ set of typedefs, #defines and inlines that can 
//   be found in tonclib, and might not represent the 
//   final forms.

#include "toolbox.h"


//! Fill the mode 3 background with color \a clr.
void m3_fill(COLOR clr)	
{	
	int ii;
	u32 *dst= (u32*)vid_mem;
	u32 word= (clr<<16) | clr;

	for(ii=0; ii<M3_SIZE/4; ii++)
		*dst++= word;
}

//! Draw a line on an 16bit buffer
/*!	\param x1		First X-coord.
	\param y1		First Y-coord.
	\param x2		Second X-coord.
	\param y2		Second Y-coord.
	\param clr		Color.
	\param dstBase	Canvas pointer (halfword-aligned plz).
	\param dstPitch		Canvas pitch in bytes.
*/
void bmp16_line(int x1, int y1, int x2, int y2, u32 clr, 
	void *dstBase, u32 dstPitch)
{
	int ii, dx, dy, xstep, ystep, dd;
	u16 *dst= (u16*)(dstBase + y1*dstPitch + x1*2);
	dstPitch /= 2;

	// --- Normalization ---
	if(x1>x2)
	{	xstep= -1;	dx= x1-x2;	}
	else
	{	xstep= +1;	dx= x2-x1;	}

	if(y1>y2)
	{	ystep= -dstPitch;	dy= y1-y2;	}
	else
	{	ystep= +dstPitch;	dy= y2-y1;	}


	// --- Drawing ---

	if(dy == 0)			// Horizontal
	{
		for(ii=0; ii<=dx; ii++)
			dst[ii*xstep]= clr;
	}
	else if(dx == 0)	// Vertical
	{
		for(ii=0; ii<=dy; ii++)
			dst[ii*ystep]= clr;
	}
	else if(dx>=dy)		// Diagonal, slope <= 1
	{
		dd= 2*dy - dx;

		for(ii=0; ii<=dx; ii++)
		{
			*dst= clr;
			if(dd >= 0)
			{	dd -= 2*dx;	dst += ystep;	}

			dd += 2*dy;
			dst += xstep;
		}				
	}
	else				// Diagonal, slope > 1
	{
		dd= 2*dx - dy;

		for(ii=0; ii<=dy; ii++)
		{
			*dst= clr;
			if(dd >= 0)
			{	dd -= 2*dy;	dst += xstep;	}

			dd += 2*dx;
			dst += ystep;
		}
	}
}


//! Draw a rectangle in 16bit mode; internal routine.
/*!	\param left		Left side of rectangle;
	\param top		Top side of rectangle.
	\param right	Right side of rectangle.
	\param bottom	Bottom side of rectangle.
	\param clr		Color.
	\param dstBase	Canvas pointer.
	\param dstPitch		Canvas pitch in bytes
*/
void bmp16_rect(int left, int top, int right, int bottom, u32 clr,
	void *dstBase, u32 dstPitch)
{
	int ix, iy;

	u32 width= right-left, height= bottom-top;
	u16 *dst= (u16*)(dstBase+top*dstPitch + left*2);
	dstPitch /= 2;

	// --- Draw ---
	for(iy=0; iy<height; iy++)
		for(ix=0; ix<width; ix++)
			dst[iy*dstPitch + ix]= clr;
}

//! Draw a rectangle in 16bit mode; internal routine.
/*!	\param left		Left side of rectangle;
	\param top		Top side of rectangle.
	\param right	Right side of rectangle.
	\param bottom	Bottom side of rectangle.
	\param clr		Color.
	\param dstBase	Canvas pointer.
	\param dstPitch		Canvas pitch in bytes
*/
void bmp16_frame(int left, int top, int right, int bottom, u32 clr,
	void *dstBase, u32 dstPitch)
{
	// Frame is RB exclusive
	right--;
	bottom--;

	bmp16_line(left,  top,   right,  top,    clr, dstBase, dstPitch);
	bmp16_line(left,  bottom, right, bottom, clr, dstBase, dstPitch);

	bmp16_line(left,  top,   left,   bottom, clr, dstBase, dstPitch);
	bmp16_line(right, top,   right,  bottom, clr, dstBase, dstPitch);
}


// EOF
