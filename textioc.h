/**
 * @file
 * @brief Contains routines for text wrapping and text input.
 *
 *
 * @authors "Captain Calc"
 *
 * Many thanks to DrDnar, jacobly, Adriweb, and the other members of the CE Programming team for their
 * help and support!
 */


#ifndef H_TEXTIOC
#define H_TEXTIOC

#include <graphx.h>
#include <fontlibc.h>
#include <stdint.h>
#include <tice.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Library version
*/
#define LIBRARY_VERSION 1

/**
 * This structure holds the pointers to the external text functions as well as the library version data.
 *
 * @see textio_SetLibraryRoutines
*/
typedef struct {
	uint8_t library_version;
	uint24_t (*get_char_width)(char);
} textio_library_routines_t;

/**
 * Function wrappers for homescreen.
*/

/* On the homescreen, the font is monospaced. */
static uint24_t textio_os_GetGlyphWidth(char codepoint) {
	return 1;
}

/**
 * Function wrappers for FontLibC.
*/

static uint24_t textio_fontlib_GetGlyphWidth(char codepoint) {
	return (uint24_t)fontlib_GetGlyphWidth(codepoint);
}

/**
 * Function wrappers for GraphX.
 */

static uint24_t textio_gfx_GetCharWidth(char codepoint) {
	return (uint24_t)gfx_GetCharWidth((const char)codepoint);
}

/**
 * Default external function pointers for homescreen.
*/
#define TEXTIO_TIOS_ROUTINES { \
				LIBRARY_VERSION, \
				textio_os_GetGlyphWidth \
				};

/**
 * Default external function pointers for FontLibC.
*/
#define TEXTIO_FONTLIB_ROUTINES { \
				LIBRARY_VERSION, \
				textio_fontlib_GetGlyphWidth \
				};

/**
 * Default external function pointers for GraphX.
*/
#define TEXTIO_GRAPHX_ROUTINES { \
				LIBRARY_VERSION, \
				textio_gfx_GetCharWidth \
				};

/**
 * Provides the external text function pointers to TextIOC.
 *
 * @note The recommended method for using this function is shown below:
 * @code
 * textio_library_routines_t routines = TEXTIO_GRAPHX_ROUTINES;
 * // Or, textio_library_routines_t routines = TEXTIO_FONTLIB_ROUTINES;
 * 
 * textio_SetLibraryRoutines(&routines);
 * @endcode
 * @see textio_library_routines_t
 * 
 * @param ptr Pointer to routine structure
*/
void textio_SetLibraryRoutines(textio_library_routines_t *ptr);

/**
 * Writes CHARACTER at LOCATION in BUFFER. If any pre-existing characters in
 * BUFFER block the insertion, they are shifted right.
 *
 * @param buffer Pointer to the text buffer
 * @param buffer_size Size of buffer (not including null terminator)
 * @param character Character to write
 * @param location Pointer to where character should be written
 * @return 0 if character was inserted; 1, if character could not be written
*/
bool textio_InsertChar(const char *buffer, uint24_t buffer_size, const char character, const char *location);

/**
 * Write NUM_CHARS of STRING at LOCATION in BUFFER. If any pre-existing characters in
 * BUFFER block the insertion, they are shifted right.
 *
 * @param buffer Pointer to the text buffer
 * @param buffer_size Size of buffer (not including null terminator)
 * @param string String to write
 * @param location Pointer to where string should be written
 * @param length Number of characters to write
 * @return 0 if string was inserted; 1, if string could not be written
*/
bool textio_InsertString(const char *buffer, uint24_t buffer_size, const char *string, const char *location, uint24_t length);

/**
 * Writes CHARACTER at LOCATION.
 *
 * @note This function is not bounds-checked. The programmer must ensure that buffer
 * overflows are prevented.
 *
 * @param location Pointer to where character should be written
 * @param character Character to write
*/
void textio_WriteChar(const char *location, const char character);

/**
 * Write NUM_CHARS of STRING at LOCATION.
 *
 * @note This function is not bounds-checked. The programmer must ensure that buffer
 * overflows are prevented.
 *
 * @param location Pointer to where string should be written
 * @param length Number of characters to write
 * @param string String to write
*/
void textio_WriteString(const char *location, uint24_t length, const char *string);

/**
 * Deletes LENGTH bytes at STRING in BUFFER. Any non-NULL bytes to the right of the
 * deleted character are shifted left.
 *
 * @param buffer Pointer to buffer
 * @param buffer_size Size of buffer (not including null terminator)
 * @param character Pointer to character
 * @return Width of deleted character
*/
uint24_t textio_ShiftDeleteChar(const char *buffer, uint24_t buffer_size, const char *character);

