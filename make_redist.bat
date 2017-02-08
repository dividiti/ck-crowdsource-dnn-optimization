@echo off

set APP_EXE=crowdsource-video-experiments-on-desktop.exe
set TARGET_FILE=crowdsource-video-experiments-on-desktop-windows.zip
set TARGET_DIR=.\tmp
set REDIST=.\redist
set BIN=.\bin
set SRC=.\src
set QT_DIR=c:\Qt\5.8\mingw53_32
set QT_LIBS=%QT_DIR%\bin
set PYTHON=

mkdir %REDIST%

@echo .
@echo Copy app files...
copy /Y %BIN%\%APP_EXE% %REDIST%
copy /Y %BIN%\*.qss %REDIST%
copy /Y %SRC%\app.conf.example %REDIST%\app.conf

@echo .
@echo Copy Qt libs...
copy /Y %QT_LIBS%\Qt5Core.dll %REDIST%
copy /Y %QT_LIBS%\Qt5Gui.dll %REDIST%
copy /Y %QT_LIBS%\Qt5Svg.dll %REDIST%
copy /Y %QT_LIBS%\Qt5Widgets.dll %REDIST%
copy /Y %QT_LIBS%\libwinpthread-1.dll %REDIST%
copy /Y "%QT_LIBS%\libstdc++-6.dll" %REDIST%
copy /Y %QT_LIBS%\libgcc_s_dw2-1.dll %REDIST%

@echo .
@echo Copy Qt plugins...
mkdir %REDIST%\imageformats
mkdir %REDIST%\platforms
copy /Y %QT_DIR%\plugins\imageformats\qjpeg.dll %REDIST%\imageformats
copy /Y %QT_DIR%\plugins\imageformats\qsvg.dll %REDIST%\imageformats
copy /Y %QT_DIR%\plugins\platforms\qwindows.dll %REDIST%\platforms

@echo .
@echo Packing...
mkdir %TARGET_DIR%
set CUR_DIR=%~dp0
pushd %REDIST%
python -m zipfile -c %CUR_DIR%/%TARGET_DIR%/%TARGET_FILE% .
popd

@echo .
@echo Package created: %TARGET_DIR%/%TARGET_FILE%
@echo .
@echo Done
