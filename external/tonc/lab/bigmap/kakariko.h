
//{{BLOCK(kakariko)

//======================================================================
//
//	kakariko, 1024x1024@4, 
//	+ palette 256 entries, not compressed
//	+ 319 tiles (t|f|p reduced) lz77 compressed
//	+ regular map (flat), not compressed, 128x128 
//	Total size: 512 + 5936 + 32768 = 39216
//
//	Time-stamp: 2007-02-16, 23:41:18
//	Exported by Cearn's Usenti v1.7.7
//	(comments, kudos, flames to "daytshen@hotmail.com")
//
//======================================================================

#ifndef __KAKARIKO__
#define __KAKARIKO__

#define kakarikoPalLen 512
extern const unsigned short kakarikoPal[256];

#define kakarikoTilesLen 5936
extern const unsigned short kakarikoTiles[2968];

#define kakarikoMapLen 32768
extern const unsigned short kakarikoMap[16384];

#endif // __KAKARIKO__

//}}BLOCK(kakariko)
