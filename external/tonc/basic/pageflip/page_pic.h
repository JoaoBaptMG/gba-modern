//======================================================================
//
//	front, 144x16@8, 
//	+ palette 16 entries, not compressed
//	+ bitmap not compressed
//	Total size: 32 + 2304 = 2336
//
//	Time-stamp: 2005-12-24, 15:30:24
//	Exported by Cearn's Usenti v1.7.1
//	(comments, kudos, flames to "daytshen@hotmail.com")
//
//======================================================================

#ifndef __FRONT__
#define __FRONT__

#define frontPalLen 32
extern const unsigned int frontPal[8];

#define frontBitmapLen 2304
extern const unsigned int frontBitmap[576];

#endif // __FRONT__

//======================================================================
//
//	back, 144x16@8, 
//	+ bitmap not compressed
//	Total size: 2304 = 2304
//
//	Time-stamp: 2005-12-24, 15:30:41
//	Exported by Cearn's Usenti v1.7.1
//	(comments, kudos, flames to "daytshen@hotmail.com")
//
//======================================================================

#ifndef __BACK__
#define __BACK__

#define backBitmapLen 2304
extern const unsigned int backBitmap[576];

#endif // __BACK__

