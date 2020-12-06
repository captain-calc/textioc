#include <graphx.h>
#include <tice.h>
#include <textioc.h>

#include <string.h>

// Debugging
#include <stdio.h>
#define dbgout ((char*)0xFB0000)
#define dbgerr ((char*)0xFC0000)
#define dbg_sprintf sprintf

void setup_tios_textio(void) {

	/* Setup the TIOS wrapper. */
	textio_library_routines_t routines = TEXTIO_TIOS_ROUTINES;

	/* Pass the wrapper pointers to TextIOC. */
	textio_SetLibraryRoutines(&routines);
	return;
}

void print(char *text)
{
	char buffer[27] = {'\0'};
	char *curr_line, *next_line;
	int i;
	
	curr_line = text;
	os_ClrHome();

	for (;;)
	{
		next_line = textio_GetLinePtr(curr_line, 1 , 26);
		dbg_sprintf(dbgout, "curr_line = 0x%6x | next_line = 0x%6x\n", curr_line, next_line);
		if (next_line == curr_line)
		{
			return;
		};
		for (i = 0; i < 27; i++)
		{
			if (i < next_line - curr_line)
			{
				buffer[i] = *(curr_line + i);
			} else {
				buffer[i] = '\0';
			};
		};
		dbg_sprintf(dbgout, "buffer = %s\n", buffer);
		os_PutStrFull(buffer);

		// If the buffer is 26 chars long, the newline will create a blank line.
		if (strlen(buffer) < 26)
		{
			os_NewLine();
		};
		curr_line = next_line;
	};
}

void main(void)
{
	
	char text[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ABCDEFGHIJKLMNOPQRSTUVWXYZLorem";
	/* Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. "; */
	
	/* Setup source library. */
	setup_tios_textio();
	
	/* Set print format to left-margin flush. */
	textio_SetPrintFormat(TEXTIOC_FORMAT_LEFT_MARGIN_FLUSH);
	
	textio_SetTabWidth(1);
	
	print(text);
	
	/* Wait for keypress */
	while (!os_GetCSC());
	
	exit(0);
}