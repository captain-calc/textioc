#include <keypadc.h>
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
	char buffer[10] = {'\0'};
	uint24_t buffer_size = 9;
	bool return_code;
	
	dbg_sprintf(dbgout, "==========================================\n");
	dbg_sprintf(dbgout, "Start of autotest\n");
	
	/**
	 * Test: 01
	 * Write initial data to buffer.
	 */
	strcpy(buffer, "ABCD");
	dbg_sprintf(dbgout, "Buffer = ");
	autotest_prints(buffer, 10);
	dbg_sprintf(dbgout, "\n");
	if (!autotest_streq("ABCD\0\0\0\0\0\0", buffer, 10))
	{
		dbg_sprintf(dbgout, "Test: 01: strcpy failed.\n");
		return 1;
	};
	
	/**
	 * Test: 02
	 * Insert a character at the start of the buffer.
	*/
	textio_InsertChar(buffer, buffer_size, 'Z', buffer);
	dbg_sprintf(dbgout, "Buffer = ");
	autotest_prints(buffer, 10);
	dbg_sprintf(dbgout, "\n");
	if (!autotest_streq("ZABCD\0\0\0\0\0", buffer, 10))
	{
		dbg_sprintf(dbgout, "Test: 02: textio_InsertChar failed.\n");
		return 1;
	};
	
	/**
	 * Test: 03
	 * Fill up the buffer.
	*/
	strcpy(buffer, "ABCDEFGHI");
	dbg_sprintf(dbgout, "Buffer = ");
	autotest_prints(buffer, 10);
	dbg_sprintf(dbgout, "\n");
	if (!autotest_streq("ABCDEFGHI\0", buffer, 10))
	{
		dbg_sprintf(dbgout, "Test: 03: strcpy failed.\n");
		return 1;
	};
	
	/**
	 * Test: 04
	 * Attempt to insert a char that will not fit in the buffer.
	*/
	return_code = textio_InsertChar(buffer, buffer_size, 'Y', buffer);
	dbg_sprintf(dbgout, "Buffer = ");
	autotest_prints(buffer, 10);
	dbg_sprintf(dbgout, "\n");
	if (!autotest_streq("ABCDEFGHI\0", buffer, 10) || !return_code)
	{
		dbg_sprintf(dbgout, "Test: 04: textio_InsertChar failed.\n");
		return 1;
	};
	
	dbg_sprintf(dbgout, "Autotest was completed successfully.\n");
    return 0;
}