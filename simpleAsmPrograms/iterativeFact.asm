push 1
pop AX
push 1
pop BX

cycleStart:
    push AX
    push BX             ;       i am comment
    mul
    pick
    pop BX

    push AX
    push 1
    add
    pop AX

    push AX
    push 7
jb cycleStart:

push BX
out
halt
