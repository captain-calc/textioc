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

void textio_PrintWrappedText(char text[], uint24_t width, uint8_t xPos, uint8_t yPos) {
	
	char *curr_line_ptr;
	uint24_t curr_line_width;
	char *last_char_ptr;
	
	curr_line_ptr = text;
	gfx_SetTextXY(xPos, yPos);
	
	// Debugging
	gfx_SetColor(224);
	gfx_FillRectangle(xPos + width, yPos, 2, 240);
	
	if (gfx_GetStringWidth(text) <= width) {
			gfx_PrintString(curr_line_ptr);
			return;
	};
	
	// Worst case scenerio
	if (width > 320)
		width = 320;
	
	do {
		gfx_SetTextXY(xPos, yPos);
		last_char_ptr = curr_line_ptr;
		curr_line_width = 0;
		
		// Increment the current selection char by char until it reaches the limit
		do {
			curr_line_width += gfx_GetCharWidth(*last_char_ptr++);
			//dbg_sprintf(dbgout, "*last_char_ptr = %c\n", *last_char_ptr);
		} while (curr_line_width < width && *last_char_ptr != '\0');
		
		//dbg_sprintf(dbgout, "Estimate done\n");
		
		// If the last char read was not a space and not the end of the text, decrement
		// the current selection char by char until you find a space
		while (*last_char_ptr != '\0' && *last_char_ptr != *curr_line_ptr && !isspace(*--last_char_ptr));
			//dbg_sprintf(dbgout, "*last_char_ptr = %c\n", *last_char_ptr);
		
		// If the selection never exceeded the first word, and the last_char_ptr is now
		// equal to curr_line_ptr, increment char by char until you find the first space
		if (*last_char_ptr == *curr_line_ptr)
			while (*last_char_ptr != '\0' && !isspace(*++last_char_ptr));
		
		*last_char_ptr = '\0';
		
		//dbg_sprintf(dbgout, "*curr_line_ptr = %c\n", *curr_line_ptr);
		gfx_PrintString(curr_line_ptr);
		curr_line_ptr = ++last_char_ptr;
		yPos += 10;
	} while (*last_char_ptr != '\0');
	
	return;
}

void textio_PrintTruncatedStringXY(char string[], uint24_t max_width, uint24_t xPos, uint8_t yPos) {
	
	uint24_t str_width;
	uint8_t dots_width;
	char *last_char_ptr;
	
	dots_width = gfx_GetStringWidth("...");
	gfx_SetTextXY(xPos, yPos);
	
	if (gfx_GetStringWidth(string) <= max_width) {
		gfx_PrintString(string);
		return;
	} else if (max_width < dots_width) {
		return;
	};
		
	last_char_ptr = string + strlen(string);
		
	do {
		
		*last_char_ptr-- = '\0';
		str_width = gfx_GetStringWidth(string);
	} while (str_width > (max_width - dots_width)) ;
	
	gfx_PrintString(strcat(string, "..."));
	return;
}

/*
long int textio_NumericalInput(uint24_t xPos, uint8_t yPos, uint8_t buffer_size) {

	char *buffer_ptr;						// Buffer for user input
	char *number_ptr;						// Holds current char
	char *temp_ptr;
	uint8_t text_BG_color;
	uint8_t text_FG_color = 0x00;
	uint24_t cursor_xPos = xPos + 1;
	uint24_t i;								// Counter variable
	bool cursor_active = false;
	uint8_t key = '\0';
	const char *numbers  = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0369\0\0\0\0\0258\0\0\0\00147\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	
	// Debugging variables
	//uint8_t j;
	//char *pLetter;
	
	buffer_ptr = malloc(buffer_size * sizeof(char) + 1);
	if (!buffer_ptr)
		return NULL;
	memset(buffer_ptr, '\0', buffer_size + 1);
	
	number_ptr = buffer_ptr;
	
	gfx_SetTextScale(1,1);
	gfx_SetTextFGColor(text_FG_color);
	
	for (;;) {
		
		gfx_SetTextXY(xPos, yPos);
		gfx_PrintString(buffer_ptr);
		cursor_active = false;
		
		if (number_ptr < buffer_ptr + buffer_size) {
			
			cursor_active = true;
			//dbg_sprintf(dbgout, "cursor_active = true\n");
		};
		
		cursor_xPos = xPos + 1;
		if (number_ptr > buffer_ptr)
			cursor_xPos += gfx_GetStringWidth(buffer_ptr);
		
		
		text_BG_color = gfx_GetPixel(cursor_xPos, yPos);
		gfx_SetTextBGColor(text_BG_color);
		gfx_SetTextTransparentColor(text_BG_color);
		
		/* Debugging
		dbg_sprintf(dbgout, "number_ptr = %x\nbuffer_ptr = %x\n", number_ptr, buffer_ptr);
		
		dbg_sprintf(dbgout, "buffer =");
	
		pLetter = buffer_ptr;
		for (j = 0; j < buffer_size; j++)
			dbg_sprintf(dbgout, "  %x", *pLetter++);
		
		dbg_sprintf(dbgout, "\t|");
		
		pLetter = buffer_ptr;
		for (j = 0; j < buffer_size; j++)
			dbg_sprintf(dbgout," %c ", *pLetter++);
		
		dbg_sprintf(dbgout, "|\n");
		*/ /*
	
	
		// Wait for input and display cursor
		do {
		
			if (cursor_active) {
				gfx_SetColor(0x00);
				gfx_Line(cursor_xPos, yPos - 1, cursor_xPos, yPos + 8);
			};
		
			i = 0;
		
			do {
				key = os_GetCSC();
				i++;
			} while (!key && i < 6000);
		
			if (cursor_active) {
				gfx_SetColor(text_BG_color);
				gfx_Line(cursor_xPos, yPos - 1, cursor_xPos, yPos + 8);			
			};
		
			while (!key && i < 12000) {
				key = os_GetCSC();
				i++;
			};
		} while (!key);
	
		if (key == sk_Clear && cursor_xPos == (xPos + 1)))
			return NULL;
		
		if (key == sk_Enter) {
			
			
			/* Debugging
			dbg_sprintf(dbgout, "buffer =");
	
			pLetter = buffer_ptr;
			for (j = 0; j < buffer_size; j++)
				dbg_sprintf(dbgout, "  %x", *pLetter++);
		
			dbg_sprintf(dbgout, "\t|");
		
			pLetter = buffer_ptr;
			for (j = 0; j < buffer_size; j++)
				dbg_sprintf(dbgout," %c ", *pLetter++);
		
			dbg_sprintf(dbgout, "|\n");
			*/ /*
			
			// Return only the chars entered and remove
			// all trailing null chars
			
			if (number_ptr > buffer_ptr) {
				
				temp_ptr = realloc(buffer_ptr, number_ptr - buffer_ptr + 1);
				if (temp_ptr != NULL) {
					
					//dbg_sprintf(dbgout,"temp_ptr = %s\n", temp_ptr);
					//dbg_sprintf(dbgout,"buffer_ptr = %s", buffer_ptr);
					return (long int)buffer_ptr;
				};
				
				//dbg_sprintf(dbgout,"realloc failed");
				return NULL;
			};
			//dbg_sprintf(dbgout,"number_ptr is equal to buffer_ptr");
			return NULL;
		};
		
		if (key == sk_Clear) {
			while (number_ptr > buffer_ptr)
				*--number_ptr = '\0';
			
			gfx_SetColor(text_BG_color);
			gfx_FillRectangle(xPos, yPos, cursor_xPos, 8);
		};
		
		if ((key == sk_Del) && number_ptr > buffer_ptr) {
			
			number_ptr--;
			
			// Debugging
			//dbg_sprintf(dbgout, "*number_ptr = %c\n&number_ptr = %x\n", *number_ptr, &number_ptr);
			//dbg_sprintf(dbgout, "Deleted char\n");
			
			// Erase char
			gfx_SetColor(text_BG_color);
			gfx_FillRectangle(cursor_xPos - gfx_GetCharWidth(*number_ptr) - 1, yPos, gfx_GetCharWidth(*number_ptr), 8);
			
			*number_ptr = '\0';
		} else {
			// Debugging
			//dbg_sprintf(dbgout, "Cannot delete char\n");
		};
				
		if (cursor_active && numbers[key]) {
			
			*number_ptr = numbers[key];
			
			// Debugging
			//dbg_sprintf(dbgout, "*number_ptr = %c\n&number_ptr = %x\n", *number_ptr, &number_ptr);
			
			number_ptr++;
		} else {
			// Debugging
			//dbg_sprintf(dbgout, "No char added\n");
		};
		
		delay(100);
	};
}
*/

void textio_LetterInput(char *buffer, uint8_t buffer_size, uint24_t xPos, uint8_t yPos) {
	
	char *curr_letter;						// Holds current char
	bool caps_on = true;
	char *temp1;							// For letter insertion and general-purpose char pointing
	char *temp2;
	uint8_t text_BG_color;
	uint8_t text_FG_color = 0x00;
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
	gfx_SetTextFGColor(text_FG_color);
	curr_letter = buffer;
	
	for (;;) {
		
		gfx_SetTextXY(xPos, yPos + 1);
		gfx_PrintString(buffer);
		
		cursor_active = false;
		if (curr_letter < buffer + buffer_size)
			cursor_active = true;

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
		
		// Debugging
		dbg_sprintf(dbgout, "cursor_xPos = %d\n", cursor_xPos);
		
		text_BG_color = gfx_GetPixel(cursor_xPos, yPos);
		gfx_SetTextBGColor(text_BG_color);
		gfx_SetTextTransparentColor(text_BG_color);
		
		// Debugging
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
		
	
	
		// Wait for input and display cursor
		do {
		
			if (cursor_active) {
				gfx_SetColor(0x00);
				gfx_Line(cursor_xPos, yPos, cursor_xPos, yPos + 8);
			};
		
			i = 0;
		
			do {
				key = os_GetCSC();
				i++;
			} while (!key && i < 6000);
		
			if (cursor_active) {
				gfx_SetColor(text_BG_color);
				// dbg_sprintf(dbgout, "text_BG_color = %d\n", text_BG_color);
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
			
			
			// Debugging
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
			
			return;
		};
		
		if (key == sk_Clear) {
			
			gfx_SetColor(text_BG_color);
			gfx_FillRectangle(xPos, yPos, gfx_GetStringWidth(buffer), 8);
			
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
			gfx_FillRectangle(cursor_xPos - gfx_GetCharWidth(*curr_letter), yPos, i, 8);

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
				gfx_PrintStringXY("a", cursor_xPos + 1, yPos);
			} else {
				caps_on = true;
				gfx_FillRectangle(cursor_xPos, yPos - 1, 9, 10);
				gfx_PrintStringXY("A", cursor_xPos + 1, yPos + 1);
			};
			
			delay(400);
			gfx_SetColor(text_BG_color);
			gfx_FillRectangle(cursor_xPos, yPos - 1, 9, 10);
			gfx_SetTextFGColor(text_FG_color);
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
				gfx_FillRectangle(cursor_xPos - gfx_GetCharWidth(*curr_letter), yPos, i, 8);
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
		
		delay(100);
	};
}