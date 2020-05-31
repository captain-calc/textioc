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
uint24_t textio_GetLineWidth(const char *line, const char *eol);


#ifdef __cplusplus
}
#endif

#endif