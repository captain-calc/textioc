#include "fonts/fonts.h"

#include <graphx.h>
#include <fontlibc.h>
#include <textioc.h>
#include <debug.h>

void setup_fontlib_textio(void);

void main(void) {
	
	char text[] = {"This line starts with a tab, as many paragraphs usually do in the Western world. This paragraph, made up of several lines of testing text, spans about ten lines. Its objective is to reveal any lurking problems in the textio_PrintText() function which is resposible for the TextIOC's text wrapping."};

	/* Start the graphics */
	gfx_Begin();
	
	/* Setup the custom font. */
	fontlib_SetFont(test_font, 0);
	
	/* Setup the source library. */
	setup_fontlib_textio();
	textio_SetFontHeight(fontlib_GetCurrentFontHeight());
	textio_SetLineSpacing(0, 0);
	
	/* Set the text window dimensions*/
	textio_SetTextWindow(0, 0, 140, 240);
	
	/* Set print format to left-margin flush */
	textio_SetPrintFormat(TEXTIOC_PRINT_LEFT_MARGIN_FLUSH);
	
	/* Set the number of spaces that make up the tab */
	textio_SetTabSize(5);
	
	/* Print the text in the TextIOC text window. */
	textio_PrintText(text, 0);
	
	/* Print the same text in a FontLib text window. */
	fontlib_SetWindow(160, 0, 150, 240);
	fontlib_SetNewlineOptions(FONTLIB_ENABLE_AUTO_WRAP);
	fontlib_HomeUp();
	fontlib_DrawString(text);
	
	/* Outline the window so we can see it */
	gfx_SetColor(224);
	gfx_Rectangle_NoClip(0, 0, 140, 240);
	gfx_Rectangle_NoClip(160, 0, 150, 240);
	
	/* Wait for keypress */
	while (!os_GetCSC());
	
	/* Close the graphics */
	gfx_End();
	exit(0);
}

void setup_fontlib_textio(void) {

	/* Allocate the pointer structure. */
	textio_library_routines_t *ptr = malloc(sizeof(textio_library_routines_t));

	/* Tell TextIOC that it will be using FontLibC. */
	textio_SetSourceLibrary(TEXTIO_SET_FONTLIBC_AS_SRC_LIB);

	/* Set the struct pointers to the necessary FontLib functions. */
	ptr->set_cursor_position = &fontlib_SetCursorPosition;
	ptr->get_cursor_x = &fontlib_GetCursorX;
	ptr->get_cursor_y = &fontlib_GetCursorY;
	ptr->draw_char = &fontlib_DrawGlyph;
	ptr->get_char_width = &fontlib_GetGlyphWidth;

	/* Pass the struct pointers to TextIOC. */
	textio_SetLibraryRoutines(ptr);
	
	/* Free the structure memory. */
	free(ptr);
	
	return;
}