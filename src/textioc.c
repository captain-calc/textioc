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
	uint8_t curr_keymap_num;
	char *curr_char;
	char *visible_buffer;
} input_config_t;

input_config_t input_config;

typedef struct {
	uint24_t horiz_tab_width;
	uint8_t line_height;
} output_config_t;

output_config_t output_config;

const char *uppercase_letters  = "\0\0\0\0\0\0\0\0\0\0\0WRMH\0\0\0\0VQLG\0\0\0ZUPKFC\0\x20YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";
const char *lowercase_letters  = "\0\0\0\0\0\0\0\0\0\0\0wrmh\0\0\0\0vqlg\0\0\0zupkfc\0\x20ytojeb\0\0xsnida\0\0\0\0\0\0\0\0";
const char *numerals = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x33\x36\x39\0\0\0\0\0\x32\x35\x38\0\0\0\0\x30\x31\x34\x37\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

// This character set lacks the theta symbol.
const char *prgm_letters  = "\0\0\0\0\0\0\0\0\0\0\0WRMH\0\0\0\0VQLG\0\0\0ZUPKFC\0\0YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";


void textio_Setup(void) {
	input_config.text_FG_color = 0x00;
	input_config.cursor_color = 0x00;
	output_config.horiz_tab_width = 4;
	output_config.line_height = 9;
}


// INTERNAL LIBRARY ROUTINES
//============================================================

int iscntrl(int c) {
	switch (c) {
		case '\0':
		case '\n':
		case '\t':
		return 1;
		default:
		return 0;
	};
}

// This function works on the assumption that the longest word is <256 characters long
uint8_t getWordWidth(char *word) {
	char *c = word;
	uint8_t width = 0;

	while (!isspace(*c) && !iscntrl(*c) && width < 240)
		width += gfx_GetCharWidth(*c++);
	return width;
}
	

// TEXT OUTPUT FUNCTIONS
//============================================================


// IMPORTANT: The control character handling in this function does not work yet.

void textio_PrintTextXY(char *text, uint8_t initial_line_num, uint8_t num_lines, uint24_t xPos, uint8_t yPos, uint24_t line_width) {
	
	char *curr_char;
	uint24_t curr_line_width = 0;
	int word_width = 0;
	uint8_t curr_line_num = initial_line_num;
	uint8_t i;
	
	curr_char = text;
	gfx_SetTextXY(xPos, yPos);
	
	// Debugging
	gfx_SetColor(224);
	gfx_FillRectangle(xPos + line_width, 0, 2, LCD_HEIGHT);
	
	if (gfx_GetStringWidth(text) <= line_width) {
		gfx_PrintString(text);
		return;
	};
	
	// Worst case scenerio
	if (line_width > LCD_WIDTH)
		line_width = LCD_WIDTH;
	
	dbg_sprintf(dbgout, "Current Line: 1 -------------\n");
	
	for (;;) {

		if (curr_line_num > num_lines || yPos > LCD_HEIGHT)
			return;

		gfx_SetTextXY(xPos, yPos + (curr_line_num - initial_line_num) * output_config.line_height);
		curr_line_width = 0;

		while (*curr_char != '\0') {

			// Get the width of the next word
			word_width = getWordWidth(curr_char);
			dbg_sprintf(dbgout, "word_width = %d | Word: ", word_width);

			// If there is room on the current line for the word, print the word
			// else, start a new line
			if (curr_line_width + word_width < line_width) {
				curr_line_width += word_width;
				while (!iscntrl(*curr_char) && !isspace(*curr_char)) {
					dbg_sprintf(dbgout, "%c", *curr_char);
					gfx_PrintChar(*curr_char++);
				};
				dbg_sprintf(dbgout, "\n");
			} else {
				dbg_sprintf(dbgout, "Cannot put word on current line. Breaking...\n");
				goto startLine;
			};

			if (*curr_char == '\0')
				return;

			switch (*curr_char++) {
				
				case '\t':
				dbg_sprintf(dbgout, "Handling horizontal tab...\n");
				if (curr_line_width + (output_config.horiz_tab_width * gfx_GetCharWidth(' ')) < line_width) {
					for (i = 1; i < output_config.horiz_tab_width; i++) {
						gfx_PrintChar(' ');
						curr_line_width += gfx_GetCharWidth(' ');
					};
				} else {
					dbg_sprintf(dbgout, "Cannot put tab on current line. Breaking...\n");
					goto startLine;
				};
				break;
				
				case '\n':
				dbg_sprintf(dbgout, "Handling newline...\n");
				goto startLine;
				
				case ' ':
				dbg_sprintf(dbgout, "Handling space...\n");
				gfx_PrintChar(*(curr_char - 1));
				curr_line_width += gfx_GetCharWidth(' ');
				break;
			};

		};

		startLine:

		curr_line_num++;
		dbg_sprintf(dbgout, "Line: %d -------------\n", curr_line_num);
	};
}

