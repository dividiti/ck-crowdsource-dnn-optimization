#!/bin/sh

export APP_EXE=crowdsource-video-experiments-on-desktop
export TARGET_FILE=crowdsource-video-experiments-on-desktop-linux-armv7l.zip
export TARGET_DIR="./prebuilt"
export REDIST="./redist"
export BIN="./bin"
export SRC="./src"
export QT_DIR="/usr/lib/arm-linux-gnueabihf/qt5"
export QT_LIBS="/usr/lib/arm-linux-gnueabihf"

rm -rf "$REDIST"
mkdir -p ${REDIST}

echo "Copy app files..."
cp -f ${BIN}/${APP_EXE} ${REDIST}
cp -f ${APP_EXE}.sh ${REDIST}
cp -f ${SRC}/app.conf.example.linux ${REDIST}/app.conf

echo "Copy Qt libs..."
cp -fL ${QT_LIBS}/libQt5Core.so.5 ${REDIST}
cp -fL ${QT_LIBS}/libQt5DBus.so.5 ${REDIST}
cp -fL ${QT_LIBS}/libQt5Gui.so.5 ${REDIST}
cp -fL ${QT_LIBS}/libQt5Svg.so.5 ${REDIST}
cp -fL ${QT_LIBS}/libQt5Widgets.so.5 ${REDIST}
cp -fL ${QT_LIBS}/libQt5XcbQpa.so.5 ${REDIST}
cp -fL ${QT_LIBS}/libicudata.so.57 ${REDIST}
cp -fL ${QT_LIBS}/libicui18n.so.57 ${REDIST}
cp -fL ${QT_LIBS}/libicuuc.so.57 ${REDIST}

cp -fL ${QT_LIBS}/libdouble-conversion.so.1 ${REDIST}
cp -fL ${QT_LIBS}/libxcb-render-util.so.0 ${REDIST}
cp -fL ${QT_LIBS}/libxcb-image.so.0 ${REDIST}
cp -fL ${QT_LIBS}/libxcb-icccm.so.4 ${REDIST}
cp -fL ${QT_LIBS}/libxcb-randr.so.0 ${REDIST}
cp -fL ${QT_LIBS}/libxcb-keysyms.so.1 ${REDIST}
cp -fL ${QT_LIBS}/libxcb-xinerama.so.0 ${REDIST}
cp -fL ${QT_LIBS}/libxcb-xkb.so.1 ${REDIST}
cp -fL ${QT_LIBS}/libxkbcommon-x11.so.0 ${REDIST}
cp -fL ${QT_LIBS}/libxkbcommon.so.0 ${REDIST}

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
TARGET_PATH=${CUR_DIR}/${TARGET_DIR}/${TARGET_FILE}
rm -f "$TARGET_PATH"
zip -r "$TARGET_PATH" .

echo "Package created: ${TARGET_DIR}/${TARGET_FILE}"
echo "Done"
