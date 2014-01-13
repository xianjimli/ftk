all:
	gcc -Wall -g unicode.c -o unicode_test
	gcc -Wall -g -DWITHOUT_FTK -I /usr/include/freetype2 fontextract.c -DHAS_FONT_DATA_SAVE fontdata.c -lfreetype -o fontextract
	gcc -ansi -Wall -g -DWITHOUT_FTK -DFONT_DATA_TEST -DHAS_FONT_DATA_SAVE fontdata.c -o fontdata_test
clean:
	rm -f fontextract *test

