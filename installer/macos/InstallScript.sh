#!/bin/sh

APP="$PWD/../files/QAN1xEditor.app" #the compiled bundle

QDEPLOY="$PWD/../../../Qt/6.5.3/macos/bin/macdeployqt" #macdeployqt location

$QDEPLOY $APP #deploying the app

#creating dmg installer using create-dmg
create-dmg \
  --volname "QAN1xEditor Installer" \
  --volicon "icon.icns" \
  --background "background.png" \
  --window-pos 200 110 \
  --window-size 605 360 \
  --icon-size 72 \
  --icon "QAN1xEditor.app" 250 170 \
  --app-drop-link 460 170 \
  --add-file "Instructions.txt" "Instructions.txt" 460 30 \
  --hide-extension "QAN1xEditor.app" \
  "$PWD/../compiled/qanxeditor-1.2-macos-arm64.dmg" \
  "$PWD/../files/"