/**
 * Deletes LENGTH bytes at STRING in BUFFER. Any non-NULL bytes to the right of the
 * deleted string are shifted left.
 *
 * @param buffer Pointer to buffer
 * @param buffer_size Size of buffer (not including null terminator)
 * @param string Pointer to string
 * @param length Number of characters to delete
 * @return Width of deleted string
*/
uint24_t textio_ShiftDeleteString(const char *buffer, uint24_t buffer_size, const char *string, uint24_t length);

/**
 * Deletes the byte at CHARACTER.
 *
 * @param character Pointer to character
*/
void textio_DeleteChar(const char *character);

/**
 * Deletes LENGTH bytes at STRING.
 *
 * @param string Pointer to string
 * @param length Number of characters to delete
*/
void textio_DeleteString(const char *string, uint24_t length);

/**
 * Shifts the null-terminated STRING left by DISTANCE.
 *
 * @param string String to shift
 * @param distance Distance (in bytes) to shift string
*/
void textio_ShiftStringLeft(const char *string, uint24_t distance);

/**
 * Shifts null-terminated STRING right by DISTANCE.
 *
 * @param string String to shift
 * @param distance Distance (in bytes) to shift string
*/
void textio_ShiftStringRight(const char *string, uint24_t distance);

/**
 * Deletes (sets to NULL) num_bytes characters starting at ptr. If OVERWRITE_MODE is
 * off, any characters to the right of the deleted characters in the buffer are
 * shifted left by num_bytes. No shift occurs if OVERWRITE_MODE is on.
 *
 * @param buffer Pointer to text buffer
 * @param num_bytes Number of bytes to delete
*/
uint24_t textio_Clear(char *buffer, char *offset, uint24_t num_bytes, uint24_t buffer_size);

/**
 * Converts a keypress into an offset for accessing characters in a keymap.
 * These keymaps are the same as the example presented in the *tice.h*
 * documentation for os_GetCSC.
 *
 * @return Keymap offset; Returns -1 if an error occured
*/
int8_t textio_KeyToOffset(void);

/**
 * Print formatting options for textio_GetLinePtr.
 *
 * @see textio_SetPrintFormat
*/
typedef enum {
	
	/**
	 * Enables left-margin-flush printing.
	*/
	TEXTIOC_FORMAT_LEFT_MARGIN_FLUSH = 0x01,
	
	/**
	 * Enables centered printing.
	*/
	TEXTIOC_FORMAT_CENTERED = 0x02,
	
	/**
	 * Enables right-margin-flush printing.
	*/
	TEXTIOC_FORMAT_RIGHT_MARGIN_FLUSH = 0x03
	
} textio_line_format_options_t;

/**
 * Sets the print format for textio_GetLinePtr.
 *
 * @see textio_print_format_options_t
 *
 * @param format Format option code
*/
bool textio_SetPrintFormat(uint8_t format);

/**
 * Gets the current print format.
 *
 * @return Current print format code
*/
uint8_t textio_GetPrintFormat(void);

/**
 * Sets the codepoint that will act as the newline character.
 *
 * @param codepoint Codepoint
*/
void textio_SetNewlineCode(char codepoint);

/**
 * Gets the codepoint that is acting as the newline character.
 *
 * @return Codepoint
*/
char textio_GetNewlineCode(void);

/**
 * Sets the number of pixels that make up each tab.
 *
 * @param width Width of tab
*/
void textio_SetTabWidth(uint24_t width);

/**
 * Gets the number of pixels that make up each tab.
 *
 * @return Tab width
*/
uint24_t textio_GetTabWidth(void);

/**
 * Gets the width of the supplied character. This function also works for tabs, and it returns the
 * tab's actual pixel width.
 *
 * @param character Target character
 * @return Width of character
*/
uint24_t textio_GetCharWidth(const char character);

/**
 * Gets the width of the supplied line.
 *
 * @param line Pointer to line
 * @param eol Pointer to the End-Of-Line
 * @return Width of line
*/
uint24_t textio_GetLineWidth(const char *line, char *eol);

/**
 * Gets the width of \p num_chars characters in \c string.
 *
 * @param string Pointer to string
 * @param num_chars Number of characters
 * @return Width of the desired number of characters
*/
uint24_t textio_GetStringWidthL(const char *string, uint24_t num_chars);

/**
 * Returns a pointer to the \p line_num line in \p text.
 *
 * @param text Pointer to text
 * @param line_num Number of line
 * @param max_line_width Maximum width of line (in pixels)
 * @return Pointer to desired line
*/
char *textio_GetLinePtr(const char *text, uint24_t line_num, uint24_t max_line_width);


#ifdef __cplusplus
}
#endif

#endif