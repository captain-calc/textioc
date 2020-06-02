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
	export textio_SetTabWidth
	export textio_GetTabWidth
	export textio_PrintTab
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
	mPrintLeftMarginFlush	:= 1 shl bPrintLeftMarginFlush
	bPrintCentered	:= 2
	mPrintCentered	:= 1 shl bPrintCentered
	bPrintRightMarginFlush	:= 3
	mPrintRightMarginFlush	:= 1 shl bPrintRightMarginFlush



macro mOpenDebugger
	scf
	sbc	hl,hl
	ld	(hl),2
end macro



;-------------------------------------------------------------
; Functions
;-------------------------------------------------------------


textio_SetPrintFormat:
; Arguments:
;   arg0 = format code
; Returns:
;   False if invalid format code passed; true otherwise
; Destroys:
;   A
;   DE
;   HL

	ld	hl, arg0
	add	hl, sp
	ld	a, bPrintRightMarginFlush	; See table of valid format codes
	sub	a, (hl)
	jr	c, .invalidValue
	jr	z, .invalidValue
	ld	a, (hl)
	ld	(_PrintFormat), a
	ld	a, 1
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
textio_SetTabWidth:
; Arguments:
;   arg0 = width
; Returns:
;   Zero if invalid width passed; true otherwise
; Destroys:
;   HL
;   DE

	ld	hl, arg0
	add	hl, sp
	xor	a, a
	cp	a, (hl)
	jr	z, .invalidValue
	ld	a, (hl)
	ld	(_TabWidth), a
	inc	a
	ret
	
.invalidValue:
	ret


;-------------------------------------------------------------
textio_GetTabWidth:
; Arguments:
;   None
; Returns:
;   Width of tab

	ld	a, (_TabWidth)
	ret


;-------------------------------------------------------------
textio_PrintTab:
; Arguments:
;   None
; Returns:
;   None
; Destroys:
;   BC
;   HL

	call	textio_GetTabWidth
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


;-------------------------------------------------------------
textio_GetLineWidth:
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
	sbc	hl, bc
	pop	hl
	jr	z, .exit
	
	ld	a, (hl)
	cp	a, null
	jr	z, .exit
	
	cp	a, tab
	jr	nz, .addGlyphWidth

	call	textio_GetTabWidth	; Add tab width
	call	util.AddAToDE
	jr	.nextChar

.addGlyphWidth:
	push	de
	push	hl
	ld	d, 0
	ld	e, (hl)
	push	de
	call	fontlib_GetGlyphWidth	; Destroys DE and HL
	pop	hl
	pop	hl
	pop	de
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
;   All registers and IY

	mOpenDebugger

	ld	iy, 0
	add	iy, sp
	ld	de, (iy + arg1)
	call	fontlib_GetWindowXMin
	push	hl
	push	de
	call	fontlib_SetCursorPosition
	pop	hl
	pop	hl
	or	a, a
	sbc	hl, hl
	ex	de, hl	; Set DE to zero (NULL)
	push	de
	call	fontlib_SetAlternateStopCode
	pop	de
	sbc	hl, hl
	ld	l, space
	push	hl
	call	fontlib_SetFirstPrintableCodePoint
	pop	hl
	ld	hl, (iy + arg0)

.outerLoop:
	push	hl
	or	a, a
	sbc hl, hl
	ex	de, hl
	ld	e, 1
	pop	hl
	push	de
	push	hl
	call	textio_GetLinePtr
	ex	de, hl	; DE -> next line
	pop	hl
	pop	hl

; Set the proper cursor positions here

.innerLoop:
; If the current character being processed is not a tab
; or a newline, print glyph
	ld	a, tab
	cp	a, (hl)
	jr	.printTab
	call	fontlib_GetNewlineCode
	cp	a, (hl)
	jr	z, .cont1
	push	de
	push	hl
	or	a, a
	sbc	hl, hl
	ex	de, hl
	pop	hl
	ld	e, (hl)
	push	hl
	push	de
	call	fontlib_DrawGlyph
	pop	hl
	pop	hl
	pop	de
.printTab:
	push	hl
	call	textio_PrintTab
	pop	hl
.cont1:
	push	de
	push	hl
	or	a, a
	sbc	hl, de
	pop	hl
	pop	de
	jr	nz, .innerLoop
	jr	z, .startNewLine
	ld	a, null
	cp	a, (hl)
	jr	nz, .innerLoop
	ret
.startNewLine:
	push	hl
	ex	de, hl
	call	fontlib_Newline	; Destroys all registers and iy
	pop	hl
	jr	.outerLoop


;-------------------------------------------------------------
textio_GetLinePtr:
; Arguments:
;   arg0  =  pointer to line
;   arg1  =  line number
; Returns:
;   Pointer to line
; Destroys:
;   All registers and IY

; The desired line number will be on the stack
; as will the current line number
; DE = current line width
; HL -> current word pointer
; BC -> current line pointer

; Set the first printable code point and alternate stop code to 0x20 (space)
	
	or	a, a
	sbc	hl, hl
	ex	de, hl
	sbc	hl, hl
	
	ld	l, space
	push	hl
	call	fontlib_SetFirstPrintableCodePoint
	call	fontlib_SetAlternateStopCode
	pop	hl

; The current line number is zero at the start
.currLineNum := $ - 1
	ld	a, 0
	ld	(.currLineNum), a

	ld	iy, 0
	add	iy, sp
	ld	hl, (iy + arg0)	; HL -> line
	ld	bc, (iy + arg0) ; BC -> line

; Return NULL if NULL string passed
	ld	a, null
	cp	a, (hl)
	jr	nz, .loop
	or	a, a
	sbc	hl, hl
	ret

.loop:
	push	hl
;	mOpenDebugger
; If the current line number is less than the desired line number, jump to cont1
; Else return the current line pointer
	ld	a, (.currLineNum)
	ld	hl, 9	; HL is on the stack as well as the two args
	add	hl, sp
	sub	a, (hl)
	pop	hl
	jr	nz, .cont1
	or	a, a
	sbc	hl, hl
	add	hl, bc
	ret
.cont1:
; Add width of next word
	push	bc
	push	de
	push	hl
	push	hl
	call	fontlib_GetStringWidth	; Destroys BC, DE, and HL
	pop	bc
	pop	bc
	pop	de
	add	hl, de
	ex	de, hl
	push	bc
	pop	hl
	pop	bc

; Add width of space following word
	push	hl
	call	fontlib_GetLastCharacterRead
	ld	a, space
	cp	a, (hl)
	pop	hl
	jr	nz, .doNotAddSpaceWidth
	push	hl
	push	de
	or	a, a
	sbc	hl, hl
	ld	l, space
	push	hl
	call	fontlib_GetGlyphWidth	; Destroys DE and HL
	pop	hl
	pop	de
	pop	hl
	call	util.AddAToDE
.doNotAddSpaceWidth:
; If next word is a tab, add tab width
	push	hl
	call	fontlib_GetLastCharacterRead
	ld	a, tab
	cp	a, (hl)
	pop	hl
	jr	nz, .doNotAddTabWidth
	call	textio_GetTabWidth
	call	util.AddAToDE
.doNotAddTabWidth:
; If the current line width is less than the window width, jump to cont2
; If not, start a new line
	push	hl
	push	bc
	call	fontlib_GetWindowWidth	; Destroys BC
	push	de
	or	a, a
	sbc	hl, de
	pop	de
	pop	bc
	pop	hl
	jr	nc, .cont2
; Increment the current line number
	ld	a, (.currLineNum)
	inc	a
	ld	(.currLineNum), a
; Set line width to zero
	push	hl
	or	a, a
	sbc	hl, hl
	ex	de, hl
	pop	hl
; If printing right margin flush, subtract one from the current word pointer (HL)
; so that the space after the last word on a line is printed at the start of the
; next line
	ld	a, (_PrintFormat)
	cp	a, bPrintRightMarginFlush
	jr	nz, .doNotDecPointer
	dec	hl
.doNotDecPointer:
; Set current line pointer to current word pointer
	push	hl
	pop	bc
; Jump back to the start so that the curr_word is not incremented
	jp	.loop
.cont2:
; If the next word is not a newline code, jump to cont3
	push	hl
	call	fontlib_GetNewlineCode
	call	fontlib_GetLastCharacterRead
	cp	a, (hl)
	pop	hl
	jr	nz, .cont3
; Else, start new line
; Increment the current line number
	ld	a, (.currLineNum)
	inc	a
	ld	(.currLineNum), a
; Set line width to zero
	or	a, a
	sbc	hl, hl
	ex	de, hl
; Set the current line pointer to the char after the newline code
	call	fontlib_GetLastCharacterRead
	push	hl
	pop	bc
.cont3:
; Increment the current word pointer to the word after the last char read
; If (HL) is NULL, return the address of the null encountered
	call	fontlib_GetLastCharacterRead
	inc	hl
	ld	a, null
	cp	a, (hl)
	jp	nz, .loop
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
; Internal Library Data
;-------------------------------------------------------------

_TabWidth:
	db	4
_PrintFormat:
	db	1