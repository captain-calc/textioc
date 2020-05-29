#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <graphx.h>
#include <debug.h>
#include "dbg/calcdbg.h"

#include <fileioc.h>
#include <fontlibc.h>
#include "fonts/fonts.h"

#include <textioc.h>


void textio_SetWindow_Outline(unsigned int x, uint8_t y, unsigned int width, uint8_t height) {
	
	gfx_SetColor(224);
	fontlib_SetWindow(x, y, width, height);
	gfx_Rectangle_NoClip(x, y, width + 1, height);
}


void main(void) {
	
    char text[] = {"\tThis is a very long sample string for wrapping around 320 pixels. This is yet another sample string made for text wrapping testing."};
	
	uint24_t width;

    gfx_Begin();
    fontlib_SetFont(test_font, 0);
    
    fontlib_SetLineSpacing(2, 2);
    textio_SetWindow_Outline(0, 0, 220, 240);
    fontlib_SetColors(0x00, 0xFF);
	
	//textio_SetTabWidth(4);
	
	//textio_SetPrintFormat(1);
    //textio_PrintText(text, 5);
	dbg_sprintf(dbgout, "tab = %d\n", textio_GetTabWidth());
	width = textio_GetLineWidth(text, text + 2);
	dbg_sprintf(dbgout, "line width = %d\n", width);
    while (!os_GetCSC());
	
    gfx_End();
}