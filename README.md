<a href="http://www.wrld3d.com/">
    <img src="http://cdn2.eegeo.com/wp-content/uploads/2017/04/WRLD_Blue.png" align="right" height="80px" />
</a>

# WRLD Oculus Demo

![WRLD](http://cdn2.eegeo.com/wp-content/uploads/2017/04/screenselection01.png)

* [Support](#support)
* [Getting Started](#getting-started)
	* [Requirements](#requirements)
	* [Setup](#setup)
	* [Controls](#controls)
* [Contributing](#contributing)
* [License](#license)

The WRLD Oculus Demo is an experimental project allowing you to explore [WRLD's 3D maps](https://www.wrld3d.com) through an Oculus VR headset. It integrates both the Oculus OSX SDK with the WRLD OSX SDK.

See [https://www.wrld3d.com/developers/virtual-reality-maps](https://www.wrld3d.com/developers/virtual-reality-maps) for more information.

## Support

If you have any questions, bug reports, or feature requests, feel free to submit to the [issue tracker](https://github.com/wrld3d/wrld-oculus-demo/issues) for this repository.

## Getting Started

To get started, first clone this repo: `git clone git@github.com:wrld3d/wrld-oculus-demo.git`

### Requirements

*   [Xcode](https://developer.apple.com/xcode/) (11.5 tested)
*   [Oculus OS X Runtime](https://developer.oculus.com/downloads/pc/0.4.1-beta/Oculus_Runtime_for_OS_X/) (0.4.1 tested)
*   Headset firmware 2.12
*   OS X machine with GPU capable of running the WRLD SDK at 75Hz

### Setup

1.  From the root of the repo, run `./update.platform.sh` to download the most up to date version of the WRLD OSX SDK.
2.  Open the [Samples/LibOVR_With_Samples.xcworkspace](https://github.com/wrld3d/wrld-oculus-demo/tree/master/Samples/LibOVR_With_Samples.xcworkspace) workspace in Xcode.
3.  Setup the headset as a second **non-mirrored** monitor:
    *   Set the display rotated **90&deg;**
    *   Set the resolution to **1080p** (or option 'best for Oculus DK2')
    *   Set the refresh rate to **75Hz**
4.  Set the headset as the primary monitor.
5.  Run the OculusWorldDemo target in Xcode.

### Controls
*   The headset's head tracking controls head movement.
*   Use the **mouse** to control body rotation.
*   Use the **WASD** keys to move horizontally.
*   Use the **R** and **F** keys to move vertically.
*   Use the **L** key to enable Fullscreen. This is required to achieve a reasonable framerate.
*   For additional options:
    *   Use the **G** key to disable gravity.
    *   Use the **N** key to toggle between day and night.
    *   Use the **O** key to begin a fly-through.
    *   Use the **T** key to jump to other locations.

## Contributing

Pull requests are welcomed if you would like to contribute to this project.

## License

This repository is a fork of the Oculus SDK. The Oculus SDK is provided under the Oculus VR Rift SDK Software License. See the [LICENSE.txt](https://github.com/wrld3d/wrld-oculus-demo/blob/master/LICENSE.txt) file for details.

Modifications by WRLD to integrate the WRLD 3D Maps SDK are released under the Simplified BSD License. See the [LICENSE.md](https://github.com/wrld3d/wrld-oculus-demo/blob/master/LICENSE.md) file for details.
