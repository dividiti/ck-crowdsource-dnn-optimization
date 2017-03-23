
QMAKE="$CK_ENV_LIB_QT_QMAKE_EXE"

$QMAKE ..

make CC=$CK_CC CXX=$CK_CXX

cp ../bin/crowdsource-video-experiments-on-desktop .
