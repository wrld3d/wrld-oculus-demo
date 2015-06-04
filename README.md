Oculus SDK / eeGeo SDK
----------------------

Run ./update.platform -p osx
Open LibOVR_With_Samples.xcworkspace in XCode
x86_64 build should work fine

Please Note:

* Alpha release only - not supported!
* More information at http://eegeo.com/oculusvr/

Recommended Versions (only tried on these):

* Headset firmware 2.12
* Oculus OSX Runtime 0.4.1 (0.4.4 has bad jitter)
* Mac with GPU capable of hitting 75Hz when rendering eeGeo SDK

Oculus Headset Setup:

* This is *important* if you want stable framerate on OSX:
* Setup Oculus as a second _non mirrored_ monitor, rotated 90o, 1080p (or option 'best for Oculus DK2') and 75hz 
* Set the Oculus monitor as the Primary Monitor (this causes problems when opening apps, remove the lenses, you enjoy that now)
