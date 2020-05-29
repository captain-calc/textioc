#include "calcdbg.h"

#include <debug.h>


/* Useful function for dumping the contents of data buffers */

void calcdbg_dump(const char *data_buffer, const int length) {
	char byte;
	unsigned int i, j;

	for (i = 0; i < length; i++) {
		byte = data_buffer[i];
		dbg_sprintf(dbgout, "%2x ", data_buffer[i]);
		if ((i % 16) == 15 || (i == length - 1)) {
			for (j = 0; j < 15 - (i % 16); j++)
				dbg_sprintf(dbgout, "       ");
			dbg_sprintf(dbgout, "| ");
			for (j = (i - (i % 16)); j <= i; j++) {
				byte = data_buffer[j];
				if ((byte > 31) && (byte < 127))
					dbg_sprintf(dbgout, "%c", byte);
				else
					dbg_sprintf(dbgout, ".");
			};
			dbg_sprintf(dbgout, "\n");
		};
	};
	return;
}