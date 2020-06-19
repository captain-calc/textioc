;-------------------------------------------------------------
; Includes
;-------------------------------------------------------------
	include 'include/library.inc'
	include 'include/include_library.inc'



;-------------------------------------------------------------
	library 'TEXTIOC', 1
;-------------------------------------------------------------



;-------------------------------------------------------------
; Dependencies
;-------------------------------------------------------------
	include_library 'graphx.asm'
	include_library 'fontlibc.asm'



;-------------------------------------------------------------
; v1 functions
;-------------------------------------------------------------
	export textio_SetPrintFormat
	export textio_GetPrintFormat
	export textio_SetTabSize
	export textio_GetTabSize
	export textio_PrintTab
	export textio_PrintChar
	export textio_GetCharWidth
	export textio_GetLineWidth
	export textio_PrintText
	export textio_GetLinePtr
	



;-------------------------------------------------------------
; Global variables
;-------------------------------------------------------------
	arg0	:= 3
	arg1	:= 6
	local0	:= -3
	null	:= $00
	tab	:= $09
	space	:= $20
	bPrintLeftMarginFlush	:= 1
	bPrintCentered	:= 2
	bPrintRightMarginFlush	:= 3



macro mOpenDebugger
	push	hl
	scf
	sbc	hl,hl
	ld	(hl),2
	pop	hl
end macro

macro mGetWordWidth
; Arguments:
;   HL = pointer to word
; Returns:
;   HL = word width
;   Pointer to word on top of stack
; Preserves BC and DE
	push	hl
	push	bc
	push	de
	push	de
	call	fontlib_GetStringWidth	; Destroys BC, DE, and HL
	pop	de
	pop	de
	pop	bc
end macro


;-------------------------------------------------------------
; Functions
;-------------------------------------------------------------


textio_SetPrintFormat:
; Arguments:
;   arg0 = format code
; Returns:
;   A = 0 if invalid format code passed; A > 0 otherwise
; Destroys:
;   A
;   DE
;   HL

	ld	hl, arg0
	add	hl, sp
	ld	a, bPrintRightMarginFlush
	sub	a, (hl)
	jr	c, .invalidValue
	ld	a, 0
	cp	a, (hl)
	jr	z, .invalidValue
	ld	a, (hl)
	ld	(_PrintFormat), a
	ret

.invalidValue:
	xor	a, a
	ret


;-------------------------------------------------------------
textio_GetPrintFormat:
; Arguments:
;   None
; Returns:
;   Current print format code
; Destroys:
;   None

	ld	a, (_PrintFormat)
	ret
	

;-------------------------------------------------------------
textio_SetTabSize:
; Arguments:
;   arg0 = width
; Returns:
;   A = 0 if width supplied was zero
;   A != 0 if valid value (undefined behavior will result if
;   tab was set to a value above 255)
; Destroys:
;   HL

	ld	hl, arg0
	add	hl, sp
	ld	a, (hl)
	cp	a, null	; NULL equals zero
	jr	z, .exit
	ld	a, (hl)
	ld	(_TabWidth), a
	inc	a
.exit:
	ret


;-------------------------------------------------------------
textio_GetTabSize:
; Arguments:
;   None
; Returns:
;   Width of tab
; Destroys:
;   All working registers

	ld	a, (_TabWidth)
	ret


textio_PrintChar:
; Arguments:
;   arg0 = char
; Returns:
;   None
; Destroys:
;   All registers and possibly shadow registers as well

	ld	hl, arg0
	add	hl, sp
	ld	a, tab
	cp	a, (hl)
	jr	z, .printTab
	ld	hl, (hl)
	push	hl
	call	fontlib_DrawGlyph
	pop	hl
	ret
.printTab:
	jr	textio_PrintTab


;-------------------------------------------------------------
textio_PrintTab:
; Arguments:
;   None
; Returns:
;   None
; Destroys:
;   BC and HL

	call	textio_GetTabSize
	ld	b, a
	ld	hl, space
	
.loop:
	push	bc
	push	hl
	call	fontlib_DrawGlyph
	pop	hl
	pop	bc
	djnz	.loop
	ret


textio_GetCharWidth:
; Arguments:
;   arg0 = char
; Returns:
;   Width of char
;   C and A = 0 if invalid codepoint; NC if valid
; Destroys:
;   A, B, DE, and HL

	ld	hl, arg0
	add	hl, sp
	ld	b, (hl)
	ld	a, tab
	cp	a, b
	jr	z, .getTabWidth
	or	a, a
	sbc	hl, hl
	ld	l, b
	push	hl
	call	fontlib_GetGlyphWidth
	pop	hl
	ret
.getTabWidth:
	or	a, a
	sbc	hl, hl
	push	hl
	ld	l, $20
	push	hl
	call	fontlib_GetGlyphWidth
	pop	hl
	pop	hl
	ld	l, a
	call	textio_GetTabSize
	ld	h, a
	mlt	hl
	ld	a, l ; This only returns L, so any tab_width * space_width that
	ret	; is greater than 255 will not behave as expected


;-------------------------------------------------------------
textio_GetLineWidth:
; IMPORTANT: Apply textio_GetCharWidth here
; Arguments:
;   arg0  =  pointer to line
;   arg1  =  pointer to end of line
; Returns:
;   Width of line
; Destroys:
;   All working registers and iy
	
	or	a, a
	sbc	hl, hl
	ex	hl, de
	
	ld	iy, 0
	add	iy, sp
	ld	hl, (iy + arg0)	; hl -> line
	ld	bc, (iy + arg1) ; bc -> eol

.loop:
	push	hl
	or	a, a
	sbc	hl, bc
	pop	hl
	jr	z, .exit
	
	call	fontlib_GetNewlineCode
	cp	a, (hl)
	jr	z, .exit
	
	ld	a, (hl)
	cp	a, null
	jr	z, .exit
	
	push	bc
	push	de
	push	hl
	or	a, a
	sbc	hl, hl
	ex	de, hl
	pop	hl
	ld	e, (hl)
	push	hl
	push	de
	call	textio_GetCharWidth
	pop	hl
	pop	hl
	pop	de
	pop	bc
	call	util.AddAToDE

.nextChar:
	inc	hl		; HL should be pointer to line
	jr	.loop

.exit:
	ex	hl, de
	ret


;-------------------------------------------------------------
textio_PrintText:
; Arguments:
;   arg0 = pointer to text
;   arg1 = yPos
; Returns:
;   None
; Destroys:
;   All registers and iy

; Set first printable char to $20
	or	a, a
	sbc	hl, hl
	ld	l, space
	push	hl
	call	fontlib_SetFirstPrintableCodePoint
	pop	hl
; Load text pointer into HL
	ld	hl, arg0
	add	hl, sp
	ld	hl, (hl)
; Set the desired yPos
	push	hl
	or	a, a
	sbc	hl, hl
	ex	de, hl
	ld	hl, 9
	add	hl, sp
	ld	de, (hl)
	call	fontlib_GetWindowXMin
	push	de
	push	hl
	call	fontlib_SetCursorPosition
	pop	hl
	pop	hl
	pop	hl

.outerLoop:
	; Verify that the char that HL points to is not NULL
	ld	a, null
	cp	a, (hl)
	ret z
	
	; Get the next line pointer
	push	hl
	or	a, a
	sbc	hl, hl
	ex	de, hl
	ld	e, 1
	pop	hl
	push	de
	push	hl
	call	textio_GetLinePtr
	; Set alternate stop code to NULL
	push	hl
	or	a, a
	sbc	hl, hl
	push	hl
	call	fontlib_SetAlternateStopCode
	pop	hl
	pop	hl
	pop	de	; DE holds pointer to start of current line
	pop	bc
	ex	de, hl
	push	de
	push	hl
	
	; If we are printing centered or RMF, get the line width
	; and set the cursor's xPos accordingly.
	ld	a, (_PrintFormat)
	cp	a, bPrintLeftMarginFlush
	jr	nz, .testForCentered
	or	a, a
	sbc	hl, hl
	call	fontlib_GetCursorY
	ld	l, a
	push	hl
	call	fontlib_GetWindowXMin
	push	hl
	call	fontlib_SetCursorPosition
	pop	hl
	pop	hl
	jr	.finishTest
	
.testForCentered:
	cp	a, bPrintCentered
	jr	nz, .testForPrintRMF
	call	textio_GetLineWidth
	call	util.ZeroBC
	ld	c, 2
	call	util.DivideHLBC
	push	hl
	call	fontlib_GetWindowWidth
	call	util.ZeroBC
	ld	c, 2
	call	util.DivideHLBC
	pop	de
	or	a, a
	sbc	hl, de
	push	hl
	call	fontlib_GetWindowXMin
	pop	de
	add	hl, de
	push	hl
	or	a, a
	sbc	hl, hl
	call	fontlib_GetCursorY
	ld	l, a
	pop	de
	push	hl
	push	de
	call	fontlib_SetCursorPosition
	pop	hl
	pop	hl
	jr	.finishTest
	
.testForPrintRMF:
	cp	a, bPrintRightMarginFlush
	jr	nz, .finishTest
	call	textio_GetLineWidth
	ex	de, hl
	call	fontlib_GetWindowWidth
	or	a, a
	sbc	hl, de
	ex	de, hl
	call	fontlib_GetWindowXMin
	add	hl, de
	push	hl
	or	a, a
	sbc	hl, hl
	call	fontlib_GetCursorY
	ld	l, a
	pop	de
	push	hl
	push	de
	call	fontlib_SetCursorPosition
	pop	hl
	pop	hl
	
.finishTest:
	pop	hl
	pop	de
	
.innerLoop:
	; If current char != printable glyph, jump to getNextLine
	call	fontlib_GetNewlineCode
	cp	a, (hl)
	jr	z, .testForNextLine
	push	de
	push	hl
	or	a, a
	sbc	hl, hl
	ex	de, hl
	pop	hl
	ld	e, (hl)
	push	hl
	push	de
	call	textio_PrintChar
	pop	hl
	pop	hl
	pop	de
	; If current char pointer == next line pointer, jump to outerLoop
.testForNextLine:
	inc	hl
	push	de
	ex	de, hl
	or	a, a
	sbc	hl, de
	ex	de, hl
	pop	de
	jr	nz, .innerLoop
	push	de
	call	fontlib_Newline
	pop	hl
	jp	.outerLoop
	
	
;-------------------------------------------------------------
textio_GetLinePtr:
; Arguments:
;   arg0 = text
;   arg1 = yPos
; Returns:
;   Pointer to next line; NULL if error
; Destroys:
;   

	; Set line width to zero
	or	a, a
	sbc	hl, hl
	ld	(_LineWidth), hl

	; Set first printable character and alternate stop code to 0x20 (space)
	ld	l, space
	push	hl
	call	fontlib_SetFirstPrintableCodePoint
	call	fontlib_SetAlternateStopCode
	pop	hl

	; Set the current line number to zero
	ld	a, 0
	ld	(_CurrLineNum), a
	; Load the desired line number into _LineNum
	ld	hl, arg1
	add	hl, sp
	ld	a, (hl)
	ld	(_LineNum), a

	; Load the current char pointer and the current line pointer
	ld	iy, 0
	add	iy, sp
	ld	hl, (iy + arg0)
	ld	bc, (iy + arg0)
	push	bc

.outerLoop:
	; If current line number == desired line number, return line pointer
	pop	bc
	push	hl
	ld	hl, _CurrLineNum
	ld	a, (_LineNum)
	cp	a, (hl)
	pop	hl
	ret	z

	; If current char pointer == null, return line pointer
	ld	a, null
	cp	a, (hl)
	ret	z

	push	bc	; Save pointer to start or line
	call	util.GetApproximateLinePtr
	pop	de
	push	de
	cp	a, null
	jr	z, .testForPrintFormat	; This is necessary since DE must be on the stack
	push	hl
	pop	bc	; BC = approximated pointer
.innerLoop:
	call	fontlib_GetNewlineCode
	cp	a, (hl)
	jr	z, .testForPrintFormat

	push	hl
	or	a, a
	sbc	hl, de
	pop	hl
	jr	z, .breakString
	
	dec	hl
	ld	a, $20
	cp	a, (hl)
	jr	c, .innerLoop
;	push	de
;	push	hl	; If after the first DEC HL, (HL) is a space, this will force
;	ex	de, hl
;	push	bc
;	pop	hl
;	xor	a, a	; the algorithm to continue decrementing until the start of the
;	sbc	hl, de	; word before the space.
;	ld	a, l
;	cp	a, 1
;	pop	hl
;	pop	de
;	jr	z, .innerLoop
	jr	.testForPrintFormat

.breakString:
	push	hl
	or	a, a
	sbc	hl, hl
	ld	(_LineWidth), hl
	pop	hl
	call	util.breakString
.testForPrintFormat:
	ld	a, (_PrintFormat)
	cp	a, bPrintLeftMarginFlush
	jr	z, .incHL
	cp	a, bPrintCentered
	jr	z, .incHL
	call	fontlib_GetNewlineCode
	cp	a, (hl)
	jr	z, .incHL
	jr	.startNewLine
.incHL:
	; Do not increment HL if it points to a NULL
	pop	bc
	ld	a, null
	cp	a, (hl)
	ret	z
	push	bc
	inc	hl
.startNewLine:
	pop	bc
	; If the newline is at the start of a line and we are printing RMF, increment HL so that the next
	; line will be (address of newline) + 1.
	call	util.startNewLine	; This expects BC to be the line pointer
	push	bc
	jr	.outerLoop


;-------------------------------------------------------------
util.GetApproximateLinePtr:
; Arguments:
;   HL = pointer to start of line
; Returns:
;   HL = approximated pointer to next line
;   A = 0 if function found a NULL
; Destroys:
;   All working registers

	; Save pointer to start of line
	push	hl
.loop:
	pop	de
	push	hl
	push	de
	call	textio_GetLineWidth
	ex	de, hl
	pop	bc	; BC = pointer to start of line
	pop	hl	; HL = pointer to current character
	push	bc
	push	hl
	call	fontlib_GetWindowWidth
	or	a, a
	sbc	hl, de
	pop	hl	; HL = pointer to current character
	pop	de	; DE = pointer to start of line
	ret	c

	call	fontlib_GetNewlineCode
	cp	a, (hl)
	ret	z

	ld	a, null
	cp	a, (hl)
	ret	z

	call	util.ZeroBC
	ld	c, (hl)
	push	de
	push	hl
	push	bc
	call	textio_GetCharWidth
	pop	bc
	push	af
	or	a, a
	sbc	hl, hl
	ex	de, hl
	pop	af
	ld	hl, (_LineWidth)
	ld	e, a
	add	hl, de
	ld	(_LineWidth), hl
	pop	hl
	inc	hl
	jr	.loop


;-------------------------------------------------------------
util.breakString:
; Arguments:
;   HL = pointer to start of string
; Returns:
;   Pointer to the remainder of the string
; Destroys:
;   A, BC, DE, and HL

; General register values:
; BC = number of chars in string
; DE = width of string
; HL = pointer to current char in string being processed


; Set B to zero
	ld	b, 0
	
.loop:
; Determine if the current char is a NULL
	push	hl
	ld	a, null
	cp	a, (hl)
	pop	hl
	ret	z
; Determine if the current char is a tab or glyph
; If printing RMF, spaces and tabs are appended to the start of
; the string
	ld	a, tab
	cp	a, (hl)
	jr	nz, .getGlyphWidth
	push	de
	push	hl
	or	a, a
	sbc	hl, hl
	ld	l, tab
	push	hl
	call	textio_GetCharWidth
	pop	hl
	pop	hl
	pop	de
	jr	.addToStrWidth
.getGlyphWidth:
	push	hl
	or	a, a
	sbc	hl, hl
	ex	de, hl
	pop	hl
	ld	e, (hl)
	push	hl
	push	de
	call	fontlib_GetGlyphWidth
	pop	de
	pop	hl
.addToStrWidth:
; Add tab or glyph width to string width (DE)
	push	hl
	push	af
	or	a, a
	sbc	hl, hl
	pop	af
	ld	de, (_LineWidth)
	ld	l, a
	add	hl, de
	ex	de, hl
; If line width + fragment width < window width, jump to loop
	push	bc
	call	fontlib_GetWindowWidth	; Destroys BC | Returns HL
	or	a, a
	sbc	hl, de
	pop	bc
	pop	hl
	ret	c
; Since char was successfully added, add its width to the line width
	ld	(_LineWidth), de
; Increment current char pointer and current char number
	inc	hl
	inc	b
	jr	.loop

	
;-------------------------------------------------------------
util.startNewLine:
; Arguments:
;   HL = pointer to current character being processed in the line
; Returns:
;   Adds one to _CurrLineNum
;   Sets BC equal to HL
;   Zeros the _LineWidth

; Increment the current line number
	ld	a, (_CurrLineNum)
	inc	a
	ld	(_CurrLineNum), a
; Set the current line pointer to the current char pointer
	ex	de, hl
	xor	a, a
	sbc	hl, hl
	ld	(_LineWidth), hl
	ex	de, hl
	push	hl
	pop	bc
	ret
	



;-------------------------------------------------------------
; Internal Library Functions
;-------------------------------------------------------------

util.AddAToDE:
; Inputs:
;   A  = Operand 1
;   HL = Operand 2
; Ouputs:
;   DE = DE + A

	add	a, e
	ld	e, a
	jr	nc, .exit
	inc	d
.exit:
	ret


;--------------------------------------------------------------
util.CompareLastCharRead:
; Preserves HL
; Arguments:
;   A = character to compare
; Returns:
;   NZ if no match
;   Z  if match

	push	hl
	call	fontlib_GetLastCharacterRead
	cp	a, (hl)
	pop	hl
	ret


;-------------------------------------------------------------
util.AddHLToDE:
; Inputs:
;   HL = Operand 1
;   DE = Operand 2
; Outputs:
;   DE = HL + DE

	push	hl
	add	hl, de
	ex	hl, de
	pop	hl
	ret
	
	
;-------------------------------------------------------------
util.ZeroBC:
; Inputs:
;   None
; Outputs:
;   BC = 0
; Destroys:
;   A

	push	hl
	or	a, a
	sbc	hl, hl
	push	hl
	pop	bc
	pop	hl
	ret


;-------------------------------------------------------------
util.DivideHLBC:
; Performs signed integer division
; Designed by the CE Programming SDK Team. Copied from graphx.asm
; URL:
; https://github.com/CE-Programming/toolchain/blob/master/src/graphx/graphx.asm
; Inputs:
;  HL : Operand 1
;  BC : Operand 2
; Outputs:
;  HL = HL/BC

	ex	de,hl
	xor	a,a
	sbc	hl,hl
	sbc	hl,bc
	jp	p,.next0
	push	hl
	pop	bc
	inc	a
.next0:
	or	a,a
	sbc	hl,hl
	sbc	hl,de
	jp	m,.next1
	ex	de,hl
	inc	a
.next1:
	add	hl,de
	rra
	ld	a,24
.loop:
	ex	de,hl
	adc	hl,hl
	ex	de,hl
	adc	hl,hl
	add	hl,bc
	jr	c,.spill
	sbc	hl,bc
.spill:
	dec	a
	jr	nz,.loop

	ex	de,hl
	adc	hl,hl
	ret	c
	ex	de,hl
	sbc	hl,hl
	sbc	hl,de
	ret



;-------------------------------------------------------------
; Internal Library Data
;-------------------------------------------------------------

_TabWidth:
	db	4
_PrintFormat:
	db	1
; Data for textio_GetLinePtr
_LineWidth:
	dl	0
_CurrLineNum:
	db	0
_LineNum:
	db	0