;-------------------------------------------------------------
; Includes
;-------------------------------------------------------------
	include 'include/library.inc'
	include 'include/include_library.inc'

;-------------------------------------------------------------
	library 'TEXTIOC',1
;-------------------------------------------------------------

;-------------------------------------------------------------
; v1 functions
;-------------------------------------------------------------
	
	export textio_SetLibraryRoutines
    
	export textio_InsertChar
	export textio_InsertString
	export textio_WriteChar
	export textio_WriteString
	export textio_ShiftDeleteChar
	export textio_ShiftDeleteString
	export textio_DeleteChar
	export textio_DeleteString
	export textio_ShiftStringLeft
	export textio_ShiftStringRight
	export textio_KeyToOffset
    
	export textio_GetCharWidth
	export textio_GetLineWidth
	export textio_GetStringWidthL
	export textio_GetLinePtr

;-------------------------------------------------------------
; Global defines
;-------------------------------------------------------------
	arg0	:= 3
	arg1	:= 6
	arg2	:= 9
	arg3	:= 12
	arg4	:= 15

	null	:= $00
	tab	:= $09
	space	:= $20
	bPrintLeftMarginFlush	:= 1
	bPrintCentered		:= 2
	bPrintRightMarginFlush	:= 3

; These define where the offsets are for data in textio_output_data_t structure
	tab_width	:= 0
	newline	:= 3
	print_format	:= 4
	max_line_width	:= 5

macro mOpenDebugger
	push	hl
	scf
	sbc	hl,hl
	ld	(hl),2
	pop	hl
end macro


;=============================================================;
;                                                             ;
;                Library Setup Functions                      ;
;                                                             ;
;=============================================================;


textio_SetLibraryRoutines:
; Arguments:
;   arg0 = pointer to structure
; Returns:
;   None
; Destroys:
;   HL and IY

	ld	hl,arg0
	add	hl,sp
	ld	iy,(hl)
	ld	a,(iy)
	ld	(_LibraryVersion),a
	ld	hl,(iy + 1)
	ld	(_GetCharWidth),hl
; If a second version is released that uses more external functions,
; use the following to supply only those functions that the library
; version requires.
;	ld	a,2
;	cp	a,(_LibraryVersion)
;	ret	z
	ret


;=============================================================;
;                                                             ;
;                  Text Input Functions                       ;
;                                                             ;
;=============================================================;


textio_InsertChar:
; Arguements:
;   arg0 -> buffer
;   arg1 = buffer size
;   arg2 = character
;   arg3 -> write location
; Returns:
;   A == 0, if successful; A == 1, if the char could not fit
; Destroys:
;   BC, DE, HL

	ld	a,0
.char := $ - 1

	ld	hl,arg2
	add	hl,sp
	ld	a,(hl)
	ld	(.char),a
	ld	de,.char
	ld	(hl),de		; Load the address of the char into the stack

	ld	hl,1
	push	hl
	ld	hl,arg4 + 3	; Plus 3 for the stack pointer, and arg4 because HL is decremented and then accessed
	add	hl,sp
	ld	a,4

; Push all of the parameters to this function onto the stack
.loop:
	dec	hl
	dec	hl
	dec	hl
	ld	de,(hl)
	push	de
	dec	a
	or	a,a
	jr	nz,.loop

	call	textio_InsertString
	pop	hl
	pop	hl
	pop	hl
	pop	hl
	pop	hl
	ret


;-------------------------------------------------------------
textio_InsertString:
; Arguments:
;   arg0 -> buffer
;   arg1 = buffer size
;   arg2 -> string
;   arg3 -> write location
;   arg4 = length of string
; Returns:
;   A == 0, if successful; A == 1, if the string could not fit
; Destroys:
;   BC, DE, HL

; Find the length of the string at the write location
	ld	hl,arg2
	add	hl,sp
	ld	de,(hl)		; DE -> argument string
	push	de
	inc	hl
	inc	hl
	inc	hl
	ld	hl,(hl)		; HL -> write location
	push	hl

	push	hl
	ld	bc,0		; BC will overflow
	xor	a,a
	cpir
	pop	de		; DE -> write location
	push	hl
	xor	a,a
	sbc	hl,de
	dec	hl		; HL = length of string at write location (could be 0)
	push	hl

; Find how many NULL bytes are in the buffer
; On stack:
;    Length of string at write location
;    Pointer to first NULL byte found
;    Pointer to write location
;    Pointer to argument string
	ld	hl,arg0 + 12		; Four stack locals
	add	hl,sp
	ld	de,(hl)		; DE -> buffer
	inc	hl
	inc	hl
	inc	hl
	ld	bc,(hl)		; BC = buffer size
	ex	de,hl
	add	hl,bc		; HL -> NULL terminator of buffer
	pop	bc		; BC = length of string at write location
	pop	de		; DE -> first NULL byte found
	xor	a,a
	sbc	hl,de
	inc	hl		; HL = number of NULL bytes at end of buffer

; If the number of NULL bytes < length of argument string. return error
	ex	de,hl
	ld	hl,arg4 + 6		; Two stack locals
	add	hl,sp
	ld	hl,(hl)		; HL = length of argument string
	push	hl
	inc	de		; Increment the number of NULL bytes, so if HL == 
	sbc	hl,de		; the previous value of DE, the carry flag will be set
	pop	hl
	jr	c,.testWriteLoc

	ld	a,1
	pop	hl		; Discard stack locals
	pop	hl
	ret

.testWriteLoc:
	dec	de		; Restore the true number of NULL bytes
; If length of string at write location == 0, write string and return
	push	hl
	ld	hl,0
	xor	a,a
	sbc	hl,bc
	pop	hl		; HL = length of argument string
	jr	c,.shiftPrep

; The state of the registers here and at the end of the next codeblock are the same,
; so the same process can be used to load the registers with the proper variables
; before shifting/writing the parameter string.
	push	hl
	pop	bc
	pop	de		; DE -> write location
	pop	hl		; HL -> argument string
	jr	.writeString

.shiftPrep:
; Else, if length of the argument string is greater than the number of NULL bytes,
; return error
	push	hl
	inc	de		; Increment DE so if HL == previous value of DE, the carry flag will be set
	xor	a,a
	sbc	hl,de
	pop	bc		; BC = length of argument string
	ld	a,1
	pop	de		; DE -> write location
	pop	hl		; HL -> argument string
	ret nc

; Else, shift the string at the write location by the length of the parameter string
	push	hl
	push	de
	push	bc
	ex	de,hl
	call	util.ShiftStringRight
	pop	bc
	pop	de
	pop	hl

.writeString:
	ldir
	xor	a,a
	ret


;-------------------------------------------------------------
textio_WriteChar:
; Arguments:
;   arg0 -> write location
;   arg1 = character
; Returns:
;   None
; Destroys:
;   BC, DE, HL

	pop	de
	pop	hl
	pop	bc
	push	bc
	push	hl
	push	de
	ld	a,c
	ld	(hl),a
	ret


;-------------------------------------------------------------
textio_WriteString:
; Arguments:
;   arg0 -> write location
;   arg1 = number of chars to write
;   arg2 -> string
; Returns:
;   None
; Destroys:
;   BC, DE, HL

	ld	hl,arg0
	add	hl,sp
	ld	de,(hl)
	inc	hl
	inc	hl
	inc	hl
	ld	bc,(hl)
	inc	hl
	inc	hl
	inc	hl
	ld	hl,(hl)
	ldir
	ret


;-------------------------------------------------------------
textio_ShiftDeleteChar:
; Arguments:
;  arg0 -> buffer
;  arg1 = buffer size
;  arg2 -> string to delete
; Returns:
;  HL = width of character deleted; HL == 0 if delete failed
; Destroys:
;  A, BC, DE

	ld	hl,1
	push	hl
	ld	hl,arg2 + 3		; One stack local
	add	hl,sp
	ld	a,3
.loop:
	ld	de,(hl)
	push	de
	dec	hl
	dec	hl
	dec	hl
	dec	a
	or	a
	jr	nz,.loop
	call	textio_ShiftDeleteString
	pop	de
	pop	de
	pop	de
	pop	de
	ret


;-------------------------------------------------------------
textio_ShiftDeleteString:
; Arguments:
;   arg0 -> buffer
;   arg1 = buffer size
;   arg2 -> string to delete
;   arg3 = length of string
; Returns:
;   HL = width of string deleted; HL == 0 if delete failed
; Destroys:
;   A, BC, DE

	ld	hl,arg0
	add	hl,sp
	ld	de,(hl)		; DE -> buffer
	inc	hl
	inc	hl
	inc	hl
	ld	bc,(hl)		; BC = buffer size
	inc	hl
	inc	hl
	inc	hl
	ld	hl,(hl)		; HL -> string to delete

; If character after HL == NULL, delete the character at HL and return
	inc	hl
	xor	a,a
	cp	a,(hl)
	dec	hl
	jr	nz,.isDeleteValid
	ex	de,hl
	sbc	hl,hl
	ex	de,hl
	ld	de,(hl)
	ld	(hl),a
	push	de
	call	textio_GetCharWidth
	pop	de
	ret

.isDeleteValid:
; If the length of the string + delete location > end of buffer, return
	ex	de,hl
	add	hl,bc		; HL -> NULL terminator of buffer
	xor	a,a
	sbc	hl,de		; HL = number of bytes between delete location and end of buffer
	push	de		; DE -> string to delete
	ex	de,hl
	ld	hl,arg3 + 3		; One stack local
	add	hl,sp
	ld	hl,(hl)
	push	hl
	xor	a,a
	sbc	hl,de
	pop	bc		; BC = length of string
	pop	de		; DE -> string to delete
	jr	c,.getStrWidth
	sbc	hl,hl
	ret

.getStrWidth:
; Get width of string to be deleted
	push	bc
	push	de
	call	textio_GetStringWidthL
	pop	de
	pop	bc

; HL should point to the first character of the string that will be
; shifted left. Get the length of this string and store to BC
	push	hl		; HL = width of string to be deleted
	push	de
	ex	de,hl
	add	hl,bc		; HL -> first char of string to be shifted left
	push	hl
	ld	bc,0
	dec	bc		; BC will overflow
	xor	a,a
	cpir
	pop	de
	sbc	hl,de
	dec	hl		; HL = length of string to be shifted left

; Shift string left
	push	hl
	pop	bc
	pop	hl		; HL -> string to delete
	ex	de,hl
	ldir

; Delete any non-NULL bytes at the end of the shifted string
	ex	de,hl
	dec	hl
.loop:
	inc	hl
	cp	a,(hl)
	ld	(hl),a
	jr	nz,.loop

	pop	hl
	ret


;-------------------------------------------------------------
textio_DeleteChar:
; Arguments:
;   arg0 -> char to delete
; Returns:
;   None
; Destroys:
;   A, DE, HL

	pop	de
	pop	hl
	push	hl
	push	de
	xor	a,a
	ld	(hl),a
	ret


;-------------------------------------------------------------
textio_DeleteString:
; Arguments:
;   arg0 -> string to delete
;   arg1 = length of string
; Returns:
;   None
; Destroys:
;   A, BC, DE, HL

	pop	de
	pop	hl
	pop	bc
	push	bc
	push	hl
	push	de

; Return if BC == 0
	push	hl
	ld	hl,0
	xor	a,a
	sbc	hl,bc
	pop	hl
	ret	z
.loop:
	ld	(hl),a
	inc	hl
	dec	bc
	push	hl
	ld	hl,0
	xor	a,a
	sbc	hl,bc
	pop	hl
	jr	c,.loop
	ret


;-------------------------------------------------------------
textio_ShiftStringLeft:
; Arguments:
;   arg0 -> string
;   arg1 = shift distance
; Returns:
;   None
; Destroys:
;   BC, DE, HL

	pop	de
	pop	hl
	pop	bc
	push	bc
	push	hl
	push	de
assert $= util.ShiftStringLeft


util.ShiftStringLeft:
; Arguments:
;   HL -> string
;   BC = shift distance
; Returns:
;   None
; Destroys:
;   BC, DE, HL

	ex	de,hl
	add	hl,bc
	ex	de,hl
	ldir
	ret


;-------------------------------------------------------------
textio_ShiftStringRight:
; Arguments:
;   arg0 = pointer to start of string to shift
;   arg1 = shift distance
; Returns:
;   None; shifts the string right by the shift distance
; Destroys:
;   A, BC, DE, HL

	pop	de
	pop	hl
	pop	bc
	push	bc
	push	hl
	push	de
assert $= util.ShiftStringRight


util.ShiftStringRight:
; Arguments:
;   HL -> string to shift
;   BC = shift distance
; Returns:
;   None; shifts the string at HL right by BC bytes
; Destroys:
;   A, BC, DE, HL

	push	bc
	push	hl
	ld	bc,0
	dec	bc		; BC will overflow
	push	bc
	xor	a,a
	cpir
	pop	hl
	sbc	hl,bc
	dec	hl		; HL = length of string to shift
	pop	de		; DE -> string to shift
	ex	de,hl
	add	hl,de
	dec	hl		; HL -> last char of string to shift
	ex	de,hl
	push	hl
	pop	bc		; BC = length of string to shift
	pop	hl		; DE = shift distance
	add	hl,de
	ex	de,hl		; After exchange: DE -> location where last char of string to shift should go
	lddr
	ret


;-------------------------------------------------------------
textio_KeyToOffset:
; Arguments:
;   None
; Returns:
;   A = keymap offset
; Destroys:
;   BC and HL

	ld	hl,$f50010
	xor	a,a
	ld	b,0
.scanGroup:
	inc	b
	bit	3,b
	jr	nz,.notFound
	inc	hl
	inc	hl
	or	a,(hl)
	jr	z,.scanGroup

	ld	c,255
.getKey:
	inc	c
	srl	a
	jr	nz,.getKey
	ld	a,c

; Return 57 - 8 * B + A
.finish:
	ld	c,8
	mlt	bc
	add	a,57
	sub	a,c
	ret

.notFound:
	ld	a,-1
	ret


;=============================================================;
;                                                             ;
;                 Text Output Functions                       ;
;                                                             ;
;=============================================================;


textio_GetCharWidth:
; Arguments:
;   arg0 = character
;   arg1 = pointer to textio_output_data_t structure
; Returns:
;   HL = width of character
; Destroys:
;   All

	ld	hl,arg0
	add	hl,sp
	ld	a,(hl)
	inc	hl
	inc	hl
	inc	hl
	ld	hl,(hl)
	ld	hl,(hl)		; HL = tab_width
	cp	a,tab
	ret	z
	or	a,a
	sbc	hl,hl
	ld	l,a
	push	hl
	call	util.GetCharWidth
	pop	de
	ret


;-------------------------------------------------------------
textio_GetStringWidthL:
; Arguments:
;   arg0 = pointer to string
;   arg1 = number of characters
;   arg2 = pointer to textio_output_data_t structure
; Returns:
;   HL = width of characters
; Destroys:
;   All

	ld	hl,arg2
	add	hl,sp
	ld	de,(hl)
	
	nop
	nop
	nop
.structPtr := $ - 3
	ld	(.structPtr),de
	
	dec	hl
	dec	hl
	dec	hl
	ld	bc,(hl)
	dec	hl
	dec	hl
	dec	hl
	ld	hl,(hl)
	inc	bc
	ld	de,0				; DE = width of characters
.loop:
; Return if BC == 0
	push	hl
	dec	bc
	push	bc
	push	bc
	pop	hl
	ld	bc,0
	xor	a,a
	sbc	hl,bc
	pop	bc
	pop	hl
	jr	z,.exit

	push	hl
	ld	hl,(.structPtr)
	inc	hl
	inc	hl
	inc	hl
	ld	a,(hl)		; A = newline character
	pop	hl
	cp	a,(hl)
	jr	z,.exit

	ld	a,(hl)
	cp	a,null
	jr	z,.exit

	push	hl
	push	bc
	push	de
	ld	de,0
	ld	e,(hl)
	ld	hl,(.structPtr)
	push	hl
	push	de
	call	textio_GetCharWidth
	pop	de
	pop	de
	pop	de
	add	hl,de
	ex	de,hl
	pop	bc
	pop	hl
	inc	hl				; HL should be pointer to line
	jr	.loop

.exit:
	ex	hl,de
	ret


;-------------------------------------------------------------
textio_GetLineWidth:
; Arguments:
;   arg0 = pointer to line
;   arg1 = pointer to end of line
;   arg2 = pointer to textio_output_data_t structure
; Returns:
;   HL = Width of line
; Destroys:
;   All

	ld	iy,0
	add	iy,sp
	ld	de,(iy + arg0)		; DE -> line
	ld	hl,(iy + arg1) 		; HL -> eol
	ld	bc,(iy + arg2)		; BC -> structure
	xor	a,a
	sbc	hl,de
	push	bc
	push	hl
	push	de
	call	textio_GetStringWidthL
	pop	de
	pop	de
	pop	de
	ret
	

;-------------------------------------------------------------
textio_GetLinePtr:
; Arguments:
;   arg0 = pointer to text
;   arg1 = line number
;   arg2 = pointer to textio_output_data_t structure
; Returns:
;   HL = pointer to next line; HL = NULL if error
; Destroys:
;   All

	ld	hl,arg0
	add	hl,sp
	ld	bc,(hl)			; BC -> text
	inc	hl
	inc	hl
	inc	hl
	ld	de,0
.currLineNum := $ - 3
	ld	de,(hl)
	ld	(.currLineNum),de
	inc	hl
	inc	hl
	inc	hl
	ld	hl,(hl)
	ld	de,0
.structPtr := $ - 3
	ld	(.structPtr),hl
	inc	hl
	inc	hl
	inc	hl
	ld	a,0
.newline := $ - 1
	ld	a,(hl)
	ld	(.newline),a
	inc	hl
	ld	a,0
.printFormat := $ - 1
	ld	a,(hl)
	ld	(.printFormat),a
	inc	hl
	ld	de,0
.maxLineWidth := $ - 3
	ld	de,(hl)
	ld	(.maxLineWidth),de
	
	push	bc
	push	bc
	pop	hl		; BC -> text

.outerLoop:
; .currLineNum acts as a decrementing counter. When it reaches zero, return pointer
	pop	bc
	push	hl
	ld	de,0
	ld	hl,(.currLineNum)
	xor	a,a
	sbc	hl,de
	pop	hl
	ret	z

	push	bc		; Save pointer to start of line
	call	util.GetApproximateLinePtr
	pop	de
	xor	a,a
	cp	a,(hl)
	ret z

	push	hl
	pop	bc		; BC = approximated pointer
	push	de		; DE -> start of line

.innerLoop:
	ld	a,(.newline)
	cp	a,(hl)
	jr	z,.skipSpaceOrNewline

	push	hl
	or	a,a
	sbc	hl,de
	pop	hl
	jr	z,.breakString

	dec hl
	ld	a,(hl)
	cp	a,space
	jr	z,.formatSpace
	cp	a,tab
	jr	z,.startNewLine		; Tabs are always prepended to the next line

	jr	.innerLoop

.breakString:
; If this executes, HL == DE and DE points to a long string
; with no spaces in it. BC points to the character that, if
; appended to the string, would make it too long. Thus, this
; character should be the first character on the next line.
	push	bc
	pop	hl
	jr	.startNewLine

.formatSpace:
; Spaces should be prepended to the next line if printing right-margin flush
	ld	a,(.printFormat)
	cp	a,bPrintRightMarginFlush
	jr	z,.startNewLine

.skipSpaceOrNewline:
	inc	hl

.startNewLine:
	ld	de,(.currLineNum)		; Decrement the current line number
	dec	de
	ld	(.currLineNum),de
	pop	bc
	push	hl		; Set the current line pointer to the current char pointer
	jr	.outerLoop


;-------------------------------------------------------------
util.GetApproximateLinePtr:
; Arguments:
;   HL = pointer to start of line
; Returns:
;   HL = approximated pointer to next line
; Destroys:
;   All working registers

	ld	de,0
.loop:
	xor	a,a
	cp	a,(hl)
	ret	z

	ld	a,(textio_GetLinePtr.newline)
	cp	a,(hl)
	ret	z

	push	hl			; HL -> current character
	push	de			; DE = line width (running total)
	ld	e,(hl)
	ld	hl,(textio_GetLinePtr.structPtr)
	push	hl
	xor	a,a
	sbc	hl,hl
	ld	l,e
	push	hl
	call	textio_GetCharWidth
	pop	de
	pop	de
	pop	de
	add	hl,de
	ex	de,hl
	ld	hl,(textio_GetLinePtr.maxLineWidth)
	xor	a,a
	sbc	hl,de
	pop	hl
	ret	c

	inc	hl
	jr	.loop


;-------------------------------------------------------------
; Internal Library Functions
;-------------------------------------------------------------


util.CallHL:
; Calls HL
; Inputs:
;   HL = address to jump to
; Outputs:
;   None

; Return if HL == 0
	add	hl,bc
	or	a,a
	sbc	hl,bc
	ret	z
	jp	(hl)
	ret


;-------------------------------------------------------------
util.GetCharWidth:
; Gets the width of a character using the external
; function whose address is in _GetCharWidth
; Arguments:
;   arg0 = character
; Returns:
;   HL = width of character
; Destroys:
;   All

	ld	hl,arg0
	add	hl,sp
	ld	hl,(hl)
	push	hl
	ld	hl,(_GetCharWidth)
	call	util.CallHL
	pop	de
	ret


;-------------------------------------------------------------
; Internal Library Data
;-------------------------------------------------------------


_LibraryVersion:
	db	1

; Pointers for external library function implementations
_GetCharWidth:
	dl	0