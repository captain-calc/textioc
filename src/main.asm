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
    
	export textio_SetCurrCharPtr
	export textio_GetCurrCharPtr
	export textio_SetBufferSize
	export textio_GetBufferSize
    export textio_WriteChar
	export textio_DeleteChar
	export textio_Clear
	export textio_KeyToOffset
    
	export textio_SetPrintFormat
	export textio_GetPrintFormat
	export textio_SetNewlineCode
	export textio_GetNewlineCode
	export textio_SetTabWidth
	export textio_GetTabWidth
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

	null	:= $00
	tab	:= $09
	space	:= $20
	bPrintLeftMarginFlush	:= 1
	bPrintCentered		:= 2
	bPrintRightMarginFlush	:= 3

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
	ld	(_SetTextPosition),hl
	ld	hl,(iy + 4)
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


textio_SetCurrCharPtr:
; Arguments:
;   arg0 = pointer to current character
; Returns:
;   None
; Destroys:
;   HL

	ld	hl,arg0
	add	hl,sp
	ld	hl,(hl)
	ld	(_CurrCharPtr),hl
	ret


;-------------------------------------------------------------
textio_GetCurrCharPtr:
; Arguments:
;   None
; Returns:
;   HL = current character pointer
; Destroys:
;   None

	ld	hl,(_CurrCharPtr)
	ret


;-------------------------------------------------------------
textio_SetBufferSize:
; Arguments:
;   arg0 = buffer size
; Returns:
;   None
; Destroys:
;   HL

	ld	hl,arg0
	add	hl,sp
	ld	hl,(hl)
	ld	(_BufferSize),hl
	ret


;-------------------------------------------------------------
textio_GetBufferSize:
; Arguments:
;   None
; Returns:
;   HL = buffer size
; Destroys:
;   HL

	ld	hl,(_BufferSize)
	ret


;-------------------------------------------------------------
textio_WriteChar:
; Arguments:
;   arg0 = pointer to buffer
;   arg1 = character to insert
; Returns:
;   A == 0 if character was written; A == 1, otherwise
; Destroys:
;   ...

	ld	hl,arg0
	add	hl,sp
	ld	bc,(hl)			; BC -> buffer
	inc	hl
	inc	hl
	inc	hl
	ld	e,(hl)			; E = character

; If the curr_char_ptr == end of buffer, return
	ld	hl,(_BufferSize)
	add	hl,bc			; HL -> end of buffer
	push	de
	ld	de,(_CurrCharPtr)
	xor	a,a
	sbc	hl,de
	pop	hl			; L = character
	ld	a,1
	ret	z

; Verify the *curr_char_ptr == NULL and insert char if true
; Also insert char if OVERWRITE_MODE is on
	ex	de,hl
	xor	a,a
	cp	a,(hl)
	jr	z,.writeChar
	ld	a,(_OverwriteMode)
	or	a,a
	jr	nz,.writeChar

; If not, shift the characters in front of the curr_char_ptr
; to the right, if possible
	push	hl
	push	de
	push	bc
	pop	de
	call	util.ShiftCharsRight
	pop	de
	pop	hl

; If the shift failed (A == 1), return
	or	a,a
	ret	nz

; Else if the shift succeeded or the *curr_char_ptr == NULL,
; insert the character and increment the curr_char_ptr
.writeChar:
	ld	(hl),e
	inc	hl
	ld	(_CurrCharPtr),hl
	ret


;-------------------------------------------------------------
util.ShiftCharsRight:
; Arguments:
;   HL -> start of string to shift
;   DE -> buffer
; Returns:
;   A == 0, if successful; A == 1, if buffer is full
; Destroys:
;   BC, DE, and HL

    ex  de,hl           ; After exchange: DE = (_CurrCharPtr)
    ld  bc,(_BufferSize)
    add hl,bc
    xor a,a
    cp  a,(hl)
    ld  a,1
    ret nz          ; Returning now means the buffer is full

    push    de
    pop hl          ; HL = (_CurrCharPtr)
    xor a,a

; This should be replaced by a binary search. A binary search will
; perform much faster on large buffers.
    cpir            ; BC = (_BufferSize)
                    ; This will repeat until BC == 0 or (HL) == NULL
	dec	hl

; Now, we must subtract DE from HL to find out how many characters we need to shift
    push    hl
    push    hl
    xor a,a
    sbc hl,de
    push    hl
    pop bc
    pop de          ; DE -> first NULL character after string to shift
    pop hl
	dec	hl
    lddr
	inc	hl
    ret


;-------------------------------------------------------------
textio_DeleteChar:
; Arguments:
;   arg0 = pointer to buffer
; Returns:
;   HL = width of target char if successful; HL = 0 if curr_char_ptr == buffer
; Destroys:
;   A, BC, and DE

	ld	hl,arg0
	add	hl,sp
	ld	de,(hl)
	ld	hl,(_CurrCharPtr)

; If curr_char_ptr == buffer, return
	push	hl
	xor	a,a
	sbc	hl,de
	pop	bc
	ld	hl,0
	ret	z

; Decrement the curr_char_ptr
	push	bc
	pop	hl
	dec	hl
	ld	(_CurrCharPtr),hl
	push	de
	
; Get the width of the target char
	ld	bc,(hl)
	push	hl
	push	bc
	call	util.GetCharWidth
	pop	bc
	ex	de,hl
	pop	hl
	pop	bc			; BC -> buffer

; Set target char to NULL
	xor	a,a
	ld	(hl),a

; If the char after the target char == NULL or
; OVERWRITE_MODE is on, return
	inc	hl
	cp	a,(hl)
	ex	de,hl
	ret	z
	ld	a,(_OverwriteMode)
	or	a,a
	ret	nz

; Else, shift all characters in after the deleted
; character left
	ex	de,hl
	push	de
	push	bc
	pop	de			; DE -> buffer
	pop	bc			; BC = width of target char
	dec	hl
	ex	de,hl
	push	bc
	ld	bc,(_BufferSize)
	add	hl,bc
	xor	a,a
	sbc	hl,de			; DE -> target char
	push	hl
	pop	bc
	push	de
	pop	hl
	inc	hl
	ldir
	pop	hl
	ret


;-------------------------------------------------------------
textio_Clear:
; Arguments:
;   arg0 = pointer to buffer
;   arg1 = pointer to start of string to delete
;   arg2 = number of chars to delete
; Returns:
;   HL = total width of deleted chars
; Destroys:
;   A, BC, and DE

	ld	hl,0
.bufferPtr := $ - 3
	ld	hl,arg0
	add	hl,sp
	ld	bc,(hl)
	ld	(.bufferPtr),bc
	inc	hl
	inc	hl
	inc	hl
	ld	de,(hl)			; DE -> start of string to delete
	inc	hl
	inc	hl
	inc	hl
	ld	bc,(hl)			; BC = number of chars to delete

; Return if the number of chars to delete == 0
	ld	hl,0
	xor	a,a
	sbc	hl,bc
	ret	nc

; Calculate the total width of the chars
;    DE -> start of string to delete
;    BC = number of chars to delete
	ld	hl,0
.totalWidth := $ - 3
	push	bc
	push	de
	call	textio_GetStringWidthL
	ld	(.totalWidth),hl
	pop	de
	pop	bc
	push	bc			; Save both of these for later
	push	de

; Delete the chars
.deleteChar:
	xor	a,a
	ld	(de),a
	inc	de
	dec	bc
	ld	hl,0
	sbc	hl,bc
	jr	c,.deleteChar
	
; Return if OVERWRITE_MODE is on
	pop	de			; DE -> start of string to delete
	pop	bc			; BC = number of chars to delete
	ld	a,(_OverwriteMode)
	or	a,a
	jr	nz,.finish
	push	de
	
; Shift all of the chars after the deleted string left
;	Calculate how many chars are after the deleted string (buffer_ptr + buffer_size) - (arg1 + arg2)
	ex	de,hl
	add	hl,bc
	ex	de,hl			; After exchange: DE = arg1 + arg2
	ld	hl,(.bufferPtr)
	ld	bc,(_BufferSize)
	add	hl,bc			; HL = buffer_ptr + buffer_size
	sbc	hl,de			; HL = number of chars after the deleted string
	push	hl
	pop	bc			; BC = number of chars after the deleted string

; If BC == 0, return
	ld	hl,0
	sbc	hl,bc
	ex	de,hl			; After exchange: HL -> first char of string to shift
	pop	de			; DE -> start of deleted string
	jr	z,.finish
	ldir

; At this point, DE -> char after the end of the shifted string
; Delete any remaining chars after the shifted string
; 	Loop until HL == buffer_ptr + buffer_size:
; 		(HL) = 0
	ld	hl,(.bufferPtr)
	ld	bc,(_BufferSize)
	add	hl,bc
	xor	a,a
	sbc	hl,de
	ld	bc,0
.eraseChar:
	ld	(de),a
	sbc	hl,bc
	inc	de
	dec	hl
	jr	nz,.eraseChar

.finish:
	ld	hl,(.totalWidth)
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


textio_SetPrintFormat:
; Arguments:
;   arg0 = format code
; Returns:
;   A = 0 if invalid format code passed; A is returned intact, otherwise
; Destroys:
;   A
;   DE
;   HL

	ld	hl,arg0
	add	hl,sp
	ld	a,bPrintRightMarginFlush
	sub	a,(hl)
	jr	c,.invalidValue
	ld	a,(hl)
	cp	a,0
	jr	z,.invalidValue
	ld	(_PrintFormat),a
	ret

.invalidValue:
	xor	a,a
	ret


;-------------------------------------------------------------
textio_GetPrintFormat:
; Arguments:
;   None
; Returns:
;   A = current print format code
; Destroys:
;   None

	ld	a,(_PrintFormat)
	ret


;-------------------------------------------------------------
textio_SetNewlineCode:
; Arguments:
;   arg0 = character
; Returns:
;   None
; Destroys:
;   A and HL

	ld	hl,arg0
	add	hl,sp
	ld	a,(hl)
	ld	(_NewlineCode),a
	ret


;-------------------------------------------------------------
textio_GetNewlineCode:
; Arguments:
;   None
; Returns:
;   A = newline character
; Destroys:
;   None

	ld	a,(_NewlineCode)
	ret


;-------------------------------------------------------------
textio_SetTabWidth:
; Arguments:
;   arg0 = width
; Returns:
;   None
; Destroys:
;   A and HL

	ld	hl,arg0
	add	hl,sp
	ld	a,(hl)
	cp	a,0
	ret	z
	ld	(_TabWidth),a
	ret


;-------------------------------------------------------------
textio_GetTabWidth:
; Arguments:
;   None
; Returns:
;   A = tab width
; Destroys:
;   None

	ld	a,(_TabWidth)
	ret


;-------------------------------------------------------------
textio_GetCharWidth:
; Arguments:
;   arg0 = char
; Returns:
;   HL = width of char
; Destroys:
;   All

	ld	hl,arg0
	add	hl,sp
	ld	a,(hl)
	cp	a,tab
	ld	hl,(_TabWidth)
	ret	z
	or	a,a
	sbc	hl,hl
	ld	l,a
	push	hl
	call	util.GetCharWidth
	pop	de
	ret


;-------------------------------------------------------------
textio_GetLineWidth:
; Arguments:
;   arg0  =  pointer to line
;   arg1  =  pointer to end of line
; Returns:
;   HL = Width of line
; Destroys:
;   All

	ld	iy,0
	add	iy,sp
	ld	de,(iy + arg0)			; DE -> line
	ld	hl,(iy + arg1) 			; HL -> eol
	xor	a,a
	sbc	hl,de
	push	hl
	push	de
	call	textio_GetStringWidthL
	pop	de
	pop	de
	ret


;-------------------------------------------------------------
textio_GetStringWidthL:
; Arguments:
;   arg0 = pointer to string
;   arg1 = number of characters
; Returns:
;   HL = width of characters
; Destroys:
;   All

	ld	hl,arg1
	add	hl,sp
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

	ld	a,(_NewlineCode)
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
	push	de
	call	textio_GetCharWidth
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
textio_GetLinePtr:
; Arguments:
;   arg0 = pointer to text
;   arg1 = line number
;   arg2 = maximum line width
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
	ld	de,(hl)
	ld	(_CurrLineNum),de
	inc	hl
	inc	hl
	inc	hl
	ld	de,(hl)
	ld (_MaxLineWidth),de
<<<<<<< HEAD
<<<<<<< HEAD
	push	bc
	push	bc
=======
	push	bc
	push	bc
>>>>>>> 25b1288c97986ae4674ec439bf03a8fdab53563e
=======
	push	bc
	push	bc
>>>>>>> 25b1288c97986ae4674ec439bf03a8fdab53563e
	pop	hl				; BC -> text

.outerLoop:
; _CurrLineNum acts as a decrementing counter. When it reaches zero, return pointer
	pop	bc
	push	hl
	ld	de,0
	ld	hl,(_CurrLineNum)
	xor	a,a
	sbc	hl,de
	pop	hl
	ret	z

	push	bc				; Save pointer to start of line
	call	util.GetApproximateLinePtr
	mOpenDebugger
	pop	de
	xor	a,a
	cp	a,(hl)
	ret z

	push	hl
	pop	bc				; BC = approximated pointer
	push	de			; DE -> start of line

.innerLoop:
	ld	a,(_NewlineCode)
	cp	a,(hl)
	jr	z,.skipSpaceOrNewline

	push	hl
	or	a,a
	sbc	hl,de
	pop	hl
	jr	z,.breakString

<<<<<<< HEAD
<<<<<<< HEAD
	dec hl
=======
>>>>>>> 25b1288c97986ae4674ec439bf03a8fdab53563e
=======
>>>>>>> 25b1288c97986ae4674ec439bf03a8fdab53563e
	ld	a,(hl)
	cp	a,space
	jr	z,.formatSpace
	cp	a,tab
	jr	z,.startNewLine		; Tabs are always prepended to the next line
<<<<<<< HEAD
<<<<<<< HEAD
=======
	dec	hl
>>>>>>> 25b1288c97986ae4674ec439bf03a8fdab53563e
=======
	dec	hl
>>>>>>> 25b1288c97986ae4674ec439bf03a8fdab53563e

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
	ld	a,(_PrintFormat)
	cp	a,bPrintRightMarginFlush
	jr	z,.startNewLine

.skipSpaceOrNewline:
	inc	hl

.startNewLine:
	ld	de,(_CurrLineNum)			; Decrement the current line number
	dec	de
	ld	(_CurrLineNum),de
	pop	bc
	push	hl						; Set the current line pointer to the current char pointer
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

	ld	a,(_NewlineCode)
	cp	a,(hl)
	ret	z

	push	hl			; HL -> current character
	push	de			; DE = line width (running total)
	ld	e,(hl)
	xor	a,a
	sbc	hl,hl
	ld	l,e
	push	hl
	call	util.GetCharWidth
	pop	de
	pop	de
	add	hl,de
	ex	de,hl
	ld	hl,(_MaxLineWidth)
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
util.SetTextPosition:
; Sets the cursor position using the external function
; whose address is in _SetTextPosition
; Arguments:
;   arg0 = xPos
;   arg1 = yPos
; Returns:
;   None
; Destroys:
;   All

	ld	hl,arg1
	add	hl,sp
	ld	de,(hl)
	push	de
	dec	hl
	dec	hl
	dec	hl
	ld	de,(hl)
	push	de
	ld	hl,(_SetTextPosition)
	call	util.CallHL
	pop	hl
	pop	hl
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
_SetTextPosition:
	dl	0
_GetCharWidth:
	dl	0

; Data for input functions
_OverwriteMode:
	db	0
_BufferSize:
    dl  0
_CurrCharPtr:
	dl	0

; Data for text output functions
_TabWidth:
	dl	16
_PrintFormat:
	db	1
_NewlineCode:
	db	$0a

; Data for textio_GetLinePtr
_CurrLineNum:
	dl	0
_MaxLineWidth:
	dl	320