#include <string.h>
#include <graphx.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <tice.h>
#include <ctype.h>
#include <keypadc.h>

// Debugging
#include <debug.h>

typedef struct {
	uint8_t text_FG_color;
	uint8_t cursor_color;
} input_t;

input_t input_config;

void textio_PrintWrappedText(char *text, uint24_t width, uint24_t xPos, uint8_t yPos) {
	
	char *curr_line_ptr;
	uint24_t curr_line_width;
	char *char_ptr;
	bool end_of_text = false;
	
	curr_line_ptr = text;
	gfx_SetTextXY(xPos, yPos);
	
	// Debugging
	gfx_SetColor(224);
	gfx_FillRectangle(xPos + width, 0, 2, LCD_HEIGHT);
	
	if (gfx_GetStringWidth(text) <= width) {
			gfx_PrintString(curr_line_ptr);
			return;
	};
	
	// Worst case scenerios
	if (width > LCD_WIDTH)
		width = LCD_WIDTH;
	
	for(;;) {
		gfx_SetTextXY(xPos, yPos);
		char_ptr = curr_line_ptr;
		curr_line_width = 0;
		
		// Increment the current selection char by char until it reaches the limit
		while (curr_line_width < (width - gfx_GetCharWidth(*char_ptr)) && *char_ptr != '\0') {
			// Debugging
			dbg_sprintf(dbgout, "*char_ptr = %c\n", *char_ptr);
			curr_line_width += gfx_GetCharWidth(*char_ptr++);
		};
	
		// Debugging
		dbg_sprintf(dbgout, "Estimate done\n");
		
		// If the last char read was not a space and not the end of the text, decrement
		// the current selection char by char until you find a space
		while (*char_ptr != '\0' && char_ptr != curr_line_ptr && !isspace(*char_ptr)) {
			// Debugging
			dbg_sprintf(dbgout, "*char_ptr = %c\n", *char_ptr);
			char_ptr--;
		};
		
		// If the selection never exceeded the first word, and the char_ptr is now
		// equal to curr_line_ptr, increment char by char until you find the first space
		while (*char_ptr != '\0' && !isspace(*char_ptr))
			char_ptr++;
		
		if (*char_ptr == '\0')
			end_of_text = true;
		
		*char_ptr = '\0';
		
		// Debugging
		dbg_sprintf(dbgout, "Printing line...\n");
		dbg_sprintf(dbgout, "*curr_line_ptr = %c\n", *curr_line_ptr);
		gfx_PrintString(curr_line_ptr);
		if (!end_of_text) {
			curr_line_ptr = ++char_ptr;
			yPos += 10;
		} else {
			return;
		};
		
		if (yPos >= LCD_HEIGHT)
			return;
	};
}

void textio_PrintTruncatedStringXY(char string[], uint24_t max_width, uint24_t xPos, uint8_t yPos) {
	
	uint24_t str_width;
	uint8_t dots_width;
	char *char_ptr;
	
	dots_width = gfx_GetStringWidth("...");
	gfx_SetTextXY(xPos, yPos);
	
	if (gfx_GetStringWidth(string) <= max_width) {
		gfx_PrintString(string);
		return;
	} else if (max_width < dots_width) {
		return;
	};
		
	char_ptr = string + strlen(string);
		
	do {
		
		*char_ptr-- = '\0';
		str_width = gfx_GetStringWidth(string);
	} while (str_width > (max_width - dots_width)) ;
	
	gfx_PrintString(strcat(string, "..."));
	return;
}

void textio_SetInputConfig(uint8_t text_FG_color, uint8_t cursor_color) {
	input_config.text_FG_color = text_FG_color;
	input_config.cursor_color = cursor_color;
}

void textio_LetterInput(char *buffer, uint8_t buffer_size, uint24_t xPos, uint8_t yPos) {
	
	char *curr_letter;						// Holds current char
	bool caps_on = true;
	char *temp1;							// For letter insertion and general-purpose char pointing
	char *temp2;
	uint8_t text_BG_color;
	uint24_t cursor_xPos = xPos + 1;
	uint24_t i;								// Used for cursor timer variable and width of chars after cursor in delete routine
	bool cursor_active = false;
	uint8_t key = '\0';
	const char *lowercase_letters  = "\0\0\0\0\0\0\0\0\0\0\0wrmh\0\0\0\0vqlg\0\0\0zupkfc\0\0ytojeb\0\0xsnida\0\0\0\0\0\0\0\0";
	const char *uppercase_letters  = "\0\0\0\0\0\0\0\0\0\0\0WRMH\0\0\0\0VQLG\0\0\0ZUPKFC\0\0YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";
	
	// Debugging variables
	uint8_t j;
	char *letter_chk = '\0';
	
	gfx_SetTextScale(1,1);
	gfx_SetTextFGColor(input_config.text_FG_color);
	curr_letter = buffer;
	
	for (;;) {
		
		gfx_SetTextXY(xPos, yPos + 1);
		gfx_PrintString(buffer);
		
		cursor_active = false;
		if (curr_letter < buffer + buffer_size)
			cursor_active = true;

		// Debugging
		dbg_sprintf(dbgout, "cursor_active = %d\n", cursor_active);
		
		cursor_xPos = xPos + 1;
		if (curr_letter > 0) {
			temp1 = buffer;
			cursor_xPos -= 2;
			while (temp1 < curr_letter) {
				cursor_xPos += gfx_GetCharWidth(*temp1);
				temp1++;
			};
		};
		
		text_BG_color = gfx_GetPixel(cursor_xPos, yPos);
		gfx_SetTextBGColor(text_BG_color);
		gfx_SetTextTransparentColor(text_BG_color);
		
		// Debugging code
		dbg_sprintf(dbgout, "cursor_xPos = %d\n", cursor_xPos);

		dbg_sprintf(dbgout, "*curr_letter = %c\n", *curr_letter);
		
		dbg_sprintf(dbgout, "buffer =");
	
		letter_chk = buffer;
		for (j = 0; j <= buffer_size; j++)
			dbg_sprintf(dbgout, "  %x", *letter_chk++);
		
		dbg_sprintf(dbgout, "\t|");
		
		letter_chk = buffer;
		for (j = 0; j <= buffer_size; j++)
			dbg_sprintf(dbgout," %c ", *letter_chk++);
		
		dbg_sprintf(dbgout, "|\n");
		// End of debugging code
	
		// Wait for input and display cursor
		do {
		
			if (cursor_active) {
				gfx_SetColor(input_config.cursor_color);
				gfx_Line(cursor_xPos, yPos, cursor_xPos, yPos + 8);
			};
		
			i = 0;
		
			do {
				key = os_GetCSC();
				i++;
			} while (!key && i < 6000);
		
			if (cursor_active) {
				gfx_SetColor(text_BG_color);
				gfx_Line(cursor_xPos, yPos, cursor_xPos, yPos + 8);			
			};
		
			while (!key && i < 12000) {
				key = os_GetCSC();
				i++;
			};
		} while (!key);
	
	
		/* Exit if cursor is at the start of the buffer and
		   there are no chars in front of the cursor */
		if (key == sk_Clear && curr_letter == buffer && *curr_letter == '\0')
			return;
		
		
		if (key == sk_Enter) {
			
			// Debugging code
			dbg_sprintf(dbgout, "*curr_letter = %c\n", *curr_letter);
		
			dbg_sprintf(dbgout, "buffer =");
	
			letter_chk = buffer;
			for (j = 0; j <= buffer_size; j++)
				dbg_sprintf(dbgout, "  %x", *letter_chk++);
		
			dbg_sprintf(dbgout, "\t|");
		
			letter_chk = buffer;
			for (j = 0; j <= buffer_size; j++)
				dbg_sprintf(dbgout," %c ", *letter_chk++);
		
			dbg_sprintf(dbgout, "|\n");
			// End of debugging code
			
			return;
		};
		
		if (key == sk_Clear) {
			
			gfx_SetColor(text_BG_color);
			gfx_FillRectangle(xPos, yPos, gfx_GetStringWidth(buffer), 9);
			
			if (curr_letter == buffer) {
				temp1 = curr_letter;
				while (*temp1 != '\0')
					*temp1++ = '\0';
			};
			
			/* If the cursor is in the middle of a word
			   erase the chars before it and move the
			   chars in front of the cursor backwards */
			while (curr_letter > buffer) {
				temp1 = curr_letter;
				curr_letter--;
				while (*temp1 != '\0') {
					temp2 = temp1;
					*(temp1 - 1) = *temp2;
					temp1++;
				};
				*--temp1 = '\0';
			};
		};
		
		if ((key == sk_Del) && curr_letter > buffer) {
			
			// Debugging
			dbg_sprintf(dbgout, "Deleted char\n");
			
			curr_letter--;

			/* Erase char left of the cursor and any
			   chars to the right of the cursor */
			gfx_SetColor(text_BG_color);
			temp1 = curr_letter;
			i = 0;
			while (*temp1++ != '\0')
				i += gfx_GetCharWidth(*temp1);
			gfx_FillRectangle(cursor_xPos - gfx_GetCharWidth(*curr_letter), yPos, i, 9);

			*curr_letter = '\0';
			
			/* If there are any chars in front of the
			   cursor, shift them back */
			if (curr_letter < buffer + buffer_size) {
				if (*(curr_letter + 1) != '\0') {
					temp1 = curr_letter;
					do {
						temp1++;
						temp2 = temp1;
						*(temp1 - 1) = *temp2;
					} while (*temp1 != '\0');
				};
			};
			
		} else {
			// Debugging
			dbg_sprintf(dbgout, "Cannot delete char\n");
		};
		
		if (key == sk_Left && curr_letter > buffer)
			curr_letter--;
		
		if (key == sk_Right && *curr_letter != '\0')
			curr_letter++;
		
		if (key == sk_Alpha) {
			gfx_SetTextBGColor(0x00);
			gfx_SetTextFGColor(0xFF);
			gfx_SetTextTransparentColor(0x00);
			gfx_SetColor(0x00);
			
			if (caps_on) {
				caps_on = false;
				gfx_FillRectangle(cursor_xPos, yPos - 1, 9, 10);
				gfx_PrintStringXY("a", cursor_xPos + 1, yPos + 1);
			} else {
				caps_on = true;
				gfx_FillRectangle(cursor_xPos, yPos - 1, 9, 10);
				gfx_PrintStringXY("A", cursor_xPos + 1, yPos + 1);
			};
			
			delay(400);
			gfx_SetColor(text_BG_color);
			gfx_FillRectangle(cursor_xPos, yPos - 1, 9, 10);
			gfx_SetTextBGColor(text_BG_color);
			gfx_SetTextFGColor(input_config.text_FG_color);
			gfx_SetTextTransparentColor(text_BG_color);
		};
		
		if (cursor_active && *(buffer + buffer_size - 1) == '\0' && (uppercase_letters[key] || lowercase_letters[key])) {
			
			/* If there are any chars in front of
			   the cursor, move them forward before
			   adding the requested char */
			if (*curr_letter != '\0') {
				temp1 = buffer + buffer_size;
				while (temp1 > curr_letter) {
					temp1--;
					temp2 = temp1;
					*(temp1 + 1) = *temp2;
				};
			
				/* Erase chars to the right of the cursor */
				gfx_SetColor(text_BG_color);
				temp1 = curr_letter;
				i = 0;
				while (*temp1++ != '\0')
					i += gfx_GetCharWidth(*temp1);
				gfx_FillRectangle(cursor_xPos, yPos, i, 9);
			};
			
			if (caps_on) {
				*curr_letter = uppercase_letters[key];
				dbg_sprintf(dbgout, "Added uppercase char\n");
			} else {
				*curr_letter = lowercase_letters[key];
				dbg_sprintf(dbgout, "Added lowercase char\n");
			};
			
			curr_letter++;
		} else {
			// Debugging
			dbg_sprintf(dbgout, "No char added\n");
		};
		
		//delay(100);
	};
}