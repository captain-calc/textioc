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
	uint24_t width;
	
	setup_tios_textio();
	
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
	 * Delete the first two characters in the buffer.
	*/
	width = textio_ShiftDeleteString(buffer, buffer_size, buffer, 2);
	dbg_sprintf(dbgout, "Buffer = ");
	autotest_prints(buffer, 10);
	dbg_sprintf(dbgout, "\n");
	if (!autotest_streq("CD\0\0\0\0\0\0\0\0", buffer, 10) || width != 2)
	{
		dbg_sprintf(dbgout, "Test: 02: textio_ShiftDeleteString failed.\n");
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
	 * Attempt to delete four characters starting at the 7th character in the buffer.
	*/
	width = textio_ShiftDeleteString(buffer, buffer_size, buffer + 6, 4);
	dbg_sprintf(dbgout, "Buffer = ");
	autotest_prints(buffer, 10);
	dbg_sprintf(dbgout, "\n");
	if (!autotest_streq("ABCDEFGHI\0", buffer, 10) || width != 0)
	{
		dbg_sprintf(dbgout, "Test: 04: textio_ShiftDeleteString failed.\n");
		return 1;
	};
	
	/**
	 * Test: 05
	 * Delete four characters starting at the 2nd character in the buffer.
	*/
	width = textio_ShiftDeleteString(buffer, buffer_size, buffer + 1, 4);
	dbg_sprintf(dbgout, "Buffer = ");
	autotest_prints(buffer, 10);
	dbg_sprintf(dbgout, "\n");
	if (!autotest_streq("AFGHI\0\0\0\0\0", buffer, 10) || width != 4)
	{
		dbg_sprintf(dbgout, "Test: 05: textio_ShiftDeleteString failed.\n");
		return 1;
	};
	
	dbg_sprintf(dbgout, "Autotest was completed successfully.\n");
    return 0;
}