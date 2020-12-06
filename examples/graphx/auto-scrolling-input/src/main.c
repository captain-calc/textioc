#include <graphx.h>
#include <keypadc.h>
#include <tice.h>
#include <textioc.h>

#include <stdio.h>

#define INPUT_FIELD_X	100
#define INPUT_FIELD_Y	150

typedef struct {
	char *first_visible_char;
	uint24_t visible_buffer_width;
} buffer_data_t;

buffer_data_t buffer_data;

void setup_gfx_textio(void) {

	/* Setup the GraphX wrapper. */
	textio_library_routines_t routines = TEXTIO_GRAPHX_ROUTINES;

	/* Pass the wrapper pointers to TextIOC. */
	textio_SetLibraryRoutines(&routines);
	return;
}
/*
void draw_debug(char *buffer, char *curr_char_ptr, uint24_t buffer_length) {
	
	uint24_t i;
	
	gfx_SetTextXY(0, 0);

	for (i = 0; i < buffer_length; i++) {
		
		gfx_SetTextXY(gfx_GetTextX(), 0);
		
		if (*(buffer + i) != '\0') {
			gfx_PrintChar(*(buffer + i));
		} else {
			gfx_PrintChar('#');
		};
		
		if (buffer + i == curr_char_ptr) {
			gfx_SetTextXY(gfx_GetTextX() - gfx_GetCharWidth(*(buffer + i)), 14);
			gfx_PrintChar('^');
		};
	};
	
	return;
}
*/
void draw_buffer_contents(char *first_char, uint24_t max_width) {
	
	uint24_t i = 0;
	uint24_t curr_width = 0;
	
	gfx_SetColor(0xff);
	gfx_FillRectangle(INPUT_FIELD_X, INPUT_FIELD_Y, buffer_data.visible_buffer_width, 10);
	gfx_SetTextXY(INPUT_FIELD_X + 1, INPUT_FIELD_Y + 1);
	
	curr_width += gfx_GetCharWidth(*(first_char + i));
	while (curr_width < max_width && *(first_char + i) != '\0') {
		gfx_PrintChar(*(first_char + i));
		curr_width += gfx_GetCharWidth(*(first_char + i));
		i += 1;
	};
	return;
}

void draw_cursor(uint24_t cursor_x, uint8_t cursor_y, uint24_t counter) {
	
	gfx_SetColor(0x00);
	if (counter <= 100)
		gfx_SetColor(0xff);
	gfx_FillRectangle(cursor_x, cursor_y, 1, 9);
	return;
}

void input(void) {
	
	char *letters = "\0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?[VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";
	char buffer[10] = {'\0'};
	char *buffer_ptr = buffer;
	
	uint24_t cursor_x = INPUT_FIELD_X;
	uint8_t cursor_y = INPUT_FIELD_Y;
	char character;
	int offset;
	uint24_t i;
	
	setup_gfx_textio();
	
    buffer_data.first_visible_char = buffer;
    buffer_data.visible_buffer_width = 50;
	
	/* This buffer size should not count the null terminator. */
	textio_SetCurrCharPtr(buffer);
	textio_SetBufferSize(9);
	
	for(;;) {
		/*
		gfx_FillScreen(0xff);
		draw_debug(buffer, textio_GetCurrCharPtr(), textio_GetBufferSize());
		
		// Debugging
		gfx_SetTextXY(0, 30);
		gfx_PrintUInt(textio_GetStringWidthL(buffer_data.first_visible_char, textio_GetCurrCharPtr() - buffer_data.first_visible_char + 1), 3);
		gfx_SetTextXY(0, 40);
		gfx_PrintUInt(textio_KeyToOffset(), 2);
		*/
		// Draw input box
		gfx_SetColor(0x00);
		gfx_Rectangle(INPUT_FIELD_X - 2, INPUT_FIELD_Y - 2, buffer_data.visible_buffer_width + 3, 13);
		
		draw_buffer_contents(buffer_data.first_visible_char, buffer_data.visible_buffer_width);
		
		i = 200;
		
		do {
			kb_Scan();
			draw_cursor(cursor_x, cursor_y, i);
			i--;
			if (i == 0)
				i = 200;
		} while (!kb_AnyKey());
		
		if (!(kb_Data[6] & kb_Enter)) {
			
			offset = textio_KeyToOffset();
			if (offset > -1) {
				character = letters[offset];
			
				if (character != '\0') {
					if (!textio_WriteChar(buffer, character)) {
						if (gfx_GetStringWidth(buffer_data.first_visible_char) > buffer_data.visible_buffer_width) {
							buffer_data.first_visible_char++;
						} else {
							cursor_x += gfx_GetCharWidth(character);
						};
					};
				};
			};
		};
		
		if (kb_Data[1] & kb_Del && textio_GetCurrCharPtr() > buffer) {
			if (!((textio_GetCurrCharPtr() == buffer_data.first_visible_char + 1) && (buffer_data.first_visible_char > buffer)))
			{
				cursor_x -= textio_DeleteChar(buffer);
			} else {
				buffer_data.first_visible_char--;
				textio_DeleteChar(buffer);
			};
		};
		
		if (kb_Data[6] & kb_Clear) {
			cursor_x -= textio_Clear(buffer, buffer, textio_GetCurrCharPtr() - buffer_ptr);
			if (cursor_x < INPUT_FIELD_X)
				cursor_x = INPUT_FIELD_X;
			
			textio_SetCurrCharPtr(buffer);
			buffer_data.first_visible_char = buffer;
		};
		
		if (kb_Data[7] & kb_Left && textio_GetCurrCharPtr() > buffer) {
			
			if ((textio_GetCurrCharPtr() == buffer_data.first_visible_char + 1) && buffer_data.first_visible_char > buffer) {
				buffer_data.first_visible_char--;
			} else {
				cursor_x -= gfx_GetCharWidth(*textio_GetCurrCharPtr());
			};
			
			textio_SetCurrCharPtr(textio_GetCurrCharPtr() - 1);
		};
		
		if (kb_Data[7] & kb_Right && *textio_GetCurrCharPtr() != '\0') {
			
			textio_SetCurrCharPtr(textio_GetCurrCharPtr() + 1);
			if (textio_GetStringWidthL(buffer_data.first_visible_char, textio_GetCurrCharPtr() - buffer_data.first_visible_char + 1) > buffer_data.visible_buffer_width) {
				buffer_data.first_visible_char++;
			} else {
				cursor_x += gfx_GetCharWidth(*textio_GetCurrCharPtr());
			};
		};
		
		if (kb_Data[6] & kb_Enter)
			break;
		
		delay(100);
	};

	return;
}

void main(void) {
    
    gfx_Begin();
    
	input();
    
    gfx_End();
    return;
}