## TEXTIOC Change Log

### Version 0.1

Comments:

First version. Written in C using the CE SDK by the CE Programming Team.

New Functions:
* textio_PrintWrappedTextXY
* textio_PrintTruncatedStringXY
* textio_LetterInput
* textio_SetInputConfig

### Version 0.2

Comments:

Rewrote textio_LetterInput in order to add automatic text scrolling, optimize text redrawing, and add custom keymaps. Renamed it textio_CustomInput. The textio_PrgmNameInput function does not support the theta character.

New Functions:
* textio_CustomInput
* textio_SetPrgmNameFlag
* textio_LetterInput (new version based on textio_CustomInput)
* textio_NumericalInput
* textio_LetterNumInput
* textio_PrgmNameInput

Function Renaming:
* textio_SetInputConfig -> textio_SetInputColorConfig

Features:
* Text scrolling for the input functions
* Added a flag to textio_CustomInput that forces an input method suitable for a TI-OS program name

### Version 0.3

Comments:

Forcing textio_Input to exit after each keypress will allow the program to immediately evaluate the input that has just been received. This immediate evaluation can be used to create text suggestion functions or to  stop the input function when the user enters a certain amount or type of input.

The changes outlined below for textio_PrintTextXY will make the function more useful for a text editor. It will also aid the creation of a scroll bar function.

New Functions:
* textio_SetLineHeight
* textio_SetTabWidth
* textio_Setup

Function Renaming:
* textio_CustomInput -> textio_Input
* textio_LetterInput -> textio_AlphaInput
* textio_LetterNumInput -> textio_AlphaNumInput
* textio_PrintWrappedText -> textio_PrintTextXY

Features:
* Double-buffering support for all input functions
* Made textio_Input exit after each keypress (this applies to all of the input functions)
* Added tab handling, newline handling, line limit, and inital line number parameters to textio_PrintTextXY

New Internal Library Routines:
* getWordWidth
* iscntrl	(determines if the passed char is a null, a newline, or a tab)


### Version 0.4.0

Comments:

A complete port of the library from C to eZ80 Assembly. Drops GraphX font support and adds FontLibC support.

*textioc.h* contains the syntax for the following functions:

* textio_SetPrintFormat
* textio_GetPrintFormat
* textio_SetTabSize
* textio_GetTabSize
* textio_PrintTab
* textio_PrintChar
* textio_GetCharWidth
* textio_GetLineWidth
* textio_PrintText
* textio_GetLinePtr


### Version 0.4.1

Comments:

Library is stable, except in very extreme or unusual circumstances. Contains a complete rewrite of several functions pertaining to *textio_GetLinePtr*.