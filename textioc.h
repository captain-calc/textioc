#ifndef TEXTIOC_H
#define TEXTIOC_H

#include <stdint.h>
#include <string.h>


/* Print wrapped text to the screen. Text wraps at width pixels.
 */
void textio_PrintWrappedText(char *text, uint24_t width, uint24_t xPos, uint8_t yPos);


/*Print a string to the screen up until max_width pixels.
 */
void textio_PrintTruncatedStringXY(char string[], uint24_t max_width, uint24_t xPos, uint8_t yPos);


/* Set configurations for textio_LetterInput.
 * Returns old colors in the lower two bytes of the returned integer.
 */
unsigned int textio_SetInputConfig(uint8_t text_FG_color, uint8_t cursor_color);


/* Set configurations for textio_PrintWrappedText and textio_PrintTruncatedStringXY.
 * Returns old config in the lower two bytes of the returned integer.
 */
unsigned int textio_SetTextConfig(uint8_t text_y_margin, uint8_t line_spacing);

/* Get string input. Returns pointer to input buffer.
 */
char *textio_LetterInput(char buffer[], uint8_t buffer_size, uint24_t xPos, uint8_t yPos);


#endif
