
//{{BLOCK(brin)

//======================================================================
//
//	brin, 512x256@4, 
//	+ palette 256 entries, not compressed
//	+ 31 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x32 
//	Total size: 512 + 992 + 4096 = 5600
//
//	Time-stamp: 2007-02-16, 14:23:41
//	Exported by Cearn's Usenti v1.7.7
//	(comments, kudos, flames to "daytshen@hotmail.com")
//
//======================================================================

#ifndef __BRIN__
#define __BRIN__

#define brinPalLen 512
extern const unsigned short brinPal[256];

#define brinTilesLen 992
extern const unsigned short brinTiles[496];

#define brinMapLen 4096
extern const unsigned short brinMap[2048];

#endif // __BRIN__

//}}BLOCK(brin)
