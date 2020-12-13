#include <graphx.h>
#include <keypadc.h>
#include <tice.h>
#include <textioc.h>

#define INPUT_FIELD_X	100
#define INPUT_FIELD_Y	150

void setup_gfx_textio(void) 
{
	/* Setup the GraphX wrapper. */
	textio_library_routines_t routines = TEXTIO_GRAPHX_ROUTINES;

	/* Pass the wrapper pointers to TextIOC. */
	textio_SetLibraryRoutines(&routines);
	return;
}

void draw_buffer_contents(char *first_visible_char, uint24_t max_width)
{
	uint24_t i = 0;
	uint24_t curr_width = 0;
	
	gfx_SetColor(0xff);
	gfx_FillRectangle(INPUT_FIELD_X, INPUT_FIELD_Y, max_width + 7, 9);
	gfx_SetTextXY(INPUT_FIELD_X + 1, INPUT_FIELD_Y + 1);
	
	curr_width += gfx_GetCharWidth(*(first_visible_char + i));
	while (curr_width < max_width && *(first_visible_char + i) != '\0')
	{
		gfx_PrintChar(*(first_visible_char + i));
		curr_width += gfx_GetCharWidth(*(first_visible_char + i));
		i += 1;
	};
	return;
}

void draw_cursor(uint24_t cursor_x, uint8_t cursor_y, uint24_t counter)
{
	gfx_SetColor(0x00);
	if (counter <= 100)
		gfx_SetColor(0xff);
	gfx_FillRectangle(cursor_x, cursor_y, 1, 9);
	return;
}

void input(void) {
	
	char *letters = "\0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?[VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";
	char buffer[10] = {'\0'};
	
	uint24_t cursor_x = INPUT_FIELD_X;
	uint8_t cursor_y = INPUT_FIELD_Y;
	uint24_t i;
	textio_input_data_t i_data = {
		buffer,
		9,
		buffer,
		buffer,
		50
	};
	textio_input_data_t *input_data = &i_data;
	textio_output_data_t o_data = TEXTIO_GRAPHX_OUTPUT_DATA;
	textio_output_data_t *output_data = &o_data;
	
	setup_gfx_textio();
	
	for(;;)
	{
		gfx_SetColor(0x00);
		gfx_Rectangle(INPUT_FIELD_X - 2, INPUT_FIELD_Y - 2, input_data->visible_buffer_width + 10, 13);
		
		draw_buffer_contents(input_data->first_visible_char, input_data->visible_buffer_width);
		
		while ((cursor_x = INPUT_FIELD_X + textio_GetStringWidthL(output_data, input_data->first_visible_char, input_data->char_ptr - input_data->first_visible_char)) - INPUT_FIELD_X > input_data->visible_buffer_width)
		{
			input_data->first_visible_char++;
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
		
		/*
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
		*/
		
		if (!(kb_Data[6] & kb_Enter))
		{
			textio_GetChar(input_data, output_data, letters);
		};
		
		if (kb_Data[1] & kb_Del && input_data->char_ptr > buffer)
		{
			input_data->char_ptr--;
			if (input_data->first_visible_char > buffer)
			{
				input_data->first_visible_char--;
			};
			textio_ShiftDeleteChar(input_data->buffer, input_data->buffer_size, input_data->char_ptr);
		};
		
		if (kb_Data[6] & kb_Clear)
		{
			textio_DeleteString(input_data->buffer, input_data->buffer_size);
			input_data->char_ptr = input_data->buffer;
			input_data->first_visible_char = input_data->buffer;
		};
		
		if (kb_Data[7] & kb_Left && input_data->char_ptr > buffer)
		{
			if ((input_data->char_ptr == input_data->first_visible_char + 1) && input_data->first_visible_char > input_data->buffer)
			{
				input_data->first_visible_char--;
			};
			input_data->char_ptr--;
		};
		
		if (kb_Data[7] & kb_Right && *(input_data->char_ptr) != '\0')
		{
			input_data->char_ptr++;
			if (textio_GetStringWidthL(output_data, input_data->first_visible_char, input_data->char_ptr - input_data->first_visible_char + 1) > input_data->visible_buffer_width)
			{
				input_data->first_visible_char++;
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
	input();
	gfx_End();
	return;
}