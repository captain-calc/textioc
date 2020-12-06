#include <keypadc.h>
#include <tice.h>
#include <textioc.h>

#include <stdio.h>
#include <string.h>

#define INPUT_FIELD_X	0
#define INPUT_FIELD_Y	0

typedef struct
{
	char *first_visible_char;
	uint8_t visible_buffer_width;
}
buffer_data_t;

buffer_data_t buffer_data;

void setup_tios_textio(void)
{
	/* Setup the GraphX wrapper. */
	textio_library_routines_t routines = TEXTIO_TIOS_ROUTINES;

	/* Pass the wrapper pointers to TextIOC. */
	textio_SetLibraryRoutines(&routines);
	return;
}

void draw_buffer_contents(char *buffer, uint24_t visible_buffer_width)
{
	uint8_t i;

	os_SetCursorPos(INPUT_FIELD_Y, INPUT_FIELD_X);
	for (i = 0; i < visible_buffer_width + 1; i++)
		os_PutStrFull(" ");
	os_SetCursorPos(INPUT_FIELD_Y, INPUT_FIELD_X);
	os_PutStrFull(buffer);
	return;
}

void input(void)
{
	
	char *letters = "\0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?[VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";
	char buffer[30] = {'\0'};
	char *buffer_ptr = buffer;
	
	uint8_t cursor_x = INPUT_FIELD_X;
	uint8_t cursor_y = INPUT_FIELD_Y;
	char character;
	int offset;
	
	setup_tios_textio();
	
    buffer_data.first_visible_char = buffer;
    buffer_data.visible_buffer_width = 25;
	
	/* This buffer size should not count the null terminator. */
	textio_SetCurrCharPtr(buffer);
	textio_SetBufferSize(29);
	
	for(;;)
	{
		draw_buffer_contents(buffer_data.first_visible_char, buffer_data.visible_buffer_width);
		
		cursor_y = strlen(buffer_data.first_visible_char) / 26;
		os_SetCursorPos(cursor_y, cursor_x);
		
		do
		{
			kb_Scan();
			os_EnableCursor();
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
					if (!textio_WriteChar(buffer, character))
					{
						if (strlen(buffer_data.first_visible_char) > buffer_data.visible_buffer_width)
						{
							buffer_data.first_visible_char++;
						} else {
							cursor_x++;
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
				cursor_x--;
			};
			
			textio_SetCurrCharPtr(textio_GetCurrCharPtr() - 1);
		};
		
		if (kb_Data[7] & kb_Right && *textio_GetCurrCharPtr() != '\0') {
			
			textio_SetCurrCharPtr(textio_GetCurrCharPtr() + 1);
			if ((textio_GetCurrCharPtr() - buffer_data.first_visible_char + 1) > buffer_data.visible_buffer_width) {
				buffer_data.first_visible_char++;
			} else {
				cursor_x++;
			};
		};
		
		if (kb_Data[6] & kb_Enter)
			break;
		
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