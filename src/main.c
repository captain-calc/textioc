/*
 *--------------------------------------
 * Program Name:
 * Author:
 * License:
 * Description:
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
    
	char *user_input;
	const char *pass = "PASSWORD";
	
	user_input = malloc(9);
	memset(user_input, '\0', 9);
	
	gfx_Begin();
	
	dbg_sprintf(dbgout, "\n------------------------------------------\nProgram starting...\n");
	
	gfx_Rectangle(48, 8, 100, 17);
	textio_LetterInput(user_input, 8, 50, 10);
	
	dbg_sprintf(dbgout, "--------------------------------------------\nuser_input = %s", user_input);
	
	if (strcmp(user_input, pass) == 0)
		gfx_PrintStringXY("Access Granted", 0, 0);
	
	while (!os_GetCSC());
	
	free(user_input);
	
	gfx_End();
	
}

