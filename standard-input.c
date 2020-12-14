char *standard_input_8bpp(char *buffer, char **keymaps, uint24_t xPos, uint24_t yPos)
{
	return buffer;
}

char *gfx_timed_input(char *buffer, uint24_t buffer_size, uint24_t visible_buffer_width, char **keymaps, uint24_t xPos, uint24_t yPos, uint24_t timer)
{
	uint24_t cursor_width = 1;
	uint24_t cursor_height = 9;
	textio_input_data_t i_data = {
		buffer,
		buffer_size,
		buffer,
		buffer,
		visible_buffer_width
	}
	textio_input_data_t *input_data = &i_data;

	/*
	This line will setup the default newline, tab widths, and print formatting for
	GraphX. Custom data can be passed to o_data if you would prefer.
	*/
	textio_output_data_t o_data = TEXTIO_GRAPHX_OUTPUT_DATA;
	textio_output_data_t output_data = &o_data;
	uint8_t keymap_num = 0;
	char *keymap = keymaps[keymap_num];

	for (;;)
	{
		// Draw buffer contents
		
		// Update cursor position
		// Get key
		textio_TimedCursorRoutine_8bpp(cursor_x, cursor_y, cursor_width, cursor_height, 16, &timer);

		// Insert char handler
		if (!(kb_Data[6] & kb_Enter))
		{
			textio_GetChar(input_data, output_data, keymap)
		};

		// Delete char handler
		if (kb_Data[1] & kb_Del)
		{
			textio_ShiftDeleteChar(input_data->buffer, input_data->buffer_size, input_data->char_ptr);
			if (input_data->char_ptr > buffer)
			{
				input_data->char_ptr--;
			};
			if (input_data->char_ptr <= input_data->first_visible_char + 1 && input_data->first_visible_char > input_data-> buffer)
			{
				input_data->first_visible_char--;
			};
		};

		// Clear buffer handler
		if (kb_Data[6] & kb_Clear)
		{
			memset(input_data->buffer, '\0', input_data->buffer_size);
			input_data->char_ptr -> input_data->buffer;
			input_data->first_visible_char -> input_data->buffer;
		};

		// Move cursor left handler
		if (kb_Data[7] & kb_Left)
		{
			textio_MoveCursorLeft(input_data);
		};

		// Move cursor right handler
		if (kb_Data[7] & kb_Right)
		{
			textio_MoveCursorRight(input_data);
		};

		// Switch keymap handler
		if (kb_Data[1] & kb_Alpha)
		{
			if (++keymap_num == sizeof(keymaps))
			{
				keymap_num = 0;
			};
			keymap = keymaps[keymap_num];
		};

		// Return if timer runs out
		if (timer == 0)
		{
			break;
		};

		// Return if [enter] pressed
		if (kb_Data[6] & kb_Enter)
		{
			break;
		};
	};

	return buffer;
}

char *prgm_name_input_8bpp(char *buffer, uint24_t xPos, uint24_t yPos)
{
	return buffer;
}

int main(void)
{
	char buffer[10] = {'\0'};
	char *uppercase_letters = "";
	char *lowercase_letters = "";
	char **keymaps = {
		uppercase_letters,
		lowercase_letters
	};

	buffer = timed_input(buffer, keymaps, 20, 20);
	return 0;
}
