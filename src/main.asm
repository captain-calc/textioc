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


;-------------------------------------------------------------
; Global variables
;-------------------------------------------------------------
	arg0	:= 3
	arg1	:= 6
	null	:= $00
	tab		:= $09
	space	:= $20




;-------------------------------------------------------------
; Functions
;-------------------------------------------------------------


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

	pop		de
	pop		hl
	push	hl
	push	de
	ld		a, $02	; See table of valid format codes
	sub		a, l
	jr		c, .invalidValue
	ld		(_PrintFormat), hl
	ld		a, $01
	ret

.invalidValue:
	xor		a, a
	ret


;-------------------------------------------------------------
textio_GetPrintFormat:
; Arguments:
;   None
; Returns:
;   Current print format code
; Destroys:
;   None

	ld		a, (_PrintFormat)
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

	pop		de
	pop		hl
	push	hl
	push	de
	xor		a, a
	cp		a, l
	jr		z, .invalidValue
	ld		(_TabWidth), hl
	inc		a
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
	ld		b, a
	ld		hl, space
	
.loop:
	push	bc
	push	hl
	call	fontlib_DrawGlyph
	pop		hl
	pop		bc
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

	scf				; dbg_Debugger()
	sbc hl,hl
	ld (hl),2
	
	scf
	ccf
	sbc		hl, hl
	ex		hl, de
	
	ld		iy, 0
	add		iy, sp
	ld		hl, (iy + arg0)	; hl -> line
	ld		bc, (iy + arg1) ; bc -> eol

.loop:
	push	hl
	sbc		hl, bc
	pop		hl
	jr		z, .exit
	
	ld		a, (hl)
	cp		a, null
	jr		z, .exit
	
	cp		a, tab
	jr		nz, .addGlyphWidth

	call	textio_GetTabWidth	; Add tab width
	call	util.AddAToDE
	jr		.nextChar

.addGlyphWidth:
	push	de
	push	hl
	ld		d, 0
	ld		e, (hl)
	push	de
	call	fontlib_GetGlyphWidth	; Destroys DE and HL
	pop		hl
	pop		hl
	pop		de
	call	util.AddAToDE

.nextChar:
	inc		hl		; HL should be pointer to line
	jr		.loop

.exit:
	ex		hl, de
	ret



;-------------------------------------------------------------
; Internal Library Functions
;-------------------------------------------------------------


;-------------------------------------------------------------
util.AddAToDE:
; Inputs:
;   A  = Operand 1
;   HL = Operand 2
; Ouputs:
;   DE = DE + A

	add		a, e
	ld		e, a
	jr		nc, .exit
	inc		d
.exit:
	ret

;-------------------------------------------------------------
util.AddHLToDE:
; Inputs:
;   HL = Operand 1
;   DE = Operand 2
; Outputs:
;   DE = HL + DE

	push	hl
	add		hl, de
	ex		hl, de
	pop		hl
	ret




;-------------------------------------------------------------
; Internal Library Data
;-------------------------------------------------------------

_TabWidth:
	db	4
_PrintFormat:
	db	0