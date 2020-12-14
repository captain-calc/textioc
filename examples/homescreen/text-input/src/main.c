#include <keypadc.h>
#include <tice.h>
#include <textioc.h>

#include <stdbool.h>
#include <string.h>

#define INPUT_FIELD_X	0
#define INPUT_FIELD_Y	0

void setup_tios_textio(void)
{
	/* Setup the GraphX wrapper. */
	textio_library_routines_t routines = TEXTIO_TIOS_ROUTINES;

	/* Pass the wrapper pointers to TextIOC. */
	textio_SetLibraryRoutines(&routines);
	return;
}

void draw_buffer_contents(char *buffer, uint24_t buffer_size)
{
	uint8_t i;

	os_SetCursorPos(INPUT_FIELD_Y, INPUT_FIELD_X);
	for (i = 0; i < buffer_size + 1; i++)
	{
		os_PutStrFull(" ");
	};
	os_SetCursorPos(INPUT_FIELD_Y, INPUT_FIELD_X);
	os_PutStrFull(buffer);
	return;
}

void input(void)
{
	
	char *letters = "\0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?[VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";
	char buffer[30] = {'\0'};
	char cursor_char[2] = {'\0'};
	uint8_t i;
	
	uint8_t cursor_x = INPUT_FIELD_X;
	uint8_t cursor_y = INPUT_FIELD_Y;
	char character;
	int offset;
	
	char *curr_char_ptr = buffer;
	uint24_t buffer_size = 29;
	
	setup_tios_textio();
	
	for(;;)
	{
		draw_buffer_contents(buffer, buffer_size);
		
		cursor_y = (curr_char_ptr - buffer) / 26;
		cursor_x = (curr_char_ptr - buffer) % 26;
		os_SetCursorPos(cursor_y, cursor_x);
		
		i = 0;
		do
		{
			kb_Scan();
			os_EnableCursor();
			if (i > 100)
			{
				if (*curr_char_ptr != '\0')
				{
					*cursor_char = *curr_char_ptr;
				} else {
					*cursor_char = ' ';
				};
				os_PutStrFull((const char *)cursor_char);
				os_SetCursorPos(cursor_y, cursor_x);
			};
			if (i == 200)
			{
				i = 0;
			};
			i++;
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
						cursor_x++;
					};
				};
			};
		};
		
		if (kb_Data[1] & kb_Del && *curr_char_ptr != '\0')
		{
			textio_ShiftDeleteChar(buffer, buffer_size, curr_char_ptr);
		};
		
		
		if (kb_Data[6] & kb_Clear)
		{
			textio_DeleteString(buffer, buffer_size);
			curr_char_ptr = buffer;
		};
		
		if (kb_Data[7] & kb_Left && curr_char_ptr > buffer)
		{
			cursor_x--;
			curr_char_ptr--;
		};
		
		if (kb_Data[7] & kb_Right && *curr_char_ptr != '\0')
		{
			
			curr_char_ptr++;
			cursor_x++;
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
	/* Setup screen for demo. */
	os_ClrHome();

	input();
    return;
}