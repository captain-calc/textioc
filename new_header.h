/*
typedef struct
{
	char *char_ptr;
	uint24_t xPos;
	uint24_t yPos;
	char *first_visible_char_ptr;
	uint24_t visible_buffer_width;
	void (*cursor_routine)(void);
	uint24_t cursor_x;
	uint24_t cursor_y;
	uint24_t timer;
	char attributes;
	uint8_t num_keymaps;
	uint8_t curr_keymap_num;
	char **keymaps;
} textio_input_data_t;

textio_SetPrgmNameFlag(textio_input_data_t *ptr);
textio_SetLockFlag(textio_input_data_t *ptr);
textio_SetTimerFlag(textio_input_data_t *ptr);
textio_Input(textio_input_data_t *ptr);
*/

uint8_t textio_KeyToOffset(void);
uint8_t textio_CursorRoutine_8bpp(uint24_t xPos, uint24_t yPos, uint24_t cursor_width, uint24_t cursor_height, uint8_t blink_rate);
uint8_t textio_TimedCursorRoutine_8bpp(uint24_t xPos, uint24_t yPos, uint24_t cursor_width, uint24_t cursor_height, uint8_t blink_rate, uint24_timer);
uint8_t textio_CursorRoutine_16bpp(uint8_t col, uint8_t row, uint8_t blink_rate);
uint8_t textio_TimedCursorRoutine_16bpp(uint8_t col, uint8_t row, uint8_t blink_rate, uint24_timer);

bool textio_InsertChar(char *buffer, uint24_t buffer_size, char character, char *location);
bool textio_InsertString(char *buffer, uint24_t buffer_size, char *string, char *location, uint24_t length);
textio_ShiftDeleteChar
textio_ShiftDeleteString


void textio_PutC(char c);
uint24_t textio_GetCharWidth(textio_output_data_t *ptr, char character);
uint24_t textio_GetLineWidth(textio_output_data_t *ptr, char *line, char *eol);
uint24_t textio_GetStringWidthL(textio_output_data_t *ptr, char *string, uint24_t length);
char *textio_GetLinePtr(textio_output_data_t *ptr, char *text, uint24_t line_num);
