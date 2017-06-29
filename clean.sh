#!/bin/bash

rm -rf build/*
rm -rf lib/*
rm -rf bin/p*

find . -name '.DS_Store'  -print -exec rm -rfv {} \;
find . -name '*~'  -print -exec rm -rfv {} \;
find . -name '#*'  -print -exec rm -rfv {} \;
find . -name '*.moos++'  -print -exec rm -rfv {} \;

#find . -name 'MOOSLog*'  -print -exec rm -rfv {} \;

