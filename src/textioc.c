#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>		// For va_arg, va_list, va_start, va_end
#include <math.h>
#include <string.h>
#include <tice.h>
#include <graphx.h>

#include <keypadc.h>

// Debugging
#include <debug.h>

typedef struct {
	uint8_t text_FG_color;
	uint8_t cursor_color;
	bool prgm_name_flag;
} input_t;

input_t input_config;

const char *uppercase_letters  = "\0\0\0\0\0\0\0\0\0\0\0WRMH\0\0\0\0VQLG\0\0\0ZUPKFC\0\x20YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";
const char *lowercase_letters  = "\0\0\0\0\0\0\0\0\0\0\0wrmh\0\0\0\0vqlg\0\0\0zupkfc\0\x20ytojeb\0\0xsnida\0\0\0\0\0\0\0\0";
const char *numerals = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x33\x36\x39\0\0\0\0\0\x32\x35\x38\0\0\0\0\x30\x31\x34\x37\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

// prgm_name_flag can be set manually or by textio_PrgmNameInput

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
	
	// Worst case scenerio
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

void textio_SetInputColorConfig(uint8_t text_FG_color, uint8_t cursor_color) {
	input_config.text_FG_color = text_FG_color;
	input_config.cursor_color = cursor_color;
}

void textio_SetPrgmNameFlag(bool prgm_name_flag) {
	input_config.prgm_name_flag = prgm_name_flag;
}

