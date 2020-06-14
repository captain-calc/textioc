#ifndef H_TEXTIOC
#define H_TEXTIOC

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool textio_SetPrintFormat(uint8_t format);
uint8_t textio_GetPrintFormat(void);
bool textio_SetTabWidth(uint8_t width);
uint8_t textio_GetTabWidth(void);
void textio_PrintTab(void);
uint8_t textio_GetCharWidth(char codepoint);
uint24_t textio_GetLineWidth(const char *line, const char *eol);
void textio_PrintText(const char *text, uint8_t yPos);
char *textio_GetLinePtr(const char *text, uint8_t line_num);


#ifdef __cplusplus
}
#endif

#endif