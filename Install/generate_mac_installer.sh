#!/bin/sh

EXEFOLDER=../Run
APPNAME=3DMetrics
OSGPLUGINS_PATH=/opt/local/lib/osgPlugins-3.6.5
MACDEPLOYQT=/opt/local/libexec/qt5/bin/macdeployqt

OUTFILE=${EXEFOLDER}/$APPNAME
APPBUNDLE=${APPNAME}.app
APPBUNDLECONTENTS=${APPBUNDLE}/Contents
APPBUNDLEEXE=${APPBUNDLECONTENTS}/MacOS
APPBUNDLERESOURCES=${APPBUNDLECONTENTS}/Resources
APPBUNDLEICON=${APPBUNDLECONTENTS}/Resources
APPBUNDLEPLUGINS=${APPBUNDLECONTENTS}/Plugins

rm -rf $APPBUNDLE
mkdir $APPBUNDLE
mkdir ${APPBUNDLE}/Contents
mkdir ${APPBUNDLE}/Contents/MacOS
mkdir ${APPBUNDLE}/Contents/Resources
mkdir ${APPBUNDLEPLUGINS}
cp macosx/Info.plist ${APPBUNDLECONTENTS}/
cp macosx/PkgInfo ${APPBUNDLECONTENTS}/
#cp macosx/${APPNAME}.icns $(APPBUNDLEICON)/
cp ${OUTFILE} ${APPBUNDLEEXE}/${APPNAME}

dylibbundler -od -b -x ${APPBUNDLEEXE}/${APPNAME} -d ${APPBUNDLECONTENTS}/Frameworks -p @executable_path/../Frameworks
${MACDEPLOYQT} $APPBUNDLE

# copy plugins and fix them
cp ${OSGPLUGINS_PATH}/*.so ${APPBUNDLEPLUGINS}

for plugin_file in `find "$APPBUNDLEPLUGINS" -name '*.so'`; do
    echo "preparing plugin ${file_to_sign}"
    dylibbundler -b -of -x ${plugin_file} -d ${APPBUNDLECONTENTS}/Frameworks -p @executable_path/../Frameworks
done

#macosx/$(APPNAME).icns: macosx/$(APPNAME)Icon.png
#    rm -rf macosx/$(APPNAME).iconset
#    mkdir macosx/$(APPNAME).iconset
#    sips -z 16 16     macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_16x16.png
#    sips -z 32 32     macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_16x16@2x.png
#    sips -z 32 32     macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_32x32.png
#    sips -z 64 64     macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_32x32@2x.png
#    sips -z 128 128   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_128x128.png
#    sips -z 256 256   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_128x128@2x.png
#    sips -z 256 256   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_256x256.png
#    sips -z 512 512   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_256x256@2x.png
#    sips -z 512 512   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_512x512.png
#    cp macosx/$(APPNAME)Icon.png macosx/$(APPNAME).iconset/icon_512x512@2x.png
#    iconutil -c icns -o macosx/$(APPNAME).icns macosx/$(APPNAME).iconset
#    rm -r macosx/$(APPNAME).iconset
