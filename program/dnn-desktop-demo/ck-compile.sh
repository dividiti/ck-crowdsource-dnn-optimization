
MACHINE=`uname -m`
NAME="crowdsource-video-experiments-on-desktop-linux-$MACHINE"

rm -rf *
unzip "../prebuilt/$NAME.zip"
chmod +x "$CK_PROG_TARGET_EXE.sh"
