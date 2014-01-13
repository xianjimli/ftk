FTK == Funny ToolKit (Write just for fun)
=====================================================
(You can use it in commercial software, but please let me know.)

How to build ftk from svn source code?
=====================================================
o download:
  svn checkout http://ftk.googlecode.com/svn/trunk/ ftk-read-only

o configure:
  cd ftk-read-only
  ./autogen.sh
  ./configure --with-backend=linux-x11:320x480
  (run ./configure --help for more options)

o build:
  make

o install(need root)
  make install

o run:
  cd src/demos
  ./demo_button
 
 or(make install first):
   desktop

How to build and run ftk on target board.
=====================================================
see tools/cross-build/README

Where can I get more information:
=====================================================
DOC:  docs/* 
BLOG: http://www.limodev.cn/blog/archives/tag/ftk
WIKI: http://code.google.com/p/ftk/wiki/
Google Group: https://groups.google.com/group/funnytoolkit
Still have problems please send email to xianjimli@gmail.com

