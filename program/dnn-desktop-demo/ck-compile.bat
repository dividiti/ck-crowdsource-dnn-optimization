
set QMAKE="%CK_ENV_LIB_QT_QMAKE_EXE%"
for /f %%i in ('%QMAKE% -query QT_INSTALL_BINS') do set WINDEPLOYQT=%%i\windeployqt

@echo on
%QMAKE% ..

%CK_MAKE% CC=%CK_CC% CXX=%CK_CXX%

copy ..\bin\crowdsource-video-experiments-on-desktop.exe %CK_PROG_TARGET_EXE%

%WINDEPLOYQT% %CK_PROG_TARGET_EXE%

@echo off

exit /b 0
