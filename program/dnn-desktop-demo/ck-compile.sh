
NAME="crowdsource-video-experiments-on-desktop-linux"

rm -rf *
unzip "../prebuilt/$NAME.zip"
chmod +x "$CK_PROG_TARGET_EXE.sh"

if [ -z "${CK_REPOS}" ]; then 
    REPOS="$HOME/CK"
else 
    REPOS="$CK_REPOS"
fi

cat >app.conf <<EOF
[General]
ck_exe_name=./ck
ck_bin_path=$CK_ROOT/bin
ck_repos_path=$REPOS

EOF
