#ifndef TEXTIOC_H
#define TEXTIOC_H

#include <stdint.h>
#include <string.h>

void textio_PrintWrappedText(char *text, uint24_t width, uint24_t xPos, uint8_t yPos);
/* maxWidth is in pixels */
void textio_PrintTruncatedStringXY(char string[], uint24_t max_width, uint24_t xPos, uint8_t yPos);


void textio_SetInputConfig(uint8_t text_FG_color, uint8_t cursor_color);
void textio_LetterInput(char buffer[], uint8_t buffer_size, uint24_t xPos, uint8_t yPos);

#endif