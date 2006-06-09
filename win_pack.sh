#!/bin/sh
mkdir wxmsn-win32
cp -r src/resources wxmsn-win32/ 2> /dev/null
cp src/wxmsn.exe wxmsn-win32/
find wxmsn-win32/ -name .svn -type d -exec rm -fr '{}' \;
zip -r wxmsn-win32.zip wxmsn-win32/
