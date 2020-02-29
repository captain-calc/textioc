#ifndef TEXTIOC_H
#define TEXTIOC_H

/*
* ========================
* Last edited: 2/22/2020
* ========================
*/

#include <stdint.h>
#include <string.h>

void textio_PrintWrappedText(char text[], uint24_t width, uint8_t xPos, uint8_t yPos);
/* maxWidth is in pixels */
void textio_PrintTruncatedStringXY(char string[], uint24_t max_width, uint24_t xPos, uint8_t yPos);


uint24_t textio_NumericalInput(uint8_t xPos, uint8_t yPos, uint8_t magnitude_limit);
void textio_LetterInput(char buffer[], uint8_t buffer_size, uint24_t xPos, uint8_t yPos);

#endif