#include <fontlibc.h>
#include <graphx.h>
#include <keypadc.h>
#include <tice.h>
#include <textioc.h>

#include "fonts/fonts.h"

#define INPUT_FIELD_X	100
#define INPUT_FIELD_Y	150


void setup_fontlib_textio(void)
{
	/* Setup the FontLib wrapper. */
	textio_library_routines_t routines = TEXTIO_FONTLIB_ROUTINES;

	/* Pass the wrapper pointers to TextIOC. */
	textio_SetLibraryRoutines(&routines);
	return;
}

void draw_buffer_contents(char *first_visible_char, uint24_t max_width)
{
	uint24_t i = 0;
	uint24_t curr_width = 0;
	
	gfx_SetColor(0xff);
	gfx_FillRectangle(INPUT_FIELD_X, INPUT_FIELD_Y, max_width + 7, fontlib_GetCurrentFontHeight());
	fontlib_SetCursorPosition(INPUT_FIELD_X + 1, INPUT_FIELD_Y + 1);
	
	curr_width += fontlib_GetGlyphWidth(*(first_visible_char + i));
	while (curr_width < max_width && *(first_visible_char + i) != '\0')
	{
		fontlib_DrawGlyph(*(first_visible_char + i));
		curr_width += fontlib_GetGlyphWidth(*(first_visible_char + i));
		i += 1;
	};
	return;
}

void draw_cursor(uint24_t cursor_x, uint8_t cursor_y, uint24_t counter)
{
	gfx_SetColor(0x00);
	if (counter <= 100)
		gfx_SetColor(0xff);
	gfx_FillRectangle(cursor_x, cursor_y, 1, fontlib_GetCurrentFontHeight());
	return;
}

void input(void) {
	
	char *letters = "\0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?[VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";
	char buffer[10] = {'\0'};
	
	uint24_t cursor_x = INPUT_FIELD_X;
	uint8_t cursor_y = INPUT_FIELD_Y;
	char character;
	int offset;
	uint24_t i;
	
	char *curr_char_ptr = buffer;
	uint24_t buffer_size = 9;
	char *first_visible_char = buffer;
	uint24_t visible_buffer_width = 50;
	textio_output_data_t data = TEXTIO_DEFAULT_OUTPUT_DATA;
	textio_output_data_t *output_data = &data;
	
	setup_fontlib_textio();
	
	fontlib_SetWindow(INPUT_FIELD_X, INPUT_FIELD_Y, visible_buffer_width, fontlib_GetCurrentFontHeight());
	fontlib_SetLineSpacing(0, 0);
	
	for(;;)
	{
		gfx_SetColor(0x00);
		gfx_Rectangle(INPUT_FIELD_X - 2, INPUT_FIELD_Y - 2, visible_buffer_width + 10, fontlib_GetCurrentFontHeight() + 4);
		
		draw_buffer_contents(first_visible_char, visible_buffer_width);
		
		if ((cursor_x = INPUT_FIELD_X + textio_GetStringWidthL(output_data, first_visible_char, curr_char_ptr - first_visible_char)) - INPUT_FIELD_X > visible_buffer_width)
		{
			first_visible_char++;
		};
		
		i = 200;
		
		do
		{
			kb_Scan();
			draw_cursor(cursor_x, cursor_y, i);
			i--;
			if (i == 0)
			{
				i = 200;
			};
		}
		while (!kb_AnyKey());
		
		if (!(kb_Data[6] & kb_Enter))
		{
			offset = textio_KeyToOffset();
			if (offset > -1)
			{
				character = letters[offset];
				if (character != '\0')
				{
					if (!textio_InsertChar(buffer, buffer_size, character, curr_char_ptr))
					{
						curr_char_ptr++;
						while (textio_GetStringWidthL(output_data, first_visible_char, curr_char_ptr - first_visible_char) > visible_buffer_width)
						{
							first_visible_char++;
						};
					};
				};
			};
		};
		
		if (kb_Data[1] & kb_Del && curr_char_ptr > buffer)
		{
			curr_char_ptr--;
			if (first_visible_char > buffer)
			{
				first_visible_char--;
				
			};
			textio_ShiftDeleteChar(buffer, buffer_size, curr_char_ptr);
		};
		
		if (kb_Data[6] & kb_Clear)
		{
			textio_DeleteString(buffer, buffer_size);
			cursor_x = INPUT_FIELD_X;
			curr_char_ptr = buffer;
			first_visible_char = buffer;
		};
		
		if (kb_Data[7] & kb_Left && curr_char_ptr > buffer)
		{
			if ((curr_char_ptr == first_visible_char + 1) && first_visible_char > buffer)
			{
				first_visible_char--;
			};
			curr_char_ptr--;
		};
		
		if (kb_Data[7] & kb_Right && *curr_char_ptr != '\0')
		{
			curr_char_ptr++;
			if (textio_GetStringWidthL(output_data, first_visible_char, curr_char_ptr - first_visible_char + 1) > visible_buffer_width)
			{
				first_visible_char++;
			};
		};
		
		if (kb_Data[6] & kb_Enter)
		{
			break;
		};
		
		delay(100);
	};

	return;
}

void main(void)
{
	gfx_Begin();
	fontlib_SetFont(test_font, 0);
	input();
	gfx_End();
	return;
}