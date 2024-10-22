



push 0
pop AX ; intital value for x coord

cycleX:
    push 0
    pop BX ; intital value for y coord

    cycleY:
        push AX
        call addSquareDistanceToStack:
        push BX
        call addSquareDistanceToStack:
        add

        push 625 ; (101 / 2) ** 2
        jb IF_isInCircle: ; TODO: change to jbe (<=)?
        jmp else:
        IF_isInCircle:
            push 255      ; cell is painted white
            jmp endOfIf:
        else:
            push 0        ; cell is painted black
        endOfIf:

        push 101     ; size of video matrix
        push AX
        mul
        push BX
        add
        pop CX      ; saves ram index, we have 2 counters, so 2 registers are already taken
        pop [CX]

        push BX
        push 1
        add
        pop BX

        push BX
        push 101
    jb cycleY:

    push AX
    push 1
    add
    pop AX

    push AX
    push 101
jb cycleX:

halt




jmp FUNCS_SAFETY_LABEL:     ; we don't want main program to execute function's code without first calling it

addSquareDistanceToStack:
    push 50      ; center of circle
    sub
    pop CX
    push CX
    push CX
    mul
    ret

FUNCS_SAFETY_LABEL:



