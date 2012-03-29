#!/bin/bash
cd ~/git/RaptorPaint


cd ./RaptorPaint
rm -rf ./*.app
rm ./moc_*.cpp
rm ./qrc_*.cpp
rm ./*.o
rm ./Makefile
rm ./ui_*.h

cd ../RaptorPaintServer
rm -rf ./*.app
rm ./moc_*.cpp
rm ./qrc_*.cpp
rm ./*.o
rm ./Makefile
rm ./ui_*.h
