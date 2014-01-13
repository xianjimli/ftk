#!/bin/bash

for f in idl/*.idl;
do
	echo $f
	./luagen $f
done

./luagen idl/ftk_typedef.idl 1

mv -fv lua_ftk*.h  ../../script_binding/lua/
mv -fv lua_ftk*.c  ../../script_binding/lua/

