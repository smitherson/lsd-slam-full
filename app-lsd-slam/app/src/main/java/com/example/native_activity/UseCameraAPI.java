package com.example.native_activity;

import android.app.Activity;
import android.graphics.Bitmap;

import android.graphics.Canvas;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.TextureView;

import java.io.IOException;
import java.util.List;

/**
 * Created by smith on 7/14/16.
 */

public class UseCameraAPI {
    private TextureView mTextureView;
    private Camera mCamera;
    private NativeLoader nativeLoader;
    private Activity activity;
    private boolean mThreadRun;
    private  int width;
    private int height;
    private byte[]              mFrame;
    private byte[]              mBuffer;
    private Bitmap mBitmap ;


    private boolean startedSlam = false;

    private static final String TAG = "Camera2";

    public UseCameraAPI(int width, int height, NativeLoader nativeLoader, TextureView mTextureView) {
        this.width = width;
        this.height = height;
        this.nativeLoader = nativeLoader;
        this.activity = nativeLoader;
        this.mTextureView = mTextureView;
        mBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
    }

    public boolean openCamera() {
        Log.i(TAG, "openCamera");
        releaseCamera();
        mCamera = Camera.open();
        if(mCamera == null) {
            Log.e(TAG, "Can't open camera!");
            return false;
        }

        mCamera.setPreviewCallbackWithBuffer(new Camera.PreviewCallback() {

            public void onPreviewFrame(byte[] data, Camera camera) {

                synchronized (UseCameraAPI.this) {
                    System.arraycopy(data, 0, mFrame, 0, data.length);
                    UseCameraAPI.this.notify();
                }
                camera.addCallbackBuffer(mBuffer);


            }
        });
        return true;
    }

    private void releaseCamera() {
        Log.i(TAG, "releaseCamera");
        //mThreadRun = false;
        synchronized (this) {
            if (mCamera != null) {
                mCamera.stopPreview();
                mCamera.setPreviewCallback(null);
                mCamera.release();
                mCamera = null;
            }
        }
        //onPreviewStopped();
    }

    public void setupCamera() {
        openCamera();
        SurfaceTexture texture = mTextureView.getSurfaceTexture();
        assert texture != null;

        // We configure the size of default buffer to be the size of camera preview we want.
        texture.setDefaultBufferSize(width, height);

        Log.i(TAG, "setupCamera");
        synchronized (this) {
            if (mCamera != null) {
                Camera.Parameters params = mCamera.getParameters();
                List<Camera.Size> sizes = params.getSupportedPreviewSizes();
                //mFrameWidth = width;
                //mFrameHeight = height;

                // selecting optimal camera preview size
                /*{
                    int  minDiff = Integer.MAX_VALUE;
                    for (Camera.Size size : sizes) {
                        if (Math.abs(size.height - height) < minDiff) {
                            mFrameWidth = size.width;
                            mFrameHeight = size.height;
                            minDiff = Math.abs(size.height - height);
                        }
                    }
                }*/

                params.setPreviewSize(width, height);

                List<String> FocusModes = params.getSupportedFocusModes();
                if (FocusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO))
                {
                    params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
                }

                mCamera.setParameters(params);

                /* Now allocate the buffer */
                params = mCamera.getParameters();
                int size = params.getPreviewSize().width * params.getPreviewSize().height;
                size  = size * ImageFormat.getBitsPerPixel(params.getPreviewFormat()) / 8;
                mBuffer = new byte[size];
                /* The buffer where the current frame will be copied */
                mFrame = new byte [size];
                mCamera.addCallbackBuffer(mBuffer);

                try {
                    //setPreview();
                    mCamera.setPreviewTexture( new SurfaceTexture(2000) );

                } catch (IOException e) {
                    Log.e(TAG, "mCamera.setPreviewDisplay/setPreviewTexture fails: " + e);
                }

                /* Notify that the preview is about to be started and deliver preview size */
                //onPreviewStarted(params.getPreviewSize().width, params.getPreviewSize().height);

                /* Now we can start a preview */
                mCamera.startPreview();

                (new Thread() {
                    public void run() {
                        runImageProcessing();
                    }
                }).start();
            }
        }
    }

    public void runImageProcessing() {
        mThreadRun = true;
        Log.i(TAG, "Starting processing thread");
        while (mThreadRun) {
            Bitmap bmp = null;

            synchronized (this) {
                try {
                    this.wait();


                    if (!startedSlam) {
                        startedSlam = true;
                        (new Thread() {
                            public void run() {
                                nativeLoader.startSLAM();
                            }
                        }).start();

                    }
                    nativeLoader.pushImage(width, height, mFrame);

                    int[] rgba = new int[width*height];
                    nativeLoader.getImage(rgba);
                    //nativeLoader.convertImage(width, height, data, rgba);

                    bmp = mBitmap;
                    bmp.setPixels(rgba, 0, width, 0, 0, width, height);

                    Canvas canvas = mTextureView.lockCanvas();

                    if (canvas != null) {
                        Log.i(TAG, "Drawing image");
                        canvas.drawBitmap(bmp, 0, 0, null );//configureTransform(width, height),  null);
                        //canvas.drawBitmap(bmp, configureTransform(width, height),  null);
                        //canvas.drawBitmap(bmp, (canvas.getWidth() - 320) / 2, (canvas.getHeight() - 240) / 2, null);
                        // canvas.drawBitmap(bmp, new Rect(0,0,320,240), new Rect(0,0,320*3, 240*3), null );


                        mTextureView.unlockCanvasAndPost(canvas);
                    }


                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            if (bmp != null) {
                Canvas canvas = mTextureView.lockCanvas();
                if (canvas != null) {
                    canvas.drawBitmap(bmp, (canvas.getWidth() - width) / 2, (canvas.getHeight() - height) / 2, null);
                    mTextureView.unlockCanvasAndPost(canvas);
                }
            }
        }
    }
}
