global inb, inbl

; inb - returns a byte from the given I/O port
; stack: [esp + 4] The address of the I/O port
; 	 [esp	 ] The return address
inb:
	mov dx, [esp + 4]	; move the address of the I/O port to dx
	in  al, dx		; read a byte from the I/O port and store it in the al register
	ret

inbl:
    mov dx, [esp + 4]
    in eax, dx
    ret
