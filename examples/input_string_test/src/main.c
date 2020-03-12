
//textioc string input test
//beckadamtheinventor

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <graphx.h>
#include <textioc.h>

void main(void){
	char buffer[256] = {0};
	gfx_Begin();
	textio_LetterInput(&buffer,255,310,2,2);
	gfx_FillScreen(255);
	textio_PrintWrappedText(&buffer,310,2,2);
	while (!os_GetCSC());
	gfx_End();
}
