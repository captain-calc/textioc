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
	
    char text[] = {"Welcome to Cemetech! Since 2000, Cemetech (pronounced KE'me'tek) has been teaching programming and electronics and developing software and hardware. Among Cemetech's specialties are TI, HP, and Casio graphing calculators like the TI-84 Plus, TI-Nspire, HP Prime, and Casio Prizm, embedded and DIY electronics, and computer and web programming. Cemetech provides a safe, friendly space for people to learn, show off projects, and share knowledge and expertise. Our staff of friendly volunteers hang out on our forum and IRC and SAX chatrooms, and are happy to help."};
	char *next_line;

    gfx_Begin();
    if (!fontlib_SetFont(test_font, 0))
		dbg_sprintf(dbgout, "Font failed to load");
    
    fontlib_SetLineSpacing(2, 2);
    textio_SetWindow_Outline(0, 0, 320, 240);
    fontlib_SetColors(0x00, 0xFF);
	fontlib_HomeUp();
	
	dbg_sprintf(dbgout, "width of first line: %d\n", textio_GetLineWidth(text, text + 43));
	dbg_sprintf(dbgout, "width of first line: %d\n", textio_GetLineWidth(text + 43, text + 85));
	if (!textio_SetPrintFormat(3)) {
		dbg_sprintf(dbgout, "Failed to set print format. Terminated program.");
		goto PRGM_EXIT;
	};
	textio_SetTabWidth(10);
	textio_PrintText(text, 5);
	//next_line = test_function(text);
	//dbg_sprintf(dbgout, "test_function = \'%c\' | 0x%6x\n", *next_line, next_line);
	dbg_sprintf(dbgout, "line width = %d\n", textio_GetLineWidth(text + 4, text + 9));
	next_line = textio_GetLinePtr(text, 1);
	dbg_sprintf(dbgout, "next line starts with \'%c\' | 0x%6x\n", *next_line, next_line);
	next_line = textio_GetLinePtr(text + 43, 1);
	dbg_sprintf(dbgout, "next line starts with \'%c\' | 0x%6x\n", *next_line, next_line);
	while (!os_GetCSC());
	
	PRGM_EXIT:
    gfx_End();
	exit(0);
}