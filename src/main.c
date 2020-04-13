/*
 *--------------------------------------
 * Program Name: textioc
 * Version: 0.3.00
 * Author: Captain Calc
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

#include "textioc.h"

void main(void) {
	
	char *text = "Squire Trelawney, Dr. Livesey, and the rest of these gentlemen\n having asked me to write down the whole particulars about Treasure Island, from the beginning to the end, keeping nothing back but the bearings of the island, and that only because there is still treasure not yet lifted, I take up my pen in the year of grace 17__ and go back to the time when my father kept the Admiral Benbow inn and the brown old seaman with the sabre cut first took up his lodging under our roof.";
	
	gfx_Begin();
	
	// Debugging code
	dbg_sprintf(dbgout, "\n--[Program Start]--------------------------\n\n");
	
	textio_Setup();
	// void textio_PrintTextXY(char *text, uint8_t initial_line_num, uint8_t num_lines, uint24_t xPos, uint8_t yPos, uint24_t line_width)
	textio_PrintTextXY(text, 1, 20, 5, 5, 310);

	
	while (!os_GetCSC());
	
	// Close program
	gfx_End();
}