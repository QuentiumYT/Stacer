#!/bin/bash
VERSION=1.6.0
DIR=stacer-$VERSION
ARCH=$(uname -m)
ARCH_DPKG=$(dpkg --print-architecture)
export VERSION=$VERSION

# cleanup
rm -rf build release translations/*.qm rpm/BUILD rpm/BUILDROOT rpm/*RPMS rpm/SOURCES debug*.list elfbins.list

# build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j $(nproc)
strip -s build/stacer/stacer

# assets
mkdir -p release/$DIR
cp -r stacer stacer-core translations CMakeLists.txt debian desktop icons release/$DIR

# translations
export QT_SELECT=qt6
# Expand PATH to find lupdate & lrelease
export PATH="/usr/lib/$QT_SELECT/bin:$PATH"
lupdate stacer/stacer.pro -no-obsolete
lrelease stacer/stacer.pro
mkdir -p release/$DIR/stacer/translations
cp translations/*.qm release/$DIR/stacer/translations

# Change architecture
sed -i "s/^Architecture:\s\+.*$/Architecture: $ARCH_DPKG/g" release/$DIR/debian/control

# tarball
tar -czf release/$DIR.tar.gz -C release $DIR

# appimagetool
wget -qc https://github.com/$(wget -q https://github.com/probonopd/go-appimage/releases/expanded_assets/continuous -O - | grep "appimagetool-.*-$ARCH.AppImage" | head -n 1 | cut -d '"' -f 2) -O appimagetool-$ARCH.AppImage
chmod +x appimagetool-$ARCH.AppImage

# appimage
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
DESTDIR=../release/$DIR cmake --build build --target install
./appimagetool-$ARCH.AppImage -s deploy release/$DIR/usr/share/applications/stacer.desktop
./appimagetool-$ARCH.AppImage release/$DIR
mv Stacer-$VERSION-$ARCH.AppImage release

rm appimagetool-$ARCH.AppImage

# debian package
cd release/$DIR
# Export CMake prefix path for debuild using aqtinstall
if [ -z "$QT_PLUGIN_PATH" ]; then
    QT_ROOT=$(dirname "$QT_PLUGIN_PATH")
    export CMAKE_PREFIX_PATH="$QT_ROOT/lib/cmake"
fi
dh_make --createorig --indep --yes
debuild --preserve-envvar=CMAKE_PREFIX_PATH \
    --preserve-envvar=QT_PLUGIN_PATH \
    --preserve-envvar=LD_LIBRARY_PATH \
    --no-lintian -us -uc
cd ../..

# rpm package
mkdir -p rpm/SOURCES
cp release/$DIR.tar.gz rpm/SOURCES
# Change architecture
sed -i "s/^BuildArch:\s\+.*$/BuildArch:      $ARCH/g" rpm/SPECS/stacer.spec
rpmbuild -bb --build-in-place --define "_topdir $(pwd)/rpm" rpm/SPECS/stacer.spec
mv rpm/RPMS/$ARCH/stacer-$VERSION-1.$ARCH.rpm release/stacer-$VERSION.$ARCH.rpm
