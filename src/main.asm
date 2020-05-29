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
	export textio_SetTabWidth
	export textio_GetTabWidth
	export textio_GetLineWidth


;-------------------------------------------------------------
; Global variables
;-------------------------------------------------------------
	arg0	:= 3
	arg1	:= 6
	null	:= $00
	tab		:= $09




;-------------------------------------------------------------
; Functions
;-------------------------------------------------------------


;-------------------------------------------------------------
textio_SetTabWidth:
; Arguments:
;   arg0 = width
; Returns:
;   None
; Destroys:
;   HL
;   DE

	pop		hl
	pop		de
	push	de
	push	hl
	ld		(_TabWidth), de
	ret

;-------------------------------------------------------------
textio_GetTabWidth:
; Arguments:
;   None
; Returns:
;   Width of tab

	ld	hl, (_TabWidth)
	ret
	

;-------------------------------------------------------------
textio_GetLineWidth:
; Arguments:
;   arg0  =  pointer to line
;   arg1  =  pointer to end of line
; Returns:
;   Width of line

	scf				; dbg_Debugger()
	sbc hl,hl
	ld (hl),2
	
	ld		iy, 0
	add		iy, sp
	ld		hl, (iy + arg0)	; hl -> line
	ld		bc, (iy + arg1) ; bc -> eol

	ld		e, 0
	ld		d, 0

.loop:
	ld		a, (hl)
	cp		a, null
	jr		z, .exit

	push	hl
	sbc		hl, bc
	jr		z, .exit

	pop		hl
	ld		a, (hl)
	cp		a, tab
	jr		nz, .addGlyphWidth

	push	hl
	call	textio_GetTabWidth
	call	util.AddHLToDE
	pop		hl
	jr		.nextChar

.addGlyphWidth:
	push	de
	push	hl
	call	fontlib_GetGlyphWidth
	pop		hl
	pop		de
	call	util.AddAToDE

.nextChar:
	inc		hl		; HL should be pointer to line
	jr		.loop

.exit:
	ex		de, hl
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

	add	a, e
	ld	e, a
	jr	nc, .exit
	inc	d
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
	dl	4
_PrintFormat:
	db	0