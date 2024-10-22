
push 7      ; argument for factorial func
call recursiveFactorial:




jmp FUNCS_SAFETY_LABEL:     ; we don't want main program to execute function's code without first calling it

recursiveFactorial:     ;       receives arguments from stack of vars, puts result in AX
    pop AX
    push AX
    push 1

    je tailRecursionCheck:    ; check if argument (n) is == 1, if so
    jmp else:
    tailRecursionCheck:
        ret
    else:

    push AX     ; saving current value of n in stack, it will be on top of stack after return from recursive call

    push AX
    push 1
    sub         ; saving argument equal to AX - 1 (n - 1) for next recursive call in stack of vars

    call recursiveFactorial: ; recursive call

    push AX
    mul
    pop AX

    ret


FUNCS_SAFETY_LABEL:
