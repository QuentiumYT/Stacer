#!/bin/bash

rm -rf build
rm -rf release
rm -f translations/*.qm
rm -rf .flatpak-builder
rm -rf repo
rm -rf *.flatpak

rm -rf rpm/BUILD/
rm -rf rpm/BUILDROOT/
rm -rf rpm/*RPMS/
rm -rf rpm/SOURCES/
rm -f debug*.list
rm -f elfbins.list

find . -name CMakeFiles -exec rm -rf {} +
find . -name '*_autogen' -exec rm -rf {} +

find . -name cmake_install.cmake -delete
find . -name CMakeCache.txt -delete
find . -name Makefile -delete
