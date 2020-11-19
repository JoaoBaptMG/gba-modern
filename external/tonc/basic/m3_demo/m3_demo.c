//
// m3_demo.c
// Basic mode 3 drawing routines
//
// (20060104 - 20060104, cearn)

#include "toolbox.h"

int main()
{
	int ii, jj;

	REG_DISPCNT= DCNT_MODE3 | DCNT_BG2;

	// Fill screen with gray color
	m3_fill(RGB15(12, 12, 14));

	// Rectangles:
	m3_rect( 12,  8, 108,  72, CLR_RED);
	m3_rect(108, 72, 132,  88, CLR_LIME);
	m3_rect(132, 88, 228, 152, CLR_BLUE);

	// Rectangle frames
	m3_frame(132,  8, 228,  72, CLR_CYAN);
	m3_frame(109, 73, 131,  87, CLR_BLACK);
	m3_frame( 12, 88, 108, 152, CLR_YELLOW);

	// Lines in top right frame
	for(ii=0; ii<=8; ii++)
	{
		jj= 3*ii+7;
		m3_line(132+11*ii, 9, 226, 12+7*ii, RGB15(jj, 0, jj));
		m3_line(226-11*ii,70, 133, 69-7*ii, RGB15(jj, 0, jj));
	}

	// Lines in bottom left frame
	for(ii=0; ii<=8; ii++)
	{
		jj= 3*ii+7;
		m3_line(15+11*ii, 88, 104-11*ii, 150, RGB15(0, jj, jj));
	}

	while(1);

	return 0;
}
