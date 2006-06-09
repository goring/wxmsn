#!/bin/sh
install -d po/test
cp po/*.gmo po/test
for file in po/test/*.gmo;
do
mv $file po/test/`basename $file .gmo`.mo
done
