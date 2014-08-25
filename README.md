FTK == Funny ToolKit 
=====================================================
(You can use it in commercial software, but please let me know.)

## How to build ftk from svn source code?
---------------------
* download:
>  git clone https://github.com/xianjimli/ftk.git

* configure:
>  cd ftk

>  ./autogen.sh

>  ./configure --with-backend=linux-x11:320x480

>  (run ./configure --help for more options)

* build:
> make

* install(need root)
>  make install

* run:
>  cd src/demos

>  ./demo_button
 
 or(make install first):
>   desktop

How to build and run ftk on target board.
=====================================================
see tools/cross-build/README

Where can I get more information:
=====================================================
DOC:  docs/* 

BLOG: http://www.limodev.cn/blog/archives/tag/ftk

WIKI: http://code.google.com/p/ftk/wiki/

Forum: http://funnytoolkit.freeforums.org/index.php


Still have problems please send email to xianjimli@hotmail.com


