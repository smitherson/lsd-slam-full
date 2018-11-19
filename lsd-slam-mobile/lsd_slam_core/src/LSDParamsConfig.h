

namespace lsd_slam_core {

    struct LSDParamsConfig {
        //Minimal Absolut Image Gradient for a Pixel to be used at all. Increase, if your camera has large image noise, Decrease if you have low image-noise and want to also exploit small gradients.
        double minUseGrad = 10; // 1, 50)

        //Image intensity noise used for e.g. tracking weight calculation. Sould be set larger than the actual sensor-noise, to also account for noise originating from discretization / linear interpolation.
        double cameraPixelNoise = 4; //, 1, 50)

        //"Determines how often Keyframes are taken, depending on the Overlap to the current Keyframe. Larger -> more Keyframes.
        double KFUsageWeight= 4.0;//,4, 0.0, 20)

        //"Determines how often Keyframes are taken, depending on the Distance to the current Keyframe. Larger -> more Keyframes.
        double KFDistWeight= 3;// 3, 0.0, 20)

        //"Toggle Global Mapping Component on/off. Only takes effect after a reset.
        bool doSLAM = false;

        //Toggle Keyframe Re-Activation on/off: If close to an existing keyframe, re-activate it instead of creating a new one. If false, Map will continually grow even if the camera moves in a relatively constrained area; If false, the number of keyframes will not grow arbitrarily.
        bool doKFReActivation = true;

        //Toggle entire Keyframe Creating / Update module on/off: If false, only the Tracking Component stays active, which will prevent rapid motion or moving objects from corrupting the map.
        bool doMapping = true;

        //Use OpenFABMAP to find large loop-closures. Only takes effect after a reset, and requires LSD-SLAM to be compiled with FabMap.
        bool useFabMap= false;

        //"Allow idepth to be (slightle) negative, to avoid introducing a bias for far-away points."
        bool allowNegativeIdepths= true;

        //"Compute subpixel-accurate stereo disparity."
        bool useSubpixelStereo= true;

        //EXPERIMENTAL: Correct for global affine intensity changes during tracking. Might help if you have Problems with Auto-Exposure.
        bool useAffineLightningEstimation= true;

        //Toggle Multi-Threading of DepthMap Estimation. Disable for less CPU usage, but possibly slightly less quality.
        bool multiThreading= true;

        //Maximal of Loop-Closures that are tracked initially for each new keyframe.
        int maxLoopClosureCandidates= 10;//, 0, 50)

        //Threshold on reciprocal loop-closure consistency check, to be added to the map. Larger -> more (possibly wrong) Loopclosures.
        double loopclosureStrictness= 1.5;//, 0.0, 100)

        //How good a relocalization-attempt has to be, to be accepted. Larger -> More Strict.
        double relocalizationTH= 0.7;//, 0;//, 1)

        //How much to smooth the depth map. Larger -> Less Smoothing
        double depthSmoothingFactor= 1;//1 , 0;//, 10)
    };
}
