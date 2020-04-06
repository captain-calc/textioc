#ifndef TEXTIOC_H
#define TEXTIOC_H

#include <stdint.h>
#include <string.h>

// Output Functions ---------------------------------------
void textio_PrintWrappedText(char *text, uint24_t width, uint24_t xPos, uint8_t yPos);
/* maxWidth is in pixels */
void textio_PrintTruncatedStringXY(char string[], uint24_t max_width, uint24_t xPos, uint8_t yPos);

// Input Functions -----------------------------------------
void textio_SetInputColorConfig(uint8_t text_FG_color, uint8_t cursor_color);
void textio_SetPrgmNameFlag(bool prgm_name_flag);
void textio_LetterInput(char *buffer, uint8_t buffer_size, uint24_t visible_input_width, uint24_t xPos, uint8_t yPos);
void textio_NumericalInput(char *buffer, uint8_t buffer_size, uint24_t visible_input_width, uint24_t xPos, uint8_t yPos);
void textio_LetterNumInput(char *buffer, uint8_t buffer_size, uint24_t visible_input_width, uint24_t xPos, uint8_t yPos);
void textio_PrgmNameInput(char *buffer, uint24_t visible_input_width, uint24_t xPos, uint8_t yPos);
void textio_CustomInput(char *buffer, uint8_t buffer_size, uint24_t view_width, uint24_t xPos, uint8_t yPos, uint8_t total_keymap_num, ...);

#endif