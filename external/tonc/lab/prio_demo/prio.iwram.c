

#include <tonc.h>

//! Sort indices via shell sort
/*! \param keys	Array sort keys
*	\param ids	Array of indices. After completion keys[ids[ii]]
*	  will be sorted in ascending order.
*	\param count	Number of entries.
*/
IWRAM_CODE void id_sort_shell(int keys[], u8 ids[], int count)
{
	u32 ii, inc;

	// find initial 'inc' in sequence x[i+1] = 3*x[i]+1 ; x[1]=1
	for(inc=1; inc<=count; inc++)
		inc *= 3;
	// actual sort
	do
	{
		inc /= 3;
		for(ii=inc; ii<count; ii++)
		{
			u32 jj, id0= ids[ii];
			int key0= keys[id0];
			for(jj=ii; jj>=inc && keys[ids[jj-inc]]>key0; jj -= inc)
				ids[jj]= ids[jj-inc];
			ids[jj]= id0;
		}
	} while(inc > 1);
}
