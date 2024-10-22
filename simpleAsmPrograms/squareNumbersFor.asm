push 0
pop AX

cycleStart:
push AX
push AX
mul
out
push AX
push 1
add
pop AX
push AX
push 101
jb cycleStart:
halt
