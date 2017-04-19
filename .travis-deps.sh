#!/bin/sh
set -e
set -x

# Build and install devkitARM + ctrulib
wget http://sourceforge.net/projects/devkitpro/files/Automated%20Installer/devkitARMupdate.pl
git clone https://github.com/smealum/ctrulib.git
perl devkitARMupdate.pl

# Get latest ctrulib and overwrite bundled one
cd ctrulib/libctru && make ; cd -
cp -rf ctrulib/libctru/ ${DEVKITPRO}

# Build and install portlibs
mkdir ${DEVKITPRO}/portlibs && mkdir ${PORTLIBS}
git clone https://github.com/Cruel/3ds_portlibs.git
cd 3ds_portlibs

make zlib
make install-zlib

make libogg
make install # Need to install libogg for libvorbis

make freetype
make install

make libpng
make install

make libjpeg-turbo
make install

make libvorbis
make install

cd -

git clone https://github.com/fincs/citro3d
git clone https://github.com/xerpi/sf2dlib
git clone https://github.com/xerpi/sftdlib
git clone https://github.com/xerpi/sfillib

cd citro3d && make install && cd -
cd sf2dlib/libsf2d && make install && cd -
cd sftdlib/libsftd && make install && cd -
cd sfillib/libsfil && make install && cd -
