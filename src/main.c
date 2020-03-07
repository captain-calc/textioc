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

void dialog( uint24_t xPos, uint8_t yPos, uint24_t width, uint8_t height, uint8_t banner_color, const char *title);

void main(void) {
    
	char *user_input;
	const char *pass = "PASSWORD";
	const char *message = "Welcome";
	
	user_input = malloc(9);
	memset(user_input, '\0', 9);
	
	gfx_Begin();
	
	// Draw dialogs
	dialog(50, 80, 220, 80, 30, "Access Requested" );
	
	gfx_PrintStringXY("Please enter your password:", 55, 115);
	gfx_SetColor(0);
	gfx_Rectangle(55, 130, 150, 13);
	
	// Get input
	textio_LetterInput(user_input, 20, 58, 132);
	
	if (strcmp(user_input, pass) == 0) {
		gfx_FillScreen(30);
		gfx_SetTextScale(3, 3);
		gfx_SetTextBGColor(30);
		gfx_SetTextFGColor(255);
		gfx_SetTextTransparentColor(30);
		gfx_PrintStringXY(message, 160 - gfx_GetStringWidth(message) / 2, 80);
	};
	
	while (!os_GetCSC());
	
	free(user_input);
	
	gfx_End();
	
}

void dialog( uint24_t xPos, uint8_t yPos, uint24_t width, uint8_t height, uint8_t banner_color, const char *title) {
	
	gfx_SetColor(181);
	gfx_FillRectangle(xPos + 1, yPos + 1, width, height);
	gfx_SetColor(0);
	gfx_FillRectangle(xPos, yPos, width, height);
	gfx_SetColor(banner_color);
	gfx_FillRectangle(xPos + 1, yPos + 1, width - 2, 15);
	gfx_SetColor(255);
	gfx_FillRectangle(xPos + 1, yPos + 17, width - 2, height - 18);
	gfx_PrintStringXY(title, xPos + 4, yPos + 4);
	
}