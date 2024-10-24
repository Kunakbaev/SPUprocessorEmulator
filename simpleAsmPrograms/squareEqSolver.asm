in
in
in  ; input 3 coefficients (a, b, c) of quadratic equation

call findDiscAndPutToDX:    ; finding discriminant

; push DX
; out

push AX
push 0
je linearCase:

push DX
push 0
jb negativeDiscCase:

push DX
push 0
je oneSolutionCase:

push DX
push 0
ja twoSolutionsCase:

; after those 4 cases program should jump to one exact label

linearCase:
    call linearCaseFunc:
    jmp end:

negativeDiscCase:
    call negativeDiscFunc:
    jmp end:

oneSolutionCase:
    call oneSolutionFunc:
    jmp end:

twoSolutionsCase:
    call twoSolutionsCaseFunc:
    jmp end:

end:
halt




; -------------------------     FUNCTIONS      -------------------------------

jmp FUNCS_SAFETY_LABEL:

;;==============================
;; input:     stack - a, b, c
;; output:    d - discriminant
;; destroyed: AX, BX, CX
;;==============================
findDiscAndPutToDX:
    pop CX
    pop BX
    pop AX

    push BX
    push BX
    mul
    push 4
    push AX
    push CX
    mul
    mul
    sub
    pop DX
    ret

linearCaseFunc:
    ; push 229
    ; out

    push BX
    push 0
    je constantCase:
    jmp elseUp:

    constantCase:
        push 100
        out
        push CX
        push 0
        je infinitlyManySolutions:
        jmp else:

        infinitlyManySolutions:
            push 100000000  ; infinitly many sols
            out
            ret
        else:
            push 0 ; no solutions at all
            out
        ret
    elseUp:

    push 0
    push CX
    sub
    push BX
    div

    push 1
    out     ; printing that we have only 1 solution

    out     ; printing solution

    ret



negativeDiscFunc:
    ; push 230
    ; out

    push 0  ; printing that we don't have any solutions
    out
    ret




oneSolutionFunc:
    ; push 228
    ; out

    push 0
    push BX
    sub

    push AX
    push 2
    mul

    div

    push 1  ; printing that we have 1 solution only
    out

    out     ; printing solution

    ret




twoSolutionsCaseFunc:
    push 2  ; printing that we have 2 solutions
    out

    ; bruh, copypaste, negative solution
    push 0
    push BX
    sub
    push DX
    sqrt
    sub
    push AX
    push 2
    mul
    div

    out

    ; positive solution
    push 0
    push BX
    sub
    push DX
    sqrt
    add
    push AX
    push 2
    mul
    div

    out

    ret


FUNCS_SAFETY_LABEL:

