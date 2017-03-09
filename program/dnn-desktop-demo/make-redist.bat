@echo off

set APP_EXE=crowdsource-video-experiments-on-desktop.exe
set APP_BAT=crowdsource-video-experiments-on-desktop.bat
set TARGET_FILE=crowdsource-video-experiments-on-desktop-windows.zip

:: All dirs relative to location of this batch file:
set TARGET_DIR=.\tmp
set REDIST=.\redist
set BIN=.\bin
set SRC=.\src

@echo Search for Qt...
:: Search in default installation paths for different Qt versions.
:: It assumes that only one version is installed on build machine.
set QT_MINGW_5_8_32=c:\Qt\5.8\mingw53_32
set QT_MSVC_5_7_2015=c:\Qt\5.8\msvc2015_64
if exist %QT_MINGW_5_8_32% (
	set QT_DIR=QT_MINGW_5_8_32
	set QT_TYPE="mingw"
	goto qt_found
)
if exist %QT_MSVC_5_7_2015% (
	set QT_DIR=%QT_MSVC_5_7_2015%
	set QT_TYPE="msvc"
	goto qt_found
)
:qt_found
set QT_LIBS=%QT_DIR%\bin
@echo Qt type is %QT_TYPE%
@echo Qt path is %QT_LIBS%


mkdir %REDIST%

@echo.
@echo Copy app files...
copy /Y %BIN%\%APP_EXE% %REDIST%
copy /Y %APP_BAT% %REDIST%
copy /Y %SRC%\app.conf.example.win %REDIST%\app.conf
copy /Y "%BIN%\*.README.md" %REDIST%

@echo.
@echo Copy Qt libs...
copy /Y %QT_LIBS%\Qt5Core.dll %REDIST%
copy /Y %QT_LIBS%\Qt5Gui.dll %REDIST%
copy /Y %QT_LIBS%\Qt5Svg.dll %REDIST%
copy /Y %QT_LIBS%\Qt5Widgets.dll %REDIST%

@echo.
@echo Copy C runtime...
if %QT_TYPE% == "mingw" (
	copy /Y "%QT_LIBS%\libstdc++-6.dll" %REDIST%
	copy /Y "%QT_LIBS%\libgcc_s_dw2-1.dll" %REDIST%
	copy /Y "%QT_LIBS%\libwinpthread-1.dll" %REDIST%
)
if %QT_TYPE% == "msvc" (
	@echo Don't forget about vc_redist.x64.exe or vc_redist.x86.exe!
	@echo You have to force user to install VC-redist when build with 
	@echo MS-related version of Qt. Download installer from here: 
	@echo http://www.microsoft.com/en-us/download/details.aspx?id=48145
	@echo or google for 'visual studio 2015 runtime'.
)

@echo.
@echo Copy Qt plugins...
mkdir %REDIST%\imageformats
mkdir %REDIST%\platforms
mkdir %REDIST%\iconengines
copy /Y %QT_DIR%\plugins\imageformats\qjpeg.dll %REDIST%\imageformats
copy /Y %QT_DIR%\plugins\imageformats\qsvg.dll %REDIST%\imageformats
copy /Y %QT_DIR%\plugins\platforms\qwindows.dll %REDIST%\platforms
copy /Y %QT_DIR%\plugins\iconengines\qsvgicon.dll %REDIST%\iconengines

@echo.
@echo Packing...
mkdir %TARGET_DIR%
set CUR_DIR=%~dp0
pushd %REDIST%
python -m zipfile -c %CUR_DIR%/%TARGET_DIR%\%TARGET_FILE% .
popd

@echo.
@echo Package created: %TARGET_DIR%/%TARGET_FILE%
@echo.
@echo Done
