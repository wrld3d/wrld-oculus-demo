#!/bin/sh

echo
echo "Compile xcode project step..."
echo

# Path where the xcworkspace is located.
projectpath=$(pwd)/Samples/

# Compile the xcode project.
(cd $projectpath && xcodebuild -workspace LibOVR_With_Samples.xcworkspace -scheme OculusWorldDemo -arch x86_64 -configuration Release)
resultcode=$?

echo
if [ $resultcode = 0 ] ; then
  echo "COMPILE XCODE PROJECT SUCCEEDED"
else
  echo "COMPILE XCODE PROJECT FAILED"
  exit $resultcode
fi
echo

exit 0
