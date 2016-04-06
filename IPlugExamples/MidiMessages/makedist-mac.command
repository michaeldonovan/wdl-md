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

VST2="/Library/Audio/Plug-Ins/VST/MidiMessages.vst"
VST3="/Library/Audio/Plug-Ins/VST3/MidiMessages.vst3"
APP="/Applications/MidiMessages.app"
AUDIOUNIT="/Library/Audio/Plug-Ins/Components/MidiMessages.component"
RTAS="/Library/Application Support/Digidesign/Plug-Ins/MidiMessages.dpm"

echo "making MidiMessages version $FULL_VERSION mac distribution..."
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

xcodebuild -project MidiMessages.xcodeproj -xcconfig MidiMessages.xcconfig -target "All" -configuration Release
#xcodebuild -project MidiMessages-ios.xcodeproj -xcconfig MidiMessages.xcconfig -target "IOSAPP" -configuration Release

#icon stuff - http://maxao.free.fr/telechargements/setfileicon.gz
echo "setting icons"
echo ""
setfileicon resources/MidiMessages.icns $AUDIOUNIT
setfileicon resources/MidiMessages.icns $VST2
setfileicon resources/MidiMessages.icns $VST3
setfileicon resources/MidiMessages.icns "${RTAS}"

#appstore stuff

# echo "code signing app"
# echo ""
# codesign -f -s "3rd Party Mac Developer Application: Oliver Larkin" $APP
#  
# echo "building pkg for app store"
# productbuild \
#      --component $APP /Applications \
#      --sign "3rd Party Mac Developer Installer: Oliver Larkin" \
#      --product "/Applications/MidiMessages.app/Contents/Info.plist" installer/MidiMessages.pkg

# installer, uses iceberg http://s.sudre.free.fr/Software/Iceberg.html

sudo sudo rm -R -f installer/MidiMessages-mac.dmg

echo "building installer"
echo ""
freeze installer/MidiMessages.packproj

# dmg, uses dmgcanvas http://www.araelium.com/dmgcanvas/

echo "building dmg"
echo ""

if [ -d installer/MidiMessages.dmgCanvas ]
then
  dmgcanvas installer/MidiMessages.dmgCanvas installer/MidiMessages-mac.dmg
else
  hdiutil create installer/MidiMessages.dmg -srcfolder installer/build-mac/ -ov -anyowners -volname MidiMessages
  
  if [ -f installer/MidiMessages-mac.dmg ]
  then
   rm -f installer/MidiMessages-mac.dmg
  fi
  
  hdiutil convert installer/MidiMessages.dmg -format UDZO -o installer/MidiMessages-mac.dmg
  sudo sudo rm -R -f installer/MidiMessages.dmg
fi

sudo sudo rm -R -f installer/build-mac/

# echo "copying binaries..."
# echo ""
# cp -R $AUDIOUNIT installer/dist/MidiMessages.component
# cp -R $VST2 installer/dist/MidiMessages.vst
# cp -R $VST3 installer/dist/MidiMessages.vst3
# cp -R $RTAS installer/dist/MidiMessages.dpm
# cp -R $APP installer/dist/MidiMessages.app
# 
# echo "zipping binaries..."
# echo ""
# ditto -c -k installer/dist installer/MidiMessages-mac.zip
# rm -R installer/dist

echo "done"