#ifndef TEXTIOC_H
#define TEXTIOC_H

#include <stdint.h>
#include <string.h>


void textio_Setup(void);


// INTERNAL LIBRARY ROUTINES
//============================================================

int iscntrl(int c);
uint8_t getWordWidth(char *word);


// TEXT OUTPUT FUNCTIONS
//============================================================

void textio_PrintTextXY(char *text, uint8_t initial_line_num, uint8_t num_lines, uint24_t xPos, uint8_t yPos, uint24_t width);
void textio_PrintTruncatedStringXY(char string[], uint24_t max_width, uint24_t xPos, uint8_t yPos);
/* max_width is in pixels */

void textio_SetLineHeight(uint8_t pxl_height);
void textio_SetTabWidth(uint8_t tab_height);


// TEXT INPUT FUNCTIONS
//============================================================

uint8_t textio_AlphaInput(char *buffer, uint8_t buffer_size, uint24_t visible_input_width, uint24_t xPos, uint8_t yPos);
void textio_NumericalInput(char *buffer, uint8_t buffer_size, uint24_t visible_input_width, uint24_t xPos, uint8_t yPos);
void textio_AlphaNumInput(char *buffer, uint8_t buffer_size, uint24_t visible_input_width, uint24_t xPos, uint8_t yPos);
void textio_PrgmNameInput(char *buffer, uint24_t visible_input_width, uint24_t xPos, uint8_t yPos);
uint8_t textio_Input(char *buffer, uint8_t buffer_size, uint24_t view_width, uint24_t xPos, uint8_t yPos, uint8_t total_keymap_num, ...);

void textio_SetInputColorConfig(uint8_t text_FG_color, uint8_t cursor_color);
void textio_SetPrgmNameFlag(bool prgm_name_flag);
void textio_SetInitialKeymap(uint8_t keymap);

#endif