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
	
    char text[] = {"This is a very long sample string for wrapping around 320 pixels. This is yet another sample string made for text wrapping testing."};

	char *next_line;

    gfx_Begin();
    if (!fontlib_SetFont(test_font, 0))
		dbg_sprintf(dbgout, "Font failed to load");
    
    fontlib_SetLineSpacing(2, 2);
    textio_SetWindow_Outline(0, 0, 220, 240);
    fontlib_SetColors(0x00, 0xFF);
	fontlib_HomeUp();
	
	
	dbg_sprintf(dbgout, "Attempting to set tab to 5 | textio_SetTabWidth = %d | tab = %d\n", textio_SetTabWidth(5), textio_GetTabWidth());
	dbg_sprintf(dbgout, "Attempting to set tab to 0 | textio_SetTabWidth = %d | tab = %d\n", textio_SetTabWidth(0), textio_GetTabWidth());
	dbg_sprintf(dbgout, "Attempting to set tab to 4 | textio_SetTabWidth = %d | tab = %d\n", textio_SetTabWidth(4), textio_GetTabWidth());
	dbg_sprintf(dbgout, "Attempting to set format to 0 | textio_SetPrintFormat = %d | format = %d\n", textio_SetPrintFormat(0), textio_GetPrintFormat());
	dbg_sprintf(dbgout, "Attempting to set format to 3 | textio_SetPrintFormat = %d | format = %d\n", textio_SetPrintFormat(3), textio_GetPrintFormat());
	dbg_sprintf(dbgout, "textio_width = %d | fontlib_width = %d\n", textio_GetLineWidth(text, text + 10), fontlib_GetStringWidthL(text, 10));
	textio_SetPrintFormat(1);
	next_line = textio_GetLinePtr(text, 1);
	dbg_sprintf(dbgout, "next line = 0x%6x | next line = %c\n", next_line, *next_line);
	textio_PrintText(text, 5);
	while (!os_GetCSC());
	
    gfx_End();
}