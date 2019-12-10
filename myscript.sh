#!/bin/bash

cp -r ${MESON_SOURCE_ROOT}/PathOfBuilding/* ${MESON_INSTALL_PREFIX}/Contents/MacOS/

mkdir -p ${MESON_INSTALL_PREFIX}/Contents/Frameworks

#cp -R /usr/local/opt/qt5/Frameworks/* \
#        ${MESON_INSTALL_PREFIX}/Contents/Frameworks

#install_name_tool -change @rpath/SDL2.framework/Versions/A/SDL2 \
#    @executable_path/../FrameWorks/SDL2.framework/Versions/A/SDL2 \
#    ${MESON_INSTALL_PREFIX}/Contents/MacOS/myapp
