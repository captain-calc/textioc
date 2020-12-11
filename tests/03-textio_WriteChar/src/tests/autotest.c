#include <stdint.h>
#include <stdio.h>
#include "autotest.h"

#define dbgout ((char*)0xFB0000)
#define dbgerr ((char*)0xFC0000)
#define dbg_sprintf sprintf

bool autotest_streq(const char *str1, const char *str2, uint24_t length)
{
	uint24_t i;
	for (i = 0; i < length; i++)
	{
		if (*(str1 + i) != *(str2 + i))
		{
			return false;
		};
	};
	return true;
}

void autotest_prints(const char *string, uint24_t length)
{
	uint24_t i;
	for (i = 0; i < length; i++)
	{
		if (*(string + i) != '\0')
		{
			dbg_sprintf(dbgout, "%c", *(string + i));
		} else {
			dbg_sprintf(dbgout, ".");
		};
	};
	return;
}