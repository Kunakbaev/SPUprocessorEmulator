
MATRIX_SIZE   = 101
CIRCLE_RADIUS = MATRIX_SIZE // 4

# CRINGE:
code = f"""



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

        push {CIRCLE_RADIUS ** 2} ; ({MATRIX_SIZE} / 2) ** 2
        jb IF_isInCircle: ; TODO: change to jbe (<=)?
        jmp else:
        IF_isInCircle:
            push 255      ; cell is painted white
            jmp endOfIf:
        else:
            push 0        ; cell is painted black
        endOfIf:

        push {MATRIX_SIZE}     ; size of video matrix
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
        push {MATRIX_SIZE}
    jb cycleY:

    push AX
    push 1
    add
    pop AX

    push AX
    push {MATRIX_SIZE}
jb cycleX:

draw
halt




jmp FUNCS_SAFETY_LABEL:     ; we don't want main program to execute function's code without first calling it

addSquareDistanceToStack:
    push {MATRIX_SIZE // 2}      ; center of circle
    sub
    pop CX
    push CX
    push CX
    mul
    ret

FUNCS_SAFETY_LABEL:



"""

with open("program.asm", "w") as f:
    f.write(code)
    f.close()
