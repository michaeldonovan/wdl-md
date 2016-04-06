#! /bin/sh

BASEDIR=$(dirname $0)

cd $BASEDIR

VERSION=`echo | grep PLUG_VER resource.h`
VERSION=${VERSION//\#define PLUG_VER }
VERSION=${VERSION//\'}
MAJOR_VERSION=$(($VERSION & 0xFFFF0000))
MAJOR_VERSION=$(($MAJOR_VERSION >> 16)) 
MINOR_VERSION=$(($VERSION & 0x0000FF00))
MINOR_VERSION=$(($MINOR_VERSION >> 8)) 
BUG_FIX=$(($VERSION & 0x000000FF))

FULL_VERSION=$MAJOR_VERSION"."$MINOR_VERSION"."$BUG_FIX

VST2="/Library/Audio/Plug-Ins/VST/IPlugGamma.vst"
VST3="/Library/Audio/Plug-Ins/VST3/IPlugGamma.vst3"
APP="/Applications/IPlugGamma.app"
AUDIOUNIT="/Library/Audio/Plug-Ins/Components/IPlugGamma.component"
RTAS="/Library/Application Support/Digidesign/Plug-Ins/IPlugGamma.dpm"

echo "making IPlugGamma version $FULL_VERSION mac distribution..."
echo ""

./update_version.py

#could use touch to force a rebuild
#touch blah.h

#remove existing dist folder
#if [ -d installer/dist ] 
#then
#  rm -R installer/dist
#fi

#mkdir installer/dist

#remove existing App
if [ -d $APP ] 
then
  sudo rm -R -f $APP
fi

#remove existing AU
if [ -d $AUDIOUNIT ] 
then
  sudo rm -R $AUDIOUNIT
fi

#remove existing VST2
if [ -d $VST2 ] 
then
  sudo rm -R $VST2
fi

#remove existing VST3
if [ -d $VST3 ] 
then
  rm -R $VST3
fi


#remove existing RTAS
if [ -d "${RTAS}" ] 
then
  sudo rm -R "${RTAS}"
fi

xcodebuild -project IPlugGamma.xcodeproj -xcconfig IPlugGamma.xcconfig -target "All" -configuration Release
#xcodebuild -project IPlugGamma-ios.xcodeproj -xcconfig IPlugGamma.xcconfig -target "IOSAPP" -configuration Release

#icon stuff - http://maxao.free.fr/telechargements/setfileicon.gz
echo "setting icons"
echo ""
setfileicon resources/IPlugGamma.icns $AUDIOUNIT
setfileicon resources/IPlugGamma.icns $VST2
setfileicon resources/IPlugGamma.icns $VST3
setfileicon resources/IPlugGamma.icns "${RTAS}"

#appstore stuff

# echo "code signing app"
# echo ""
# codesign -f -s "3rd Party Mac Developer Application: Oliver Larkin" $APP
#  
# echo "building pkg for app store"
# productbuild \
#      --component $APP /Applications \
#      --sign "3rd Party Mac Developer Installer: Oliver Larkin" \
#      --product "/Applications/IPlugGamma.app/Contents/Info.plist" installer/IPlugGamma.pkg

# installer, uses iceberg http://s.sudre.free.fr/Software/Iceberg.html

sudo sudo rm -R -f installer/IPlugGamma-mac.dmg

echo "building installer"
echo ""
freeze installer/IPlugGamma.packproj

# dmg, uses dmgcanvas http://www.araelium.com/dmgcanvas/

echo "building dmg"
echo ""

if [ -d installer/IPlugGamma.dmgCanvas ]
then
  dmgcanvas installer/IPlugGamma.dmgCanvas installer/IPlugGamma-mac.dmg
else
  hdiutil create installer/IPlugGamma.dmg -srcfolder installer/build-mac/ -ov -anyowners -volname IPlugGamma
  
  if [ -f installer/IPlugGamma-mac.dmg ]
  then
   rm -f installer/IPlugGamma-mac.dmg
  fi
  
  hdiutil convert installer/IPlugGamma.dmg -format UDZO -o installer/IPlugGamma-mac.dmg
  sudo sudo rm -R -f installer/IPlugGamma.dmg
fi

sudo sudo rm -R -f installer/build-mac/

# echo "copying binaries..."
# echo ""
# cp -R $AUDIOUNIT installer/dist/IPlugGamma.component
# cp -R $VST2 installer/dist/IPlugGamma.vst
# cp -R $VST3 installer/dist/IPlugGamma.vst3
# cp -R $RTAS installer/dist/IPlugGamma.dpm
# cp -R $APP installer/dist/IPlugGamma.app
# 
# echo "zipping binaries..."
# echo ""
# ditto -c -k installer/dist installer/IPlugGamma-mac.zip
# rm -R installer/dist

echo "done"