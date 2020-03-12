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
	ld iy,0
	add iy,sp
	ld (textio_PrintWrappedText.spsv),iy
	ld a,(textio_clearcolor)
	ld l,a
	push hl
	call gfx_SetColor
	pop hl
	ld hl,(textio_linespacing)
	push hl
	ld hl,(iy+6)
	push hl
	ld hl,(iy+12)
	push hl
	ld hl,(iy+9)
	push hl
	call gfx_FillRectangle
	pop bc
	pop bc
	pop bc
	pop bc
	ld hl,textio_PrintWrappedText.exit
	jr textio_PrintWrappedText.entry

;-------------------------------------------------------------------------------
;void textio_PrintWrappedText(char *text, uint24_t width, uint24_t xPos, uint8_t yPos);
textio_PrintWrappedText:
	ld iy,0
	add iy,sp
	ld (.spsv),iy
	ld hl,(iy+6)
	ld de,(iy+9)
	or a,a
	sbc hl,de
	ld (.max_width),hl
	ld hl,(iy+3)
	push hl
	call gfx_GetStringWidth
	pop bc
	ld bc,(.max_width)
	call ti._idivu
	ld a,(textio_linespacing)
	ld h,a
	mlt hl
	ld (.max_height),hl
	ld l,255
textio_clearcolor:=$-1
	push hl
	call gfx_SetColor
	pop hl
	ld hl,0
.max_height:=$-3
	push hl
	ld hl,0
.max_width:=$-3
	push hl
	ld iy,(.spsv)
	ld hl,(iy+12)
	push hl
	ld hl,(iy+9)
	push hl
	call gfx_FillRectangle
	pop bc
	pop bc
	pop bc
	pop bc
	ld hl,.nextline
.entry:
	ld (.smc),hl
	ld iy,(.spsv)
	ld hl,(iy+3)
	push hl
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
	pop hl
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
	ld sp,0
.spsv:=$-3
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
;void textio_SetTextConfig(uint8_t text_y_margin, uint8_t line_spacing, uint8_t clear_color);
textio_SetTextConfig:
	ld iy,0
	add iy,sp
	ld a,(iy+3)
	ld (textio_margin_y),a
	ld a,(iy+6)
	ld (textio_linespacing),a
	ld a,(iy+9)
	ld (textio_clearcolor),a
	ret


;-------------------------------------------------------------------------------
;void textio_LetterInput(char buffer[], uint8_t buffer_size, uint24_t width, uint24_t xPos, uint8_t yPos);
textio_LetterInput:
	ld l,0
.fg:=$-1
	push hl
	call gfx_SetTextFGColor
	pop bc
	ld iy,0
	add iy,sp
	ld (.svsp),iy
	ld hl,(iy+3)
	ld (.buffer),hl
	ld (.start),hl
	ld (.realstart),hl
	push hl
	pop de
	inc de
	xor a,a
	ld (hl),a
	ld bc,(iy+6)
	add hl,bc
	ld (.end),hl
	sbc hl,bc
	ldir
	or a,a
	sbc hl,hl
	ld (.current_scroll),hl
	ld l,(iy+15)
	push hl
	ld hl,(iy+12)
	push hl
	ld de,(iy+9)
	add hl,de
	ld (.end_x),hl
	push hl
.draw:
	ld hl,(.start)
	push hl
	call textio_PrintTruncatedStringXY
	pop hl ;pop the string
	ld l,240
.cursor:=$-1
	push hl
	call gfx_SetColor
	pop bc
	ld hl,9
	push hl
	push hl
	call gfx_GetTextY
	push hl
	call gfx_GetTextX
	push hl
	call gfx_Rectangle ;draw the cursor outline
	pop hl
	pop hl
	pop hl
	pop hl
	ld hl,textio_overtypes
	ld de,0
.mapno:=$-3
	add hl,de
	ld l,(hl)
	push hl
	call gfx_PrintChar ;draw the cursor character
	pop hl
.key:
	call ti.GetCSC
	or a,a
	jr z,.key
	ld hl,0
.start:=$-3
	cp a,56
	jp z,.delete
	cp a,15
	jp z,.cancel
	cp a,9
	jp z,.enter
	cp a,54
	jp z,.prevmap
	cp a,48
	jp z,.nextmap
	jr nc,.key
	sub a,10
	jr c,.key
	ld de,0
	ld e,a
	ld hl,textio_keymap_A
.keymap:=$-3
	add hl,de
	push hl
	ld hl,0
.buffer:=$-3
	ld de,0
.end:=$-3
	or a,a
	sbc hl,de
	add hl,de
	ex hl,de
	pop hl
	jr nc,.key
	ld a,(hl)
	ldi
	ld (.buffer),de
	or a,a
	sbc hl,hl
	ld l,a
	xor a,a
	ld (de),a
	push hl
	call gfx_GetCharWidth
	pop hl
	ld l,a
	ld bc,0
.current_scroll:=$-3
	add hl,bc
	ld (.current_scroll),hl
	ld bc,0
.end_x:=$-3
	or a,a
	sbc hl,bc
	jp c,.draw ;if we're not at the max X coord we don't need to scroll the string
	ld hl,(.start)
	inc hl
	ld (.start),hl
	jp .draw
.prevmap:
	ld a,(.mapno)
	or a,a
	jr nz,.next
	ld a,2
.next:
	dec a
.cont2:
	ld hl,textio_keymaps
	ld (.mapno),a
	ld de,(.mapno)
	add hl,de
	add hl,de
	add hl,de
	ld hl,(hl)
	ld (.keymap),hl
	jp .draw
.delete:
	ld de,(.buffer)
	dec de
	ld hl,0
.realstart:=$-3
	or a,a
	sbc hl,de
	jp z,.key
	ld (.buffer),de
	ld a,(de)
	or a,a
	sbc hl,hl
	ld l,a
	push de
	push hl
	call gfx_GetCharWidth
	pop bc
	pop de
	ld c,a
	ld hl,(.current_scroll)
	sbc hl,bc
	jr c,.zeroit
	xor a,a
.setchar:
	ld (de),a
	jp .draw
.zeroit:
	xor a,a
	sbc hl,hl
	ld (.current_scroll),hl
	ld hl,(.realstart)
	ld (.start),hl
	jr .setchar
.cancel:
	ld (hl),0
.enter:
	ld sp,0
.svsp:=$-3
	ret
.nextmap:
	ld a,(.mapno)
	cp a,2
	jr z,.zero
	inc a
	jr .cont2
.zero:
	xor a,a
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


