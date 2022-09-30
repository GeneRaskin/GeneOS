global outb, outbl

outb:
	mov al, [esp + 8]	; move the data to be sent into the al register
	mov dx, [esp + 4]	; move the address of the I/O port into the dx register
	out dx, al		; send the data to the I/O port
	ret

outbl:
    mov eax, [esp + 8]
    mov dx, [esp + 4]
    out dx, eax
    ret
