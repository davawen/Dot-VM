" Vim syntax file
" Language: DotVM
"
" Usage: put this in your $MYVIMRUNTIME/syntax directory
" And add this to your vimrc:
" autocmd BufRead,BufNewFile *.vm set filetype=dotvm

if exists("b:current_syntax")
	finish
endif


syntax keyword dotvmTodo contained TODO FIXMENOTE
syntax match dotvmComment ';.*$' contains=dotvmTodo

syntax match dotvmNumber '(0(x|X))\=\d\+'
syntax match dotvmLiteral '[a-zA-Z0-9$]'
syntax region dotvmString start='"' end='"' contained

syntax keyword dotvmInstruction push pop add sub mul div mod and or xor not lshift rshift mov jump ifeq call print syscall nexgroup=dotvmNumber,dotvmLiteral,dotvmString

let b:current_syntax = "dotvm"

highlight def link dotvmTodo Todo
highlight def link dotvmComment Comment

highlight def link dotvmInstruction Statement

highlight def link dotvmNumber Constant
highlight def link dotvmString Constant
highlight def link dotvmLiteral Type