// Usage: ..., const char *keymap1, const char *keymap1_indicator, const char *keymap2, ...
void textio_CustomInput(char *buffer, uint8_t buffer_size, uint24_t visible_input_width, uint24_t xPos, uint8_t yPos, uint8_t total_keymap_num, ...) {
	
	char *visible_buffer;
	char *last_visible_char;
	char *curr_char;
	char *temp1;
	char *temp2;
	uint8_t text_BG_color;
	uint24_t cursor_xPos = xPos;
	uint24_t i;
	bool cursor_active = false;
	uint8_t key = '\0';

	va_list keymap_list;
	char *curr_keymap;
	const char *curr_indicator;
	uint8_t curr_keymap_num = 2;
	
	// Debugging vars
	uint8_t j;
	char *letter_chk = '\0';
	
	// Setup
	gfx_SetTextScale(1, 1);
	gfx_SetTextFGColor(input_config.text_FG_color);
	curr_char = buffer;
	visible_buffer = buffer;
	va_start(keymap_list, total_keymap_num);
	curr_keymap = va_arg(keymap_list, char*);
	curr_indicator = va_arg(keymap_list, const char*);
	
	for(;;) {
		
		cursor_active = false;
		if (curr_char < buffer + buffer_size)
			cursor_active = true;
		
		text_BG_color = gfx_GetPixel(cursor_xPos, yPos);
		gfx_SetTextBGColor(text_BG_color);
		gfx_SetTextTransparentColor(text_BG_color);
		
		// Erase all visible input and redraw it
		gfx_SetColor(text_BG_color);
		gfx_FillRectangle(xPos, yPos, visible_input_width, 9);
		gfx_SetTextXY(xPos + 1, yPos + 1);
		
		cursor_xPos = xPos;
		temp1 = visible_buffer;
		i = 0;
		while (*temp1 != '\0') {
			i += gfx_GetCharWidth(*temp1);
			if (i > visible_input_width)
				break;
			gfx_PrintChar(*temp1++);
			if (temp1 == curr_char)
				cursor_xPos += i;
		};
		
		if (curr_char == buffer && *curr_char == '\0'){
			last_visible_char = buffer;
		} else {
			last_visible_char = --temp1;
		};
		
		
		// Debugging code -----------------------------------
		dbg_sprintf(dbgout, "cursor_xPos = %d   | *visible_buffer = %c\n", cursor_xPos, *visible_buffer);
		dbg_sprintf(dbgout, "cursor_active = %d | *curr_char = %c\n", cursor_active, *curr_char);
		dbg_sprintf(dbgout, "*last_visible_char = %c\n", *last_visible_char);
		dbg_sprintf(dbgout, "Buffer:");
	
		letter_chk = buffer;
		for (j = 0; j < buffer_size; j++)
			dbg_sprintf(dbgout, " %x", *letter_chk++);
		
		dbg_sprintf(dbgout, " |");
	
		letter_chk = buffer;
		for (j = 0; j < buffer_size; j++) {
			if (*letter_chk == '\0') {
				dbg_sprintf(dbgout, ".");
			} else {
				dbg_sprintf(dbgout,"%c", *letter_chk++);
			};
		};
		
		dbg_sprintf(dbgout, "|\n");
		// End of debugging code ----------------------------
		
		
		// Wait for input and display cursor
		do {
			if (cursor_active) {
				gfx_SetColor(input_config.cursor_color);
				gfx_Line(cursor_xPos, yPos, cursor_xPos, yPos + 8);
			};
			
			i = 0;
			
			do {
				key = os_GetCSC();
			} while (!key && i++ < 4000);
			
			if (cursor_active) {
				gfx_SetColor(text_BG_color);
				gfx_Line(cursor_xPos, yPos, cursor_xPos, yPos + 8);
			};
			
			while (!key && i++ < 8000)
				key = os_GetCSC();;
			
		} while (!key);
		
		/* Exit if the cursor is at the start of the buffer and there are no chars 
		   in front of the cursor */
		if (key == sk_Clear && curr_char == buffer && *curr_char == '\0') {
			va_end(keymap_list);
			return;
		};
		
		if (key == sk_Enter) {
			// Debugging code omitted
			va_end(keymap_list);
			return;
		};
		
		if (key == sk_Clear) {
			if (curr_char == buffer) {
				temp1 = curr_char;
				while (*temp1 != '\0')
					*temp1++ = '\0';
			};
			
			/* If the cursor is in the middle of a word, shift the chars in front of
			   the cursor back */
			while (curr_char > buffer) {
				temp1 = curr_char;
				curr_char--;
				while (*temp1 != '\0') {
					temp2 = temp1;
					*(temp1 - 1) = *temp2;
					temp1++;
				};
				*--temp1 = '\0';
			};
			visible_buffer = buffer;
		};
		
		if (key == sk_Del && curr_char > buffer) {
			if (visible_buffer != buffer)
				visible_buffer--;
			
			*--curr_char = '\0';
			
			/* If there are any chars in front of the cursor, shift them back */
			if (curr_char < buffer + buffer_size) {
				if (*(curr_char + 1) != '\0') {
					temp1 = curr_char;
					do {
						temp2 = ++temp1;
						*(temp1 - 1) = *temp2;
					} while (*temp1 != '\0');
				};
			};
		} else {
			// Debugging code
			dbg_sprintf(dbgout, "No char deleted\n");
		};
		
		if (key == sk_Left && curr_char > buffer) {
			if (curr_char == visible_buffer)
				visible_buffer--;
			curr_char--;
		};
		
		if (key == sk_Right && *curr_char != '\0') {
			if (*++curr_char != '\0' && (curr_char - 1) == last_visible_char)
				visible_buffer++;
		};
		
		if (key == sk_Alpha) {
			if (curr_keymap_num == total_keymap_num) {
				va_start(keymap_list, total_keymap_num);
				curr_keymap_num = 0;
			};
			
			curr_keymap = va_arg(keymap_list, char*);
			curr_indicator = va_arg(keymap_list, const char*);
			curr_keymap_num += 2;
			
			gfx_SetTextBGColor(0x00);
			gfx_SetTextFGColor(0xFF);
			gfx_SetTextTransparentColor(0x00);
			gfx_SetColor(0x00);
			
			gfx_FillRectangle(cursor_xPos, yPos, 9, 9);
			gfx_PrintStringXY(curr_indicator, cursor_xPos + 1, yPos + 1);
			
			delay(400);
			gfx_SetColor(text_BG_color);
			gfx_FillRectangle(cursor_xPos, yPos, 9, 9);
			gfx_SetTextBGColor(text_BG_color);
			gfx_SetTextFGColor(input_config.text_FG_color);
			gfx_SetTextTransparentColor(text_BG_color);
		};
		
		// The second arg verifies that there is space in the buffer for another char
		if (cursor_active && *(buffer + buffer_size - 1) == '\0' && curr_keymap[key]) {
			
			/* If there are any chars in front of the cursor, shift them forward */
			if (*curr_char != '\0') {
				temp1 = buffer + buffer_size;
				while (temp1 > curr_char) {
					temp2 = --temp1;
					*(temp1 + 1) = *temp2;
				};
			};
			
			if (!input_config.prgm_name_flag || (input_config.prgm_name_flag && !(curr_keymap == numerals && curr_char == buffer))) {
				*curr_char = curr_keymap[key];
				if (cursor_xPos - xPos + gfx_GetCharWidth(*curr_char++) > visible_input_width)
					visible_buffer++;
			};
		} else {
			// Debugging code
			dbg_sprintf(dbgout, "No char added\n");
		};
		
		
	};	
}

void textio_LetterInput(char *buffer, uint8_t buffer_size, uint24_t visible_input_width, uint24_t xPos, uint8_t yPos) {
	textio_SetPrgmNameFlag(false);
	textio_CustomInput(buffer, buffer_size, visible_input_width, xPos, yPos, 4, uppercase_letters, "A", lowercase_letters, "a");
}

void textio_NumericalInput(char *buffer, uint8_t buffer_size, uint24_t visible_input_width, uint24_t xPos, uint8_t yPos) {
	textio_SetPrgmNameFlag(false);
	textio_CustomInput(buffer, buffer_size, visible_input_width, xPos, yPos, 2, numerals, "1");
}

void textio_LetterNumInput(char *buffer, uint8_t buffer_size, uint24_t visible_input_width, uint24_t xPos, uint8_t yPos) {
	textio_SetPrgmNameFlag(false);
	textio_CustomInput(buffer, buffer_size, visible_input_width, xPos, yPos, 6, uppercase_letters, "A", lowercase_letters, "a", numerals, "1");
}

void textio_PrgmNameInput(char *buffer, uint24_t visible_input_width, uint24_t xPos, uint8_t yPos) {
	textio_SetPrgmNameFlag(true);
	textio_CustomInput(buffer, 8, visible_input_width, xPos, yPos, 4, uppercase_letters, "A", numerals, "1");
}