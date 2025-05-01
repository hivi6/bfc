# bfc

Brain Fuck Compiler

## Environment

Has a memory tape of 30,000 length, with each memory block of 1 byte.
There exists a pointer that points to this memory tape.

## Tokens

```
>	:= increases pointer value by 1
<	:= decreases pointer value by 1
+	:= increases value of memory block pointed by the pointer by 1
-	:= decreases value of memory block pointed by the pointer by 1
[	:= start of a while loop, enters a while loop is memory block is non-zero
]	:= end of a while loop, jumps back to [ if memory block is non-zero
,	:= input a character from stdin to memory block
.	:= output a character to stdout from memory block(ascii)
```

