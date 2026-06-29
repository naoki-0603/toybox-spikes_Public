@echo off

cmake -S . -B build-msvc -G "Visual Studio 17 2022" -A x64

PAUSE