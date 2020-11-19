//======================================================================
//
//	octants, 256x256@8, 
//	+ palette 16 entries, not compressed
//	+ 172 tiles (t reduced) lz77 compressed
//	+ affine map, lz77 compressed, 32x32 
//	Total size: 32 + 1940 + 476 = 2448
//
//	Time-stamp: 2006-09-26, 19:52:57
//	Exported by Cearn's Usenti v1.7.6
//	(comments, kudos, flames to "daytshen@hotmail.com")
//
//======================================================================

#ifndef __OCTANTS__
#define __OCTANTS__

#define octantsPalLen 32
extern const unsigned int octantsPal[8];

#define octantsTilesLen 1940
extern const unsigned int octantsTiles[485];

#define octantsMapLen 476
extern const unsigned int octantsMap[119];

#endif // __OCTANTS__

