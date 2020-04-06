/*
 *--------------------------------------
 * Program Name: textioc
 * Version: 0.2.00
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
	
	// Make sure you supply one extra char for the null terminator
	char *check;
	char *gen_input;
	char *prgm_name;
	const char *password = "ABC";
	
	gen_input = malloc(21);
	memset(gen_input, '\0', 21);
	prgm_name = malloc(9);
	memset(prgm_name, '\0', 9);
	
	gfx_Begin();
	
	dbg_sprintf(dbgout, "\n--[Program Start]--------------------------\n\n");
	
	gfx_PrintStringXY("Letter Input Only:", 5, 5);
	gfx_Rectangle(5, 16, 120, 13);
	gfx_PrintStringXY("Numerical Input Only:", 5, 34);
	gfx_Rectangle(5, 45, 120, 13);
	gfx_PrintStringXY("Letter and Numerical Input:", 5, 63);
	gfx_Rectangle(5, 74, 120, 13);
	gfx_PrintStringXY("Program Name Input:", 5, 92);
	gfx_Rectangle(5, 103, 120, 13);
	
	textio_LetterInput(gen_input, 20, 117, 7, 18);
	memset(gen_input, '\0', 21);
	textio_NumericalInput(gen_input, 20, 117, 7, 47);
	memset(gen_input, '\0', 21);
	textio_LetterNumInput(gen_input, 20, 117, 7, 76);
	textio_PrgmNameInput(prgm_name, 117, 7, 105);
	
	while (!os_GetCSC());	
	gfx_End();
}