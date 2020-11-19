//======================================================================
//
//	gba_pic, 240x160@8, 
//	+ palette 16 entries, not compressed
//	+ bitmap not compressed
//	Total size: 32 + 38400 = 38432
//
//	Time-stamp: 2005-12-24, 14:31:24
//	Exported by Cearn's Usenti v1.7.1
//	(comments, kudos, flames to "daytshen@hotmail.com")
//
//======================================================================

#ifndef __GBA_PIC__
#define __GBA_PIC__

#define gba_picPalLen 32
extern const unsigned int gba_picPal[8];

#define gba_picBitmapLen 38400
extern const unsigned int gba_picBitmap[9600];

#endif // __GBA_PIC__

