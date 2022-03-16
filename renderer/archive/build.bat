@echo off

if not exist .\build mkdir .\build

set TARGET=renderer
set CFLAGS=/nologo /Od /Zi /Wall /WX /wd4668 /wd4100 /wd4820 /wd5045
set SRCS=win32_main.cpp
set LFLAGS=/incremental:no
set LIBS=kernel32.lib user32.lib gdi32.lib
set OUT_DIR=/Fo.\build\ /Fe.\build\%TARGET% /Fm.\build\
set INC_DIR=/I.\
set LIB_DIR=/LIBPATH:.\
set DEFINES=/D_CRT_SECURE_NO_WARNINGS

cl %CFLAGS% %INC_DIR% %SRCS% %OUT_DIR% %DEFINES% /link %LFLAGS% %LIB_DIR% %LIBS%
