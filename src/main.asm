;-------------------------------------------------------------------------------
include 'include/library.inc'
include 'include/include_library.inc'
;-------------------------------------------------------------------------------

;-------------------------------------------------------------------------------
library 'TEXTIOC',1
;-------------------------------------------------------------------------------

;-------------------------------------------------------------------------------
; Dependencies
include_library 'graphx.asm'
;-------------------------------------------------------------------------------

export textio_PrintWrappedText
export textio_PrintTruncatedStringXY
export textio_SetInputConfig
export textio_LetterInput
export textio_SetTextConfig

;-------------------------------------------------------------------------------
;void textio_PrintTruncatedStringXY(char string[], uint24_t max_width, uint24_t xPos, uint8_t yPos);
textio_PrintTruncatedStringXY:
	ld hl,textio_PrintWrappedText.exit
	jr textio_PrintWrappedText.entry

;-------------------------------------------------------------------------------
;void textio_PrintWrappedText(char *text, uint24_t width, uint24_t xPos, uint8_t yPos);
textio_PrintWrappedText:
	ld hl,.nextline
.entry:
	ld (.smc),hl
	ld iy,0
	add iy,sp
	ld hl,(iy+6)
	ld (.margin_x),hl
	or a,a
	sbc hl,hl
	ld l,(iy+12)
	ld (.y),hl
	push hl
	ld hl,(iy+9)
	push hl
	call gfx_SetTextXY
	pop hl
	ld (.x),hl
	ld (.basex),hl
	pop hl
	ld hl,(iy+3)
.loop:
	ld a,(hl)
	inc hl
	or a,a
	ret z
	ld e,a
	push hl
	push de
	call gfx_GetCharWidth
	ld de,0
.x:=$-3
	add hl,de
	ld (.x),hl
	ld de,320
.margin_x:=$-3
	or a,a
	sbc hl,de
	call nc,.nextline
.smc:=$-3
	call gfx_PrintChar
	pop de
	pop hl
	jr .loop
.nextline:
	ld hl,0
.y:=$-3
	ld de,9
textio_linespacing:=$-3
	add hl,de
	ld (.y),hl
	ld de,231
textio_margin_y:=$-3
	or a,a
	sbc hl,de
	jr nc,.exit
	add hl,de
	push hl
	ld hl,0
.basex:=$-3
	ld (.x),hl
	push hl
	call gfx_SetTextXY
	pop hl
	pop hl
	ret
.exit:
	ld sp,iy
	ret

;-------------------------------------------------------------------------------
;unsigned int textio_SetInputConfig(uint8_t text_FG_color, uint8_t cursor_color);
textio_SetInputConfig:
	pop hl
	pop bc
	pop de
	push de
	push bc
	push hl
	ld hl,textio_LetterInput.cursor
	ld a,(hl)
	ld (hl),c
	ld hl,textio_LetterInput.fg
	ld d,(hl)
	ld (hl),e
	ld e,a
	ex hl,de
	ret

;-------------------------------------------------------------------------------
;unsigned int textio_SetTextConfig(uint8_t text_y_margin, uint8_t line_spacing);
textio_SetTextConfig:
	pop hl
	pop bc
	pop de
	push de
	push bc
	push hl
	ld hl,textio_linespacing
	ld a,(hl)
	ld (hl),c
	ld hl,textio_margin_y
	ld d,(hl)
	ld (hl),e
	ld e,a
	ex hl,de
	ret


;-------------------------------------------------------------------------------
;void textio_LetterInput(char buffer[], uint8_t buffer_size, uint24_t xPos, uint8_t yPos);
textio_LetterInput:
	ld l,0
.fg:=$-1
	push hl
	call gfx_SetTextFGColor
	pop bc
	ld l,0
.cursor:=$-1
	push hl
	call gfx_SetColor
	pop bc
	ld iy,0
	add iy,sp
	ld hl,(iy+3)
	ld (.buffer),hl
	ld (.start),hl
	ld de,(iy+6)
	add hl,de
	ld (.end),hl
	or a,a
	sbc hl,hl
	ld l,(iy+12)
	push hl
	ld hl,(iy+9)
	push hl
.draw:
	ld hl,0
.start:=$-3
	push hl
	call gfx_PrintStringXY
	ld hl,9
	push hl
	push hl
	call gfx_GetTextY
	push hl
	call gfx_GetTextX
	push hl
	call gfx_Rectangle
	pop hl
	pop hl
	pop hl
	pop hl
	ld hl,textio_overtypes
	ld de,0
.mapno:=$-3
	add hl,de
	ld l,a
	push hl
	call gfx_PrintChar
	pop hl
.key:
	call ti.GetCSC
	or a,a
	jr z,.key
	pop hl
	cp a,15
	jr z,.cancel
	cp a,9
	jr z,.enter
	push hl
	ld hl,textio_keymap_A
.keymap:=$-3
	cp a,10
	jr c,.key
	cp a,54
	jr z,.prevmap
	cp a,48
	jr z,.nextmap
	jr nc,.key
	sub a,10
	ld de,0
	ld e,a
	add hl,de
	ld de,0
.end:=$-3
	or a,a
	sbc hl,de
	add hl,de
	jr nc,.key
	ld de,0
.buffer:=$-3
	ldi
	ld (.buffer),de
	jr .key
.cancel:
	xor a,a
	ld (hl),a
.enter:
	pop bc
	pop bc
	ret
.prevmap:
	ld hl,textio_keymaps
	ld de,(.mapno)
	ld a,6
	cp a,e
	jr z,.zero
	inc de
	inc de
	inc de
.cont2:
	ld (.mapno),de
	add hl,de
	ld (.keymap),hl
	jp .draw
.zero:
	ld de,0
	jr .cont2
.nextmap:
	ld hl,textio_keymaps
	ld de,(.mapno)
	xor a,a
	or a,e
	jr nz,.cont
	ld e,9
.cont:
	dec de
	dec de
	dec de
	jr .cont2

;-------------------------------------------------------------------------------
; internal static data
textio_keymaps:
	dl textio_keymap_A,textio_keymap_a,textio_keymap_1,0
textio_keymap_A:
	db "#WRMH  ?!VQLG  :ZUPKFC  YTOJEB  XSNIDA"
textio_keymap_a:
	db "#wrmh  ?!vqlg  :zupkfc  ytojeb  xsnida"
textio_keymap_1:
	db "+-*/^  ;369)$@ .258(&~ 0147,][  ",$1A,"<=>}{"
textio_overtypes:
	db "Aa1"


