#include <keypadc.h>
#include <graphx.h>
#include <tice.h>
#include <textioc.h>

#include <string.h>

// Debugging
#include <stdio.h>
#include "tests/autotest.h"
#define dbgout ((char*)0xFB0000)
#define dbgerr ((char*)0xFC0000)
#define dbg_sprintf sprintf

void setup_tios_textio(void)
{
	/* Setup the TI-OS wrapper. */
	textio_library_routines_t routines = TEXTIO_TIOS_ROUTINES;

	/* Pass the wrapper pointers to TextIOC. */
	textio_SetLibraryRoutines(&routines);
	return;
}

int main(void)
{
	/* Setup the default TextIOC output data for the text output functions. */
	textio_output_data_t data = TEXTIO_TIOS_OUTPUT_DATA;
	textio_output_data_t *output_data = &data;
	
	setup_tios_textio();
	
	dbg_sprintf(dbgout, "==========================================\n");
	dbg_sprintf(dbgout, "Start of autotest\n");
	
	/**
	 * Test: 01
	 * Get the width of one character using the homescreen char width routine.
	 */
	if (textio_GetCharWidth(output_data, 'A') != 1)
	{
		dbg_sprintf(dbgout, "Test: 01: textio_GetCharWidth failed.\n");
		return 1;
	};
	
	/**
	 * Test: 02
	 * Get the width of four characters using the homescreen char width routine.
	 */
	if (textio_GetStringWidthL(output_data, "ABCD", 4) != 4)
	{
		dbg_sprintf(dbgout, "Test: 02: textio_GetStringWidthL failed.\n");
		return 1;
	};
	
	dbg_sprintf(dbgout, "Autotest was completed successfully.\n");
    return 0;
}