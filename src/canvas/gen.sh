#!/bin/bash
name=$1
up_name=$2

if [ "$up_name" == "" ]
then
	echo "$0 name upname"
	exit 0
fi

rm -rf $name
cp -rf dummy $name 

echo ":%s/dummy/$name/g" >s
echo ":%s/DUMMY/$up_name/g" >>s
echo ":wq" >>s

cd $name
for f in *.*;
do
	echo vim -s ../s $f
	vim -s ../s $f
	new_file=`echo $f|sed -e "s/dummy/${name}/g"`
	echo mv -f $f $new_file
	mv -f $f $new_file
done
