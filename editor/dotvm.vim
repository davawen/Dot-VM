" Vim syntax file
" Language: DotVM
"
" Usage: put this in your $MYVIMRUNTIME/syntax directory
" And copy this to your vimrc:
" autocmd BufRead,BufNewFile *.vm set filetype=dotvm

if exists("b:current_syntax")
	finish
endif

syntax keyword dotvmTodo contained TODO FIXMENOTE

syntax match dotvmLiteral '\<[a-zA-Z0-9_\-.]\+\>'

syntax match dotvmNumber '\(0\(x\|X\)\x\+\)\|\(0\o\+\)\|\(\d\+\)'
syntax match dotvmStringEscape '\\.' contained

syntax region dotvmComment start=';' end='$' contains=dotvmTodo
syntax region dotvmString start=/\v"/ skip=/\v\\./ end=/\v"/ contains=dotvmStringEscape
syntax region dotvmDirective start=/\v#/ end=/\v\ |$/

syntax keyword dotvmInstruction push pop add sub mul div mod and or xor not lshift rshift mov ifeq call print syscall
syntax keyword dotvmRegister reg rax rcx sp

syntax match dotvmLabelDecl '\(:\|\(jump\)\)\s\+[a-zA-Z0-9_\-.@]\+' contains=dotvmLabelInstruction,dotvmLabel
syntax match dotvmLabel '[a-zA-Z0-9_\-.]\+' contained
syntax match dotvmLabelInstruction ':\|\(jump\)' contained

let b:current_syntax = "dotvm"

highlight default link dotvmTodo Todo
highlight default link dotvmComment Comment

highlight default link dotvmDirective Directory

highlight default link dotvmInstruction Statement
highlight default link dotvmLabelInstruction Statement

highlight default link dotvmLiteral Constant
highlight default link dotvmLabel DiagnosticInfo 
highlight default link dotvmRegister Identifier

highlight default link dotvmNumber Number
highlight default link dotvmString String
highlight default link dotvmStringEscape Number