void textio_PrintTruncatedStringXY(char string[], uint24_t max_line_width, uint24_t xPos, uint8_t yPos) {
	
	uint24_t str_line_width;
	uint8_t dots_line_width;
	char *char_ptr;
	
	dots_line_width = gfx_GetStringWidth("...");
	gfx_SetTextXY(xPos, yPos);
	
	if (gfx_GetStringWidth(string) <= max_line_width) {
		gfx_PrintString(string);
		return;
	} else if (max_line_width < dots_line_width) {
		return;
	};
		
	char_ptr = string + strlen(string);
		
	do {
		
		*char_ptr-- = '\0';
		str_line_width = gfx_GetStringWidth(string);
	} while (str_line_width > (max_line_width - dots_line_width)) ;
	
	gfx_PrintString(strcat(string, "..."));
	return;
}

void textio_SetLineHeight(uint8_t pxl_height) {
	output_config.line_height = pxl_height;
}

void textio_SetTabWidth(uint24_t width) {
	output_config.horiz_tab_width = width;
}


// TEXT INPUT FUNCTIONS
//============================================================



void textio_SetInputColorConfig(uint8_t text_FG_color, uint8_t cursor_color) {
	input_config.text_FG_color = text_FG_color;
	input_config.cursor_color = cursor_color;
}

void textio_SetPrgmNameFlag(bool prgm_name_flag) {
	input_config.prgm_name_flag = prgm_name_flag;
}

void textio_SetInitialKeymap(uint8_t keymap_num) {
	input_config.curr_keymap_num = keymap_num;
	input_config.curr_char = NULL;
	input_config.visible_buffer = NULL;
}

// Usage: ... uint8_t total_keymap_num, const char *keymap1, const char *keymap1_indicator, const char *keymap2, ...
uint8_t textio_Input(char *buffer, uint8_t buffer_size, uint24_t visible_input_line_width, uint24_t xPos, uint8_t yPos, uint8_t total_keymap_num, ...) {
	
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
	const char *curr_keymap;
	const char *curr_indicator;
	uint8_t curr_keymap_num;
	
	// Debugging vars
	uint8_t j;
	char *letter_chk = '\0';
	
	// Setup
	gfx_SetTextScale(1, 1);
	gfx_SetTextFGColor(input_config.text_FG_color);
	
	// curr_char and visible_buffer are saved to input_config to preserve them between function calls
	curr_char = input_config.curr_char;
	if (curr_char < buffer)
		curr_char = buffer;
	
	visible_buffer = input_config.visible_buffer;
	if (visible_buffer < buffer)
		visible_buffer = buffer;
	
	va_start(keymap_list, total_keymap_num);
	curr_keymap = va_arg(keymap_list, const char*);
	curr_indicator = va_arg(keymap_list, const char*);
	curr_keymap_num = 1;
	
	// Search the supplied keymaps for the last one used/set
	while (curr_keymap_num != input_config.curr_keymap_num) {
		curr_keymap = va_arg(keymap_list, const char*);
		curr_indicator = va_arg(keymap_list, const char*);
		curr_keymap_num++;
	};	
		
	cursor_active = false;
	if (curr_char < buffer + buffer_size)
		cursor_active = true;
		
	text_BG_color = gfx_GetPixel(cursor_xPos, yPos);
	gfx_SetTextBGColor(text_BG_color);
	gfx_SetTextTransparentColor(text_BG_color);
		
	// Erase all visible input and redraw it
	gfx_SetColor(text_BG_color);
	gfx_FillRectangle(xPos, yPos, visible_input_line_width, 9);
	gfx_SetTextXY(xPos + 1, yPos + 1);
	
	cursor_xPos = xPos;
	temp1 = visible_buffer;
	i = 0;
	while (*temp1 != '\0') {
		i += gfx_GetCharWidth(*temp1);
		if (i > visible_input_line_width)
			break;
		gfx_PrintChar(*temp1++);
		if (temp1 == curr_char)
			cursor_xPos += i;
	};
	
	if (gfx_GetDraw())
		gfx_BlitRectangle(1, xPos, yPos, visible_input_line_width, 9);

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
			if (gfx_GetDraw())
				gfx_BlitRectangle(1, xPos, yPos, visible_input_line_width, 9);
		};
			
		i = 0;
			
			do {
			key = os_GetCSC();
		} while (!key && i++ < 4000);
	
		if (cursor_active) {
			gfx_SetColor(text_BG_color);
			gfx_Line(cursor_xPos, yPos, cursor_xPos, yPos + 8);
			if (gfx_GetDraw())
				gfx_BlitRectangle(1, xPos, yPos, visible_input_line_width, 9);
		};
		
		while (!key && i++ < 8000)
			key = os_GetCSC();;
		
	} while (!key);
	
	/* Exit if the cursor is at the start of the buffer and there are no chars 
	   in front of the cursor */
	/*
	if (key == sk_Clear && curr_char == buffer && *curr_char == '\0') {
		va_end(keymap_list);
		return;
	};
		if (key == sk_Enter) {
		// Debugging code omitted
		va_end(keymap_list);
		return;
	};
	*/
	
	if (key == sk_Clear) {
		if (curr_char == buffer) {
			temp1 = curr_char;
			while (*temp1 != '\0')
				*temp1++ = '\0';
		} else {
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
		
		curr_keymap = va_arg(keymap_list, const char*);
		curr_indicator = va_arg(keymap_list, const char*);
		curr_keymap_num++;
		
		gfx_SetTextBGColor(0x00);
		gfx_SetTextFGColor(0xFF);
		gfx_SetTextTransparentColor(0x00);
		gfx_SetColor(0x00);
		
		gfx_FillRectangle(cursor_xPos, yPos, 9, 9);
		gfx_PrintStringXY(curr_indicator, cursor_xPos + 1, yPos + 1);
		if (gfx_GetDraw())
			gfx_BlitRectangle(1, xPos, yPos, visible_input_line_width, 9);
		
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
			if (cursor_xPos - xPos + gfx_GetCharWidth(*curr_char++) > visible_input_line_width)
				visible_buffer++;
		};
	} else {
		// Debugging code
		dbg_sprintf(dbgout, "No char added\n");
	};
	
	input_config.curr_keymap_num = curr_keymap_num;
	input_config.curr_char = curr_char;
	input_config.visible_buffer = visible_buffer;
	va_end(keymap_list);
	return key;
}

uint8_t textio_AlphaInput(char *buffer, uint8_t buffer_size, uint24_t visible_input_line_width, uint24_t xPos, uint8_t yPos) {
	uint8_t key;
	textio_SetPrgmNameFlag(false);
	key = textio_Input(buffer, buffer_size, visible_input_line_width, xPos, yPos, 2, uppercase_letters, "A", lowercase_letters, "a");
	return key;
}

void textio_NumericalInput(char *buffer, uint8_t buffer_size, uint24_t visible_input_line_width, uint24_t xPos, uint8_t yPos) {
	textio_SetPrgmNameFlag(false);
	textio_Input(buffer, buffer_size, visible_input_line_width, xPos, yPos, 2, numerals, "1");
}

void textio_AlphaNumInput(char *buffer, uint8_t buffer_size, uint24_t visible_input_line_width, uint24_t xPos, uint8_t yPos) {
	textio_SetPrgmNameFlag(false);
	textio_Input(buffer, buffer_size, visible_input_line_width, xPos, yPos, 6, uppercase_letters, "A", lowercase_letters, "a", numerals, "1");
}

void textio_PrgmNameInput(char *buffer, uint24_t visible_input_line_width, uint24_t xPos, uint8_t yPos) {
	textio_SetPrgmNameFlag(true);
	textio_Input(buffer, 8, visible_input_line_width, xPos, yPos, 4, prgm_letters, "A", numerals, "1");
}