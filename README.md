# Dot-VM

## Description

The goal of this project was to learn a bit more about how things function at a low level, by constructing a small assembly like programming language and executing it.

The parser and interpreter are handwritten in C++.

## Language description

### **Syntax**
Statements are evaluated on a line by line basis. \
Every line follows this rough scheme:
```assembly
[instruction] [argument] ; Comment
```
Multiple arguments are separated by commas :
```assembly
[instruction] [argument 1],...,[argument n] ; Comment
```
Or with no arguments :
```assembly
[instruction] ; Comment
```


### **Arguments**
An argument can either be:
-	A 64 bit signed integer \
	Please note : **Expressions are not supported ! Inserting one will cause a syntax error !**
-	A string, which will be represented by a pointer
-	A register

Arguments are passed to instructions through the syntax seen above, but lots of instructions are overloaded to get their arguments from the stack. \
In that case, arguments are popped from the stack in order left to right.

### **Numbers**
Numbers are interpreted using the `strtoll` C function, meaning they can use the following prefixes:
-	Numbers starting by `0` are base-8 :
	`02375` &#8594; `1277`
-	Numbers starting by `0x` or `0X` are base-16 :
	`0xF421` &#8594; `62497`
-	All other numbers are base-10

### **Strings**
Strings are any character wrapped in double quotation marks. \
They support escape sequences in the form of : `\a`, `\b`, `\f`, `\n`, `\r`, `\t`, `\v`, `\"`, `\?` and `\\`, and they are NULL terminated.

### **Registers**
Registers are 64 bit wide variables for storing data, there are currently 5:
```assembly
reg  ; General purpose register
eax  ; Arithmetic register
sp   ; Pointer to top of the stack
mem  ; Pointer to start of memory block
void ; Blackhole register
``` 
Values of registers are acessed through the dollar `$` operator:
```assembly
reg  ; Pointer to 'reg'
$reg ; Value of 'reg'
```

### **Labels**
Labels are use to jump around the program during execution. \
They are defined with the colon instruction and jumped to with the jump instruction.
```assembly
: label
; Defines the label 'label'
; /!\ Do not forget the space

jump label
; Jumps to the label 'label'
```
There is one reserved label, `$$` which denotes the last place a `call` instruction was invoked, and one obligatory label, `.start`, which is where the interpreter starts running the program. \
More will be explained in the **Functions** header.

### **Instructions**

The language is (currently) composed of these 21 instructions :
-	Stack manipulation : `push`, `pop` and `swap`
	```assembly
	push [value...]
	; Pushes a/multiple value to the stack
    
		push 1968, 12 ; Pushes 1968 to the stack, then pushes 12
		push $reg     ; Pushes the value of register 'reg' to the stack
  
	pop
	pop [register]
	pop [register], [offset]
	; Removes a value from the stack at a given offset and optionally store it somewhere

		pop         ; Removes the top of the stack
		pop void, 2 ; Removes the third element from the stack
		            ; This means `pop` is equivalent to `pop void, 0`
		
		pop reg ; Removes the top of the stack and stores it in register 'reg'
	
	swap
	swap [value]
	; Swaps the top of the stack with the given index

		swap 2
		;        ╭─────┐ 
		; [5, 8, 9, 3, 4] <- Top
		;        └─────╯

		swap
		; No value means swapping the two top elements on the stack, or the equivalent of `swap 1`
	```
-	Arithmethic instructions : `add`, `sub`, `mul`, `div` and `mod`
	```assembly
	add
	add [value 1], [value 2]
	; Adds two values together, and pushes the result to the stack
    
		add            ; Pops the first and second values from the stack and adds them together
		add $reg, $eax ; Adds the value from 'reg' and 'eax'
	
	sub
	sub [value 1], [value 2]
	; Substracts the first value by the second value, and pushes the result to the stack
	; Works in the same way as the 'add' instruction

	mul
	mul [value 1], [value 2]
	; Multiplies two values together, and pushes the result to the stack
	; Works in the same way as the 'add' instruction
	
	div
	div [value 1], [value 2]
	; Divides the first value by the second value, and pushes the result to the stack
	; Works in the same way as the 'add' instruction
	
	mod
	mod [value 1], [value 2]
	; Divides the first value by the second value, and pushes the remainder to the stack
	; Works in the same way as the `div` instruction

	```
-	Bitwise instructions : `and`, `or`, `xor`, `not`, `lshift`, `rshift`
	```assembly
	and
	and [value 1], [value 2]
	; ANDS two values together, and pushes the result to the stack

		and      ; Pops the first and second value from the stack, and ANDs them
		and 7, 4 ; Pushes 4 to the stack
	
	or
	or [value 1], [value 2]
	; ORs two values together, and pushes the result to the stack
	; Works in the same way as the 'and' instruction

	xor
	xor [value 1], [value 2]
	; XORs two values together, and pushes the result to the stack
	; Works in the same way as the 'xor' instruction

	not
	not [value]
	; Flips the bits of a value, and pushes the result to the stack

		not      ; Pops the top of the stack, flips it and pushes it back
		not $eax ; Flips the bits of 'eax'
	
	lshift [value]
	lshift [value 1], [value 2]
	; Shifts the bit of a value to the left by the given amount, and pushes the result to the stack

		lshift 2     ; Pops the top of the stack, left shifts it by two and pushes it back
		lshift 12, 3 ; Pushes 96 to the stack
	
	rshift [value]
	rshift [value 1], [value 2]
	; Shifts the bit of a value to the right by the given amount, and pushes the result to the stack
	; Works in the same way as the 'lshift' instruction
	```
-	Register manipulation : `mov`
	```assembly
	mov [register], [value]
	; Moves a value to a register
		mov reg, $eax ; Moves the value of 'eax' to 'reg'

		mov sp, 20    ; Changes the value of the top of the stack to 20
		; Equivalent to : `pop` and `push 20`

		push $mem, 16
		add
		mov $sp, "String" 
		; Moves a pointe to a string to the second index of memory
	```
-	Runtime manipulation : `:`, `jump`, `ifeq` and `call`
	```assembly
	: [label]
    ; Defines a label to be jumped to
    
    	: label ; Defines the label 'label'
    
	jump [label]
	; Jumps to a label
	; Throws an error if the label doesn't exists

		jump label ; Jumps to the label 'label'
	
	ifeq [operator]
	ifeq [operator], [value], [value]
	; Compares two values together based on the given operator
	; Skips the next instruction if the condition is false

		ifeq le, $eax, 10
		; Will only execute the next instruction if the value of 'eax'
		; is less or equal to 10

		; Valid comparisons are 'eq','lt','le','gt','ge','ne'
		; Which translates to : 
		; 'equal', 'less than', 'less or equal',
		; 'greater than', 'greater or equal' and 'not equal'
		
		ifeq eq
		; Pops two values from the stack and only execute the next instruction if they are equal

	call [label]
	; Jumps to a label and sets the '$$' label to the next instruction
	; Call is functionally equivalent to 'jump', but it is used to invoke functions
	; More is explained in the **Functions** header

		call func
		push 10
		; Jumps to label func, and set label $$ to the next instruction
		; In this case, a push instruction
	```
-	Other : `print`, `syscall`
	```assembly
	print [(string | value)...]
	; Will print the given characters to stdout
	
		print "Hello, world!\n" ; Prints "Hello, World!" and a line feed
		print "The value of eax is ", $eax, "\n" ; Concatenation of values
	
	syscall
	syscall [number of args], [syscall id]
	; Calls the given syscall
	; Arguments for the syscall are taken from the stack
		
		; This exits the program manually
		push 0
		syscall 1, 60
	```

### **Functions**
Functions are defined at the start of the program as labels. \
They return by a jump to `$$`, and are called with the `call` instruction.

Arguments are traditionally passed through the stack.

Here is an exemple function :

```assembly
: funcThreeAdd ; Defines label 'funcThreeAdd'
	; Body of the function, here it adds the first, second and third value from the stack
	
	; Add the first and second value
	add
	
	; Adds the result with the third value
	add
	
	; Jump back to the call statement
	jump $$

: .start ; Program starts here
	push 12, 60, 0xF
	
	call funcThreeAdd ; Jumps to function
	; $$ now points here
	
	print "The value is :", $sp, "\n" ; This should print '87'
	
```
As the `$$` label represents returning, jumping to it from the main program will thus exit the program.

Note: Traditionally, the exit code is set with `reg` and exit is handled automatically by the compiler, however this can be disabled through a flag and done manually through a syscall.

## The Preprocessor
The preprocessor is the first thing that will interact with the code after reading.

It allows for abstraction through the use of macros. \
It is interacted with through these preprocessor directives:

### **Built-in directives**
-	`#include` directive, will insert the given files body, relative to the path of the current file. \
	The `#include` directive won't include the same file multiple times, however this can be circumvented using the `#include_recursive` directive.
	```asm
	#include "FILE"
	#include_recursive "FILE"
	```
-	`#define` directive, will define the given single-line macro, and every subsequent use of the macro will replace it with its contents.
	```asm
	#define MACRO VALUE
	```
-	`#macro` directive, will define the given multi-line macro.
	```asm
	#macro MACRO
		; BODY
	#endmacro
	```
-	`#ifdef`/`ifndef` directive, will only insert its body if the given macro is defined / not defined
	```asm
	#ifdef MACRO
		; BODY
	#endif
	```
-	`#ifis` and `#elif`/`#ifnis` and `#elnif` directives, will only insert their body if the macro exists and is equal to the given value. \
	Note this only works with single line macros.
	```asm
	#ifis MACRO VALUE
		; BODY
	#elif MACRO VALUE
		; BODY
	#endif
	```
-	`#undef` directive, removes the definition of a macro.
	```asm
	#undef MACRO
	```
-	`#error` directive, throws a compilation error with the given message.
	```asm
	#error "MESSAGE"
	```

### **Macros**
Macros are designed to streamline programming in dotvm, they acts as functions without requiring the use of the callstack, though they come at the cost of additional size. \
As seen before, they are defined using the `#define` and the `#macro` directive.

Built-in macros:
-	`__FILE__`: Name of the current file
-	Only defined in macros:
	-	`__NUM_ARGS__`: Number of arguments given
	-	`ARG_n`: Argument with index n, where n is a number between 0 and `__MAX_MACRO_ARGS__`
	-	`__MAX_MACRO_ARGS__`: Maximum number of arguments which can be given to a macro.
		Default is 16, but can be adjusted through a compiler flag.
	-	`__MACRO_INDEX__`: Unique index given to a macro.
	-	`__MACRO_ID__`: Unique identifier given to each expansion of a macro. \
		This means that between two expansions of the same macro, `__MACRO_INDEX__` will be equal but not `__MACRO_ID__`.

Macros can be expanded implicitely, if they are on their own, or explicitely, if you need to compose things together. \
It that case, `#()` will be used :
```asm
#define define_label : label#(ARG_1) ; (value)
; labelARG_1 would be considered a single word and wouldn't work

define_label 1
; Gets expanded to:
: label1
```

Multi-line macro definition:
```asm
#macro jifeq ; (label, word, val1, val2)
	#ifnis __NUM_ARGS__ 4
		#error "Wrong number of arguments given to jifeq"
	#endif
	
	ifeq ARG_2, ARG_3, ARG_4
	jump ARG_1
#endmacro

jifeq somelabel, eq, $reg, 0

; Gets expanded to: 
; (trailing spaces are removed)
#ifnis 4 4
#error "Wrong number of arguments given to jifeq"
#endif

ifeq eq, $reg, 0
jump somelabel

; Further directives are processed:
ifeq eq, $reg, 0
jump somelabel
```


