//
// second.c
// Cleaner version of 'first.c'
//
// (20060211 - 20060922, cearn)

#include "toolbox.h"

int main()
{
	REG_DISPCNT= DCNT_MODE3 | DCNT_BG2;

	m3_plot( 120, 80, RGB15(31, 0, 0) );	// or CLR_RED
	m3_plot( 136, 80, RGB15( 0,31, 0) );	// or CLR_LIME
	m3_plot( 120, 96, RGB15( 0, 0,31) );	// or CLR_BLUE

	while(1);

	return 0;
}
