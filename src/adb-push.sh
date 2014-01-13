#!/bin/bash

ANDROID_ROOT=/work/android/froyo/

adb shell rm -r /data/ftk
adb shell mkdir /data/ftk
adb shell mkdir /data/ftk/bin
adb shell mkdir /data/ftk/base
adb shell mkdir /data/ftk/base/theme
adb shell mkdir /data/ftk/base/data
adb shell mkdir /data/ftk/base/testdata
adb shell mkdir /data/ftk/base/theme/default

DIR=/data/ftk/base/theme/default/
for f in ../theme/default/*.png ../theme/default/*.xml;
do
	echo adb push $f $DIR
	adb push $f $DIR
done

DIR=/data/ftk/base/data/
for f in ../data/unicode.fnt 
do
	echo adb push $f $DIR 
	adb push $f $DIR
done

DIR=/data/ftk/base/testdata/
for f in ../testdata/*.png ../testdata/*.jpg;
do
	echo adb push $f $DIR 
	adb push $f $DIR
done

DIR=/data/ftk/bin/
for f in $ANDROID_ROOT/out/target/product/generic/system/bin/demo_* demos/t*.xul \
	$ANDROID_ROOT/out/target/product/generic/system/lib/libjpeg.so
do
	echo adb push $f $DIR 
	adb push $f $DIR
done

