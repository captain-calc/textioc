#ifndef AUTOTEST_H
#define AUTOTEST_H

#include <stdbool.h>

/**
 * Description:
 *   Compare two substrings of length LENGTH from STR1 and STR2,
 *   including any NULL characters.
 * Arguments:
 *   str1 = first string
 *   str2 = second string
 * Returns:
 *   True if substrings match; false, otherwise
*/
bool autotest_streq(const char *str1, const char *str2, uint24_t length);

/**
 * Description:
 *   Prints LENGTH characters of STRING. NULL characters are printed as "."
 * Arguments:
 *   string = Pointer to string
 *   length = Number of characters to print
 * Returns:
 *   None
*/
void autotest_prints(const char *string, uint24_t length);

#endif