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

syntax match dotvmNumber '(0(x|X)\x\+)|(0\o\+)|(\d\+)'
syntax match dotvmLiteral '[a-zA-Z0-9_\-.@]'

syntax region dotvmComment start=';' end='$' contains=dotvmTodo
syntax region dotvmString start=/\v"/ skip=/\v\\./ end=/\v"/
syntax region dotvmDirective start=/\v#/ end=/\v\ |$/

syntax keyword dotvmInstruction push pop add sub mul div mod and or xor not lshift rshift mov : jump ifeq call print syscall nexgroup=dotvmNumber,dotvmLiteral,dotvmString

let b:current_syntax = "dotvm"

highlight default link dotvmTodo Todo
highlight default link dotvmComment Comment

highlight default link dotvmInstruction Statement
highlight default link dotvmDirective Directory

highlight default link dotvmNumber Constant
highlight default link dotvmString String
highlight default link dotvmLiteral Type

