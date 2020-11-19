//======================================================================
//
//	metr, 64x64@4, 
//	+ palette 32 entries, not compressed
//	+ 64 tiles not compressed
//	Total size: 64 + 2048 = 2112
//
//	Time-stamp: 2005-12-24, 17:36:57
//	Exported by Cearn's Usenti v1.7.1
//	(comments, kudos, flames to "daytshen@hotmail.com")
//
//======================================================================

#ifndef __METR__
#define __METR__

#define metrPalLen 64
extern const unsigned int metrPal[16];

#define metrTilesLen 2048
extern const unsigned int metrTiles[512];

#endif // __METR__

//======================================================================
//
//	metr_box, 64x64@4, 
//	+ 64 tiles not compressed
//	Total size: 2048 = 2048
//
//	Time-stamp: 2005-12-24, 17:37:08
//	Exported by Cearn's Usenti v1.7.1
//	(comments, kudos, flames to "daytshen@hotmail.com")
//
//======================================================================

#ifndef __METR_BOX__
#define __METR_BOX__

#define metr_boxTilesLen 2048
extern const unsigned int metr_boxTiles[512];

#endif // __METR_BOX__

