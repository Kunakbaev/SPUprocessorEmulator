
push 10      ; argument for fibonacci func
call recursiveFibonacci:

push AX
out



jmp FUNCS_SAFETY_LABEL:     ; we don't want main program to execute function's code without first calling it

recursiveFibonacci:     ;       receives arguments from stack of vars, puts result in AX
    pop AX
    push AX
    push 3

    jb tailRecursionCheck:    ; check if argument (n) is <= 2 (< 3), if so
    jmp else:
    tailRecursionCheck:
        push 1
        pop AX
        ret
    else:

    push AX
    push 1
    sub         ; saving argument equal to AX - 1 (n - 1) for next recursive call in stack of vars

    push AX
    push 2
    sub

    call recursiveFibonacci: ; recursive call

    pop BX
    push AX
    push BX

    call recursiveFibonacci: ; recursive call

    push AX
    add
    pop AX

    ret


FUNCS_SAFETY_LABEL:
