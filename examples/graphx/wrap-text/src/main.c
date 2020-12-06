#include <graphx.h>
#include <textioc.h>


// Debugging
#include <stdio.h>
#define dbgout ((char*)0xFB0000)
#define dbgerr ((char*)0xFC0000)
#define dbg_sprintf sprintf


void setup_gfx_textio(void) {

	/* Setup the GraphX wrapper. */
	textio_library_routines_t routines = TEXTIO_GRAPHX_ROUTINES;

	/* Pass the wrapper pointers to TextIOC. */
	textio_SetLibraryRoutines(&routines);
	return;
}

void print_text(char *text, uint24_t xPos, uint8_t yPos, uint24_t max_line_width) {
	
	char *curr_line, *next_line, *curr_char;
	uint8_t line_spacing = 9;
	
	curr_line = text;

	for (;;) {
		
		// It is important to remember that textio_GetLineWidth() retrieves the width of all characters between line and eol, INCLUSIVE.
		next_line = textio_GetLinePtr(curr_line, 1, max_line_width);
<<<<<<< HEAD
		
		// Debugging
		dbg_sprintf(dbgout, "curr_line = 0x%6x | next_line = 0x%6x\n", curr_line, next_line);
=======
		if (curr_line == next_line)
			return;
>>>>>>> 25b1288c97986ae4674ec439bf03a8fdab53563e
		
		if (textio_GetPrintFormat() == TEXTIOC_FORMAT_RIGHT_MARGIN_FLUSH) {
			gfx_SetTextXY(max_line_width - xPos - textio_GetLineWidth(curr_line, next_line - 1), yPos);
		} else if (textio_GetPrintFormat() == TEXTIOC_FORMAT_CENTERED) {
			gfx_SetTextXY((max_line_width - xPos - textio_GetLineWidth(curr_line, next_line - 1)) / 2, yPos);
		} else {
			gfx_SetTextXY(xPos, yPos);
		};
		
		curr_char = curr_line;
		while (curr_char < next_line) {
			if (*curr_char == '\t') {
				gfx_PrintString("    ");
			} else if (*curr_char != '\n') {
				gfx_PrintChar(*curr_char);
			};
			curr_char++;
		};
		curr_line = next_line;
<<<<<<< HEAD
		if (*curr_line == '\0')
			return;
=======
>>>>>>> 25b1288c97986ae4674ec439bf03a8fdab53563e
		yPos += line_spacing;
		if (yPos > 240 - line_spacing)
			return;
	};
}

void main(void) {
	
	char text[] = {"\tThis line starts with a tab. The dimensions of the highlighted window are 140 pixels wide by 240 pixels tall. The initial text position is (0, 0). When the text reaches the bottom of the window, any text that will not fit will be truncated."};

	/* Start the graphics */
	gfx_Begin();
	
	/* Setup source library. */
	setup_gfx_textio();
	
	/* Set print format to left-margin flush. */
	textio_SetPrintFormat(TEXTIOC_FORMAT_LEFT_MARGIN_FLUSH);
	
	/* Set the number of pixels that make up the tab. */
	textio_SetTabWidth(gfx_GetCharWidth(' ') * 4);
<<<<<<< HEAD
	
	// Debugging
	dbg_sprintf(dbgout, "The eagle has landed.");
=======
>>>>>>> 25b1288c97986ae4674ec439bf03a8fdab53563e
	
	/* Print the text */
	print_text(text, 0, 5, 140);
	
	/* Outline the window. */
	gfx_SetColor(224);
	gfx_Rectangle_NoClip(0, 0, 140, 240);
	
	/* Wait for keypress */
	while (!os_GetCSC());
	
	/* Close the graphics */
	gfx_End();
	exit(0);
}