#!/bin/bash

STRIP=${TARGET_CC/gcc/strip}

echo "Copy files to dist directory..."
rm -rf ${DIST}
mkdir -p ${DIST}
cp -rf ${STAGING}/* ${DIST}/.

cd ${DIST}
#####################################################
echo "Remove noused files..."
find -name man      | xargs rm -rf
find -name \*config | xargs rm -rf
find -name aclocal  | xargs rm -rf
find -name include  | xargs rm -rf

for EXT in a h la
do 
	find -name \*.${EXT} | xargs rm -rf
done

######################################################
echo "Strip($STRIP)..."
for f in ${DIST}${PREFIX}"/lib/"*.so.*
do
	$STRIP $f
done

for f in ${DIST}${PREFIX}"/bin/"*
do
	$STRIP $f
done

echo "Make tar..."
tar czf ${DIST}"/ftk-dist.tar.gz" "."${PREFIX}

echo Distribute package is: ${DIST}"/ftk-dist.tar.gz"

