syntax on
" 语法高亮
 
"autocmd InsertLeave * se nocul
"autocmd InsertEnter * se cul
" 用浅色高亮当前行
 
set smartindent
" 智能对齐
 
set autoindent
" 自动对齐
 
set confirm
" 在处理未保存或只读文件的时候，弹出确认
 
set tabstop=4
" Tab键的宽度
 
set softtabstop=4
set shiftwidth=4
"  统一缩进为4
 
set noexpandtab
" 不要用空格代替制表符
 
set number
" 显示行号
 
set history=50
" 历史纪录数
 
set hlsearch
set incsearch
" 搜索逐字符高亮
 
set gdefault
" 行内替换
 
set encoding=utf-8
set fileencodings=utf-8,ucs-bom,shift-jis,gb18030,gbk,gb2312,cp936,utf-16,big5,euc-jp,latin1
" 编码设置

set fileformat=unix
" 设置文件格式
 
colorscheme default
" 设置颜色主题
 
set mouse=a
" use mouse
 
 
"set guifont=Menlo:h16:cANSI
" 设置字体
 
set langmenu=zn_CN.UTF-8
set helplang=cn
" 语言设置
 
"set cmdheight=2
" 命令行（在状态行）的高度，默认为1,这里是2
 
set ruler
" 在编辑过程中，在右下角显示光标位置的状态行
 
set laststatus=2
" 总是显示状态行
 
set showcmd
" 在状态行显示目前所执行的命令，未完成的指令片段亦会显示出来
 
set scrolloff=3
" 光标移动到buffer的顶部和底部时保持3行距离
 
set showmatch
" 高亮显示对应的括号
 
set matchtime=5
" 对应括号高亮的时间（单位是十分之一秒）
 
set autowrite
" 在切换buffer时自动保存当前文件
 
set wildmenu
" 增强模式中的命令行自动完成操作
 
set linespace=2
" 字符间插入的像素行数目
 
set whichwrap=b,s,<,>,[,]
" 开启Normal或Visual模式下Backspace键，空格键，左方向键，右方向键，Insert或replace模式下左方向键，右方向键跳行的功能。
 
filetype plugin indent on
" 分为三部分命令：file on, file plugin on, file indent on.分别表示自动识别文件类型，用文件类型脚本，使用缩进定义文件。
 
"==================自定义的键映射======================
 
vnoremap $1 <esc>`>a)<esc>`<i(<esc>
vnoremap $2 <esc>`>a]<esc>`<i[<esc>
vnoremap $3 <esc>`>a}<esc>`<i{<esc>
vnoremap $$ <esc>`>a"<esc>`<i"<esc>
vnoremap $q <esc>`>a'<esc>`<i'<esc>
vnoremap $e <esc>`>a"<esc>`<i"<esc>
" 括号自动生成
 
inoremap ( ()<ESC>i
inoremap ) <c-r>=ClosePair(')')<CR>
inoremap { {}<ESC>i
inoremap } <c-r>=ClosePair('}')<CR>
inoremap [ []<ESC>i
inoremap ] <c-r>=ClosePair(']')<CR>
"inoremap < <><ESC>i
"inoremap > <c-r>=ClosePair('>')<CR>
function ClosePair(char)
    if getline('.')[col('.') - 1] == a:char
        return "\<Right>"
    else
        return a:char
    endif
endf
"括号自动补全
