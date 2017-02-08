#!/bin/sh

export APP_EXE=crowdsource-video-experiments-on-desktop
export TARGET_FILE=crowdsource-video-experiments-on-desktop-ubuntu.zip
export TARGET_DIR="./tmp"
export REDIST="./redist"
export BIN="./bin"
export SRC="./src"
export QT_DIR="${HOME}/Qt/5.7/gcc_64"
export QT_LIBS="${QT_DIR}/lib"

mkdir -p ${REDIST}

echo "Copy app files..."
cp -f ${BIN}/${APP_EXE} ${REDIST}
cp -f ${BIN}/*.sh ${REDIST}
cp -f ${BIN}/*.qss ${REDIST}
cp -f ${SRC}/app.conf.example ${REDIST}/app.conf

echo "Copy Qt libs..."
cp -fL ${QT_LIBS}/libQt5Core.so.5 ${REDIST}
cp -fL ${QT_LIBS}/libQt5DBus.so.5 ${REDIST}
cp -fL ${QT_LIBS}/libQt5Gui.so.5 ${REDIST}
cp -fL ${QT_LIBS}/libQt5Svg.so.5 ${REDIST}
cp -fL ${QT_LIBS}/libQt5Widgets.so.5 ${REDIST}
cp -fL ${QT_LIBS}/libQt5XcbQpa.so.5 ${REDIST}
cp -fL ${QT_LIBS}/libicudata.so.56 ${REDIST}
cp -fL ${QT_LIBS}/libicui18n.so.56 ${REDIST}
cp -fL ${QT_LIBS}/libicuuc.so.56 ${REDIST}

echo "Copy Qt plugins..."
mkdir -p ${REDIST}/imageformats
mkdir -p ${REDIST}/platforms
cp -fL ${QT_DIR}/plugins/imageformats/libqjpeg.so ${REDIST}/imageformats
cp -fL ${QT_DIR}/plugins/imageformats/libqsvg.so ${REDIST}/imageformats
cp -fL ${QT_DIR}/plugins/platforms/libqxcb.so ${REDIST}/platforms

echo "Packing..."
mkdir -p ${TARGET_DIR}
export CUR_DIR=${PWD}
cd ${REDIST}
zip -r ${CUR_DIR}/${TARGET_DIR}/${TARGET_FILE} .

echo "Package created: ${TARGET_DIR}/${TARGET_FILE}"
echo "Done"
