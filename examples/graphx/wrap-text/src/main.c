#include <graphx.h>
#include <textioc.h>
#include <debug.h>

void setup_gfx_textio(void);

void main(void) {
	
	char text[] = {"\tThis line starts with a tab, as many paragraphs usually do in the Western world. This paragraph, made up of several lines of testing text, spans about ten lines. Its objective is to reveal any lurking problems in the textio_PrintText() function which is resposible for the TextIOC's text wrapping."};
	char test[] = {" "};

	/* Start the graphics */
	gfx_Begin();
	
	/* Setup source library. */
	setup_gfx_textio();
	
	/* Preliminary configuration */
	textio_SetFontHeight(8);
	textio_SetLineSpacing(1, 1);
	
	/* Set the text window dimensions*/
	textio_SetTextWindow(0, 0, 140, 240);
	
	/* Set print format to left-margin flush */
	textio_SetPrintFormat(TEXTIOC_PRINT_LEFT_MARGIN_FLUSH);
	
	/* Set the number of spaces that make up the tab */
	textio_SetTabSize(5);
	
	/* Print the text */
	textio_PrintText(text, 5);
	gfx_PrintStringXY("This line is printed", 160, 5);
	gfx_PrintStringXY("without using the", 160, 15);
	gfx_PrintStringXY("TextIOC library. It is", 160, 25);
	gfx_PrintStringXY("a demonstration of", 160, 35);
	gfx_PrintStringXY(" being able to use the", 160, 45);
	gfx_PrintStringXY("source library's", 160, 55);
	gfx_PrintStringXY("functions", 160, 65);
	gfx_PrintStringXY("independantly of", 160, 75);
	gfx_PrintStringXY("TextIOC while a", 160, 85);
	gfx_PrintStringXY("TextIOC text window", 160, 95);
	gfx_PrintStringXY("is active.", 160, 105);
	
	/* Outline each "window". */
	gfx_SetColor(224);
	gfx_Rectangle_NoClip(0, 0, 140, 240);
	gfx_Rectangle_NoClip(160, 0, 150, 240);
	
	/* Wait for keypress */
	while (!os_GetCSC());
	
	/* Close the graphics */
	gfx_End();
	exit(0);
}

void setup_gfx_textio(void) {

	/* Allocate the pointer structure. */
	textio_library_routines_t *ptr = malloc(sizeof(textio_library_routines_t));

	/* Tell TextIOC that it will be using GraphX. */
	textio_SetSourceLibrary(TEXTIO_SET_GRAPHX_AS_SRC_LIB);

	/* Set the struct's pointers to the requisite GraphX functions. */
	ptr->set_cursor_position = &gfx_SetTextXY;
	ptr->get_cursor_x = &gfx_GetTextX;
	ptr->get_cursor_y = &gfx_GetTextY;
	ptr->draw_char = &gfx_PrintChar;
	ptr->get_char_width = &gfx_GetCharWidth;

	/* Pass the struct pointers to TextIOC. */
	textio_SetLibraryRoutines(ptr);
	
	/* Free the structure memory. */
	free(ptr);
	
	return;
}