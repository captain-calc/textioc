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
	 * Insert a string at the start of the buffer.
	*/
	textio_InsertString(buffer, buffer_size, "XYZ", buffer, 3);
	dbg_sprintf(dbgout, "Buffer = ");
	autotest_prints(buffer, 10);
	dbg_sprintf(dbgout, "\n");
	if (!autotest_streq("XYZABCD\0\0\0", buffer, 10))
	{
		dbg_sprintf(dbgout, "Test: 01: textio_InsertString failed.\n");
		return 1;
	};
	
	/**
	 * Test: 03
	 * Attempt to insert a string that cannot fit at the start of the buffer.
	*/
	return_code = textio_InsertString(buffer, buffer_size, "XYZ", buffer, 3);
	dbg_sprintf(dbgout, "Buffer = ");
	autotest_prints(buffer, 10);
	dbg_sprintf(dbgout, "\n");
	if (!autotest_streq("XYZABCD\0\0\0", buffer, 10) || !return_code)
	{
		dbg_sprintf(dbgout, "Test: 03: textio_InsertString failed.\n");
		return 1;
	};
	
	dbg_sprintf(dbgout, "Autotest was completed successfully.\n");
    return 0;
}