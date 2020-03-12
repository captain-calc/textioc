/*
 *--------------------------------------
 * Program Name: textioc truncated text example
 * Version: 1
 * Author: beckadamtheinventor, Captain Calc
 * License:
 * Description: A library for advanced text
 * I/O for the TI-84 Plus CE
 *--------------------------------------
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <keypadc.h>
#include <graphx.h>
#include <debug.h>

#include <textioc.h>

void main(void) {
	
	gfx_Begin();
	
	textio_PrintTruncatedStringXY("Squire Trelawney, Doctor Livesey, and the rest of these gentlemen having asked me to write down the whole particulars about Treasure Island, from the beginning to the end, keeping nothing back but the bearings of the island, and that only because there is still treasure not yet lifted, I take up my pen in the year of grace 17-, and go back to the time when my father kept the \"Admiral Benbow\" Inn, and the brown old seaman, with the saber cut, first took up his lodging under our roof.", 300, 1, 1);
	
	while (!os_GetCSC());	
	gfx_End();
}
