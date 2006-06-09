#!/bin/sh
tar xvfz mac-bundle.tar.gz
rm -fr wxMsn.app/Contents/MacOS/*
cp src/wxmsn wxMsn.app/Contents/MacOS/
cp -R src/resources wxMsn.app/Contents/MacOS/
