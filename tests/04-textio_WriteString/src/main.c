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
	/* Setup the GraphX wrapper. */
	textio_library_routines_t routines = TEXTIO_TIOS_ROUTINES;

	/* Pass the wrapper pointers to TextIOC. */
	textio_SetLibraryRoutines(&routines);
	return;
}

int main(void)
{
	char buffer[10] = {'\0'};
	
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
	 * Write a string at the start of the buffer.
	*/
	textio_WriteString(buffer, 7, "ZYXWVUT");
	dbg_sprintf(dbgout, "Buffer = ");
	autotest_prints(buffer, 10);
	dbg_sprintf(dbgout, "\n");
	if (!autotest_streq("ZYXWVUT\0\0\0", buffer, 10))
	{
		dbg_sprintf(dbgout, "Test: 02: textio_WriteString failed.\n");
		return 1;
	};
	
	dbg_sprintf(dbgout, "Autotest was completed successfully.\n");
    return 0;
}