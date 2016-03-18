<a href="http://www.eegeo.com/">
    <img src="http://cdn2.eegeo.com/wp-content/uploads/2016/03/eegeo_logo_quite_big.png" alt="eeGeo Logo" title="eegeo" align="right" height="80px" />
</a>

# eeGeo Oculus Demo

The eeGeo Oculus Demo is an experimental project allowing you to explore [eeGeo's 3D maps](https://www.eegeo.com) through an Oculus VR headset. It integrates both the Oculus OSX SDK with the eeGeo OSX SDK.

The project is still in its early stages. See [https://www.eegeo.com/developers/virtual-reality-maps](https://www.eegeo.com/developers/virtual-reality-maps) for more information.

## Getting Started

To get started, first clone this repo: `git clone git@github.com:eegeo/eegeo-oculus-demo.git`

### Requirements

*   [Xcode](https://developer.apple.com/xcode/) (7.2 tested)
*   [Oculus OS X Runtime](https://developer.oculus.com/downloads/pc/0.4.1-beta/Oculus_Runtime_for_OS_X/) (0.4.1 tested)
*   Headset firmware 2.12
*   OS X machine with GPU capable of running the eeGeo SDK at 75Hz

### Setup

1.  From the root of the repo, run `./update.platform.sh` to download the most up to date version of the eeGeo OSX SDK.
2.  Open the [LibOVR_With_Samples.xcworkspace](https://github.com/eegeo/eegeo-oculus-demo/tree/master/Samples/LibOVR_With_Samples.xcworkspace) workspace in Xcode.
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

## Support

If you have any issues, bug reports, or feature requests, feel free to submit to the [issue tracker](https://github.com/eegeo/eegeo-oculus-demo/issues) for this repository. If you have other questions, you can contact us at [support@eegeo.com](mailto:support@eegeo.com).