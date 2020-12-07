#include <keypadc.h>
#include <tice.h>
#include <textioc.h>

#include <stdio.h>
#include <string.h>

// Debugging
#define dbgout ((char*)0xFB0000)
#define dbgerr ((char*)0xFC0000)
#define dbg_sprintf sprintf

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
	
	/* The TextIOC functions modify the first two variables. */
    char *first_visible_char = buffer;
	char *curr_char_ptr = buffer;
	uint24_t buffer_size = 4;
    uint24_t visible_buffer_width = 25;
	
	//Debugging
	dbg_sprintf(dbgout, "buffer = 0x%6x\n", buffer);
	
	setup_tios_textio();
	
	for(;;)
	{
		draw_buffer_contents(first_visible_char, visible_buffer_width);
		
		cursor_y = strlen(first_visible_char) / 26;
		os_SetCursorPos(cursor_y, cursor_x);
		
		// Debugging
		dbg_sprintf(dbgout, "cursor_y = %d | cursor_x = %d\n", cursor_y, cursor_x);
		
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
					// Debugging
					dbg_sprintf(dbgout, "curr_char_ptr = 0x%6x\n", curr_char_ptr);
					if (!textio_WriteChar(buffer, buffer_size, &curr_char_ptr, character))
					{
						if (strlen(first_visible_char) > visible_buffer_width)
						{
							first_visible_char++;
						} else {
							cursor_x++;
						};
					};
					
					// Debugging
					dbg_sprintf(dbgout, "curr_char_ptr = 0x%6x\n", curr_char_ptr);
				};
			};
		};
		
		if (kb_Data[1] & kb_Del && curr_char_ptr > buffer) {
			if (!((curr_char_ptr == first_visible_char + 1) && (first_visible_char > buffer)))
			{
				cursor_x -= textio_DeleteChar(buffer, &curr_char_ptr, buffer_size);
			} else {
				first_visible_char--;
				textio_DeleteChar(buffer, &curr_char_ptr, buffer_size);
			};
		};
		
		/* Variables in block:
		 *		curr_char_ptr (modified)
		 *		buffer
		 *		first_visible_char (modified)
		 *		visible_buffer_width
		 *		cursor_x (modified)
		*/
		if (kb_Data[7] & kb_Left && curr_char_ptr > buffer)
		{
			if ((curr_char_ptr == first_visible_char + 1) && first_visible_char > buffer)
			{
				first_visible_char--;
			} else {
				cursor_x--;
			};
			
			curr_char_ptr--;
		};
		
		/* Variables in block:
		 *		curr_char_ptr (modified)
		 *		first_visible_char (modified)
		 *		visible_buffer_width
		 *		cursor_x (modified)
		*/
		if (kb_Data[7] & kb_Right && *curr_char_ptr != '\0') {
			
			curr_char_ptr++;
			if ((curr_char_ptr - first_visible_char + 1) > visible_buffer_width) {
				first_visible_char++;
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