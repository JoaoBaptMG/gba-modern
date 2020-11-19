// snd1_demo.c: 
// Beep! ^_^ 
//
// (20060125 - 20060125, Cearn)
//
// === NOTES ===
// * TODO: strings for notes / octaves

#include <stdio.h>
#include <tonc.h>

// ===DATA ============================================================

u8 txt_scrolly= 8;

const char *names[]=
{	"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "	};

// === FUNCTIONS ======================================================

// Show the octave the next note will be in
void note_prep(int octave)
{
	tte_printf("#{P:8,%d;cx:0x1000}[  %+2d]", txt_scrolly, octave);
}


// Play a note and show which one was played
void note_play(int note, int octave)
{
	// Clear next top and current rows
	SBB_CLEAR_ROW(31, (txt_scrolly/8-2)&31);
	SBB_CLEAR_ROW(31, txt_scrolly/8);	

	// Display note and scroll
	tte_printf("#{P:16,%d;cx:0}%-2s%+2d", txt_scrolly, names[note], octave);
	//se_puts(16, txt_scrolly, str, 0);

	txt_scrolly -= 8;
	REG_BG0VOFS= txt_scrolly-8;

	// Play the actual note
	REG_SND1FREQ = SFREQ_RESET | SND_RATE(note, octave);
}


// Play a little ditty
void sos()
{
	const u8 lens[6]= { 1,1,4, 1,1,4 };
	const u8 notes[6]= { 0x02, 0x05, 0x12,  0x02, 0x05, 0x12 };
	int ii;
	for(ii=0; ii<6; ii++)
	{
		note_play(notes[ii]&15, notes[ii]>>4);
		VBlankIntrDelay(8*lens[ii]);
	}
}

int main()
{
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;

	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	tte_init_se(0, BG_CBB(0) | BG_SBB(31), 0, CLR_ORANGE, 0, NULL, NULL);
	tte_init_con();
	pal_bg_mem[0x11]= CLR_GREEN;
	
	int octave= 0;

	// turn sound on
	REG_SNDSTAT= SSTAT_ENABLE;
	// snd1 on left/right ; both full volume
	REG_SNDDMGCNT = SDMG_BUILD_LR(SDMG_SQR1, 7);
	// DMG ratio to 100%
	REG_SNDDSCNT= SDS_DMG100;

	// no sweep
	REG_SND1SWEEP= SSW_OFF;
	// envelope: vol=12, decay, max step time (7) ; 50% duty
	REG_SND1CNT= SSQR_ENV_BUILD(12, 0, 7) | SSQR_DUTY1_2;
	REG_SND1FREQ= 0;

	sos();

	while(1)
	{
		VBlankIntrWait();
		key_poll();

		// Change octave:
		octave += bit_tribool(key_hit(-1), KI_R, KI_L);
		octave= wrap(octave, -2, 6);
		note_prep(octave);

		// Play note
		if(key_hit(KEY_DIR|KEY_A))
		{
			if(key_hit(KEY_UP))
				note_play(NOTE_D, octave+1);
			if(key_hit(KEY_LEFT))
				note_play(NOTE_B, octave);
			if(key_hit(KEY_RIGHT))
				note_play(NOTE_A, octave);
			if(key_hit(KEY_DOWN))
				note_play(NOTE_F, octave);
			if(key_hit(KEY_A))
				note_play(NOTE_D, octave);
		}

		// Play ditty
		if(key_hit(KEY_B))
			sos();		
	}
	return 0;
}
