ftkdatadir=$(datadir)/ftk/base/data

ftkdata_DATA=
if FONT_ENGINE_FREETYPE
ftkdata_DATA+=$(srcdir)/font.ttf
endif

if FONT_ENGINE_DEFAULT
ftkdata_DATA+=$(srcdir)/gb2312.fnt
endif

if ENABLE_GPINYIN
ftkdata_DATA +=$(srcdir)/gpinyin.dat  $(srcdir)/gpinyin_user.dat
else
ftkdata_DATA +=$(srcdir)/fhw.bin  $(srcdir)/py.bin  $(srcdir)/wbx.bin
endif

EXTRA_DIST=font.ttf gb2312.fnt fhw.bin  py.bin  wbx.bin gpinyin.dat gpinyin_user.dat
