//======================================================================
//
//	cage, 256x512@8, 
//	+ palette 128 entries, not compressed
//	+ 218 tiles (t|f reduced) not compressed
//	+ regular map (flat), not compressed, 32x64 
//	Total size: 256 + 13952 + 4096 = 18304
//
//	Time-stamp: 2006-04-17, 00:27:04
//	Exported by Cearn's Usenti v1.7.4
//	(comments, kudos, flames to "daytshen@hotmail.com")
//
//======================================================================

#ifndef __CAGE__
#define __CAGE__

#define cagePalLen 256
extern const unsigned short cagePal[128];

#define cageTilesLen 13952
extern const unsigned short cageTiles[6976];

#define cageMapLen 4096
extern const unsigned short cageMap[2048];

#endif // __CAGE__

