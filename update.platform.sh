#!/bin/bash

baseUrl="http://s3.amazonaws.com/eegeo-static/"
destPackageName="./sdk.package.tar.gz"
srcPackageName="sdk.package.osx"
includeDestination="./eegeo-platform"
sdkDestination="sdk.package.osx"

echo "Updating osx platform..."

rm -rf $includeDestination

packageUrl="${baseUrl}${srcPackageName}.tar.gz"
echo "downloading ${packageUrl} ..."
curl $packageUrl > ./$destPackageName

if [ $? -ne 0 ] ; then
  echo "Failed to download SDK package from ${packageUrl}."
  exit 1
fi

echo "unzipping ${destPackageName} ..."
tar -zxvf $destPackageName 2> /dev/null

if [ $? -ne 0 ] ; then
  echo "Failed to unzip ${destPackageName}"
  exit 1
fi

rm -f ./$destPackageName

platformVersion=`cat ./$sdkDestination/version.txt`

echo "Platform version --> $platformVersion"

mv ./$sdkDestination $includeDestination

if [ $? -ne 0 ] ; then
  echo "Failed to move SDK package to destination directory"
  exit 1
fi

echo "Successfully updated osx SDK package."

exit 0
