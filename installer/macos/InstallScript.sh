#!/bin/sh

APP="$PWD/../files/QAN1xEditor.app" #the compiled bundle
INSTALLER="$PWD/../compiled/qan1xeditor-1.6.2-macos.dmg"
QDEPLOY="$PWD/../../../Qt/6.5.3/macos/bin/macdeployqt" #macdeployqt location

$QDEPLOY $APP -sign-for-notarization="Developer ID Application: Hristo Konstantinov" #deploying the app

#deleting the old dmg installer
Rm -R $INSTALLER

#creating dmg installer using create-dmg
create-dmg \
  --volname "QAN1xEditor Installer" \
  --volicon "icon.icns" \
  --background "background.png" \
  --window-pos 200 110 \
  --window-size 605 360 \
  --icon-size 72 \
  --icon "QAN1xEditor.app" 250 190 \
  --app-drop-link 460 190 \
  --hide-extension "QAN1xEditor.app" \
  $INSTALLER \
  "$PWD/../files/"

codesign --options runtime --timestamp -s "Developer ID Application: Hristo Konstantinov" $INSTALLER

xcrun notarytool submit $INSTALLER --keychain-profile "DeployProfile" --wait

xcrun stapler staple $INSTALLER

spctl --assess -vv --type install $INSTALLER

rm -R $APP
