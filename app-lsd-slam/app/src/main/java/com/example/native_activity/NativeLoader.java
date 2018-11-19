package com.example.native_activity;

import android.app.Activity;

import android.graphics.SurfaceTexture;

import android.os.Bundle;
import android.view.TextureView;

/**
 * Created by smith on 6/16/16.
 */

public class NativeLoader extends Activity   {
    static {
        System.loadLibrary("g2o_types_slam3d");
        System.loadLibrary("g2o_types_sba");
        System.loadLibrary("g2o_types_sim3");
        System.loadLibrary("g2o_ext_csparse");
        System.loadLibrary("g2o_csparse_extension");
        System.loadLibrary("g2o_solver_csparse");
        System.loadLibrary("g2o_stuff");
        System.loadLibrary("g2o_core");
        System.loadLibrary("opencv_java3");
        System.loadLibrary("boost_thread");
        System.loadLibrary("boost_date_time");
        System.loadLibrary("boost_program_options");
        System.loadLibrary("boost_filesystem");
        System.loadLibrary("boost_serialization");
        System.loadLibrary("crystax");
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("boost_system");
        System.loadLibrary("lsdslam");
        System.loadLibrary("native-activity");
    }

    private static final String TAG = "Sample::SurfaceView";

    //16:9, 160:90, 320:180
    // 1280	720
    private static final int mWidth = 320;// 320;
    private static final int mHeight = 240;// 240;
    private NativeLoader nativeLoader;
    //private static final int height = 480;

    public native boolean pushImage(int width, int height, byte[] YUVFrameData);
    public native boolean convertImage(int width, int height, byte[] YUVFrameData, int[] frameData);

    public native void startSLAM();

    public native boolean getImage(int[] frameData);

    private TextureView mTextureView;


    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.native_loader);

        nativeLoader = this;

        mTextureView = (TextureView) findViewById(R.id.textureView);

        mTextureView.setSurfaceTextureListener(mSurfaceTextureListener);
    }

    private final TextureView.SurfaceTextureListener mSurfaceTextureListener
            = new TextureView.SurfaceTextureListener() {

        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture texture, int width, int height) {
            //setupCamera();
            boolean useCam2 = true;
            if (useCam2) {
                UseCamera2API camerauser = new UseCamera2API(mWidth, mHeight, nativeLoader, mTextureView);
                camerauser.setupCamera();
            } else {
                UseCameraAPI camerauser = new UseCameraAPI(mWidth, mHeight, nativeLoader, mTextureView);
                camerauser.setupCamera();
            }
        }


        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture texture, int width, int height) {
            //configureTransform(width, height);
        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture texture) {
            return true;
        }

        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture texture) {
        }

    };

}
