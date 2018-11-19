package com.example.native_activity;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.ColorSpaceTransform;
import android.hardware.camera2.params.RggbChannelVector;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.util.Log;
import android.util.Range;
import android.util.Size;
import android.view.Surface;
import android.view.TextureView;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by smith on 7/14/16
 */

public class UseCamera2API {

    private TextureView mTextureView;
    private String mCameraId = "0";
    private CameraDevice mCameraDevice;
    private CameraCaptureSession mCaptureSession;
    private NativeLoader nativeLoader;

    private CaptureRequest.Builder mPreviewRequestBuilder;
    private CaptureRequest mPreviewRequest;


    private  int width;
    private int height;
    private Activity activity;
    private static final String TAG = "Camera2";

    public UseCamera2API(int width, int height, NativeLoader nativeLoader, TextureView mTextureView) {
        this.width = width;
        this.height = height;
        this.nativeLoader = nativeLoader;
        this.activity = nativeLoader;
        this.mTextureView = mTextureView;
        mBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        //mBitmap = Bitmap.createBitmap(height, width, Bitmap.Config.ARGB_8888);
    }


    private Matrix configureTransform(int viewWidth, int viewHeight ) {
        if (null == mTextureView ) {
            return null;
        }
        int rotation = activity.getWindowManager().getDefaultDisplay().getRotation();
        android.graphics.Matrix matrix = new Matrix();
        RectF viewRect = new RectF(0, 0, viewWidth, viewHeight);
        RectF bufferRect = new RectF(0, 0, width, height);
        float centerX = viewRect.centerX();
        float centerY = viewRect.centerY();
        if (Surface.ROTATION_90 == rotation || Surface.ROTATION_270 == rotation) {
            bufferRect.offset(centerX - bufferRect.centerX(), centerY - bufferRect.centerY());
            matrix.setRectToRect(viewRect, bufferRect, android.graphics.Matrix.ScaleToFit.FILL);
            float scale = Math.max(
                    (float) viewHeight / height,
                    (float) viewWidth / width);
            matrix.postScale(scale, scale, centerX, centerY);
            matrix.postRotate(90 * (rotation - 2), centerX, centerY);
        } else if (Surface.ROTATION_180 == rotation) {
            matrix.postRotate(180, centerX, centerY);
        }
        return matrix;
    }

    public void setupCamera()  {

        CameraManager manager = (CameraManager) activity.getSystemService(Context.CAMERA_SERVICE);
        try {
            manager.openCamera(mCameraId, mStateCallback, null);

        } catch (SecurityException sec) {
            Log.d(TAG, sec.toString());
        }
        catch (Exception e) {
            Log.e(TAG, e.toString());
        }
    }

    private final CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {

        @Override
        public void onOpened(CameraDevice cameraDevice) {
            //mCameraOpenCloseLock.release();
            mCameraDevice = cameraDevice;
            createCameraPreviewSession();
        }

        @Override
        public void onDisconnected( CameraDevice cameraDevice) {
            //mCameraOpenCloseLock.release();
            cameraDevice.close();
            mCameraDevice = null;
        }

        @Override
        public void onError( CameraDevice cameraDevice, int error) {
            //mCameraOpenCloseLock.release();
            cameraDevice.close();
            mCameraDevice = null;
        }

    };

    private void createCameraPreviewSession() {
        CameraManager manager = (CameraManager) activity.getSystemService(Context.CAMERA_SERVICE);

        SurfaceTexture texture = mTextureView.getSurfaceTexture();
        assert texture != null;

        // We configure the size of default buffer to be the size of camera preview we want.
        //texture.setDefaultBufferSize(630, 640);

        // This is the output Surface we need to start preview.
        Surface surface = new Surface(texture);
        try {
            CameraCharacteristics characteristics
                    = manager.getCameraCharacteristics(mCameraId);

            StreamConfigurationMap configs = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);

            assert configs != null;
            Size[] sizes = configs.getOutputSizes(ImageFormat.YV12);
            mImageReader = ImageReader.newInstance(width, height, ImageFormat.YV12, 2);

            mImageReader.setOnImageAvailableListener(mOnImageAvailableListener, null);

            Surface rgbCaptureSurface = mImageReader.getSurface();
            List<Surface> surfaces = new ArrayList<Surface>();
            surfaces.add(rgbCaptureSurface);

            //surfaces.add(surface);

            mPreviewRequestBuilder
                    = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            //mPreviewRequestBuilder.addTarget(surface);

            mPreviewRequestBuilder.addTarget(rgbCaptureSurface);

            mCameraDevice.createCaptureSession(surfaces, new CameraCaptureSession.StateCallback() {


                @Override
                public void onConfigured(CameraCaptureSession cameraCaptureSession) {
                    // The camera is already closed
                    if (null == mCameraDevice) {
                        return;
                    }

                    // When the session is ready, we start displaying the preview.
                    mCaptureSession = cameraCaptureSession;
                    try {
                        // Auto focus should be continuous for camera preview.

                        mPreviewRequestBuilder.set(CaptureRequest.BLACK_LEVEL_LOCK, false);


                        mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AWB_LOCK, false);
                        mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AWB_MODE, CaptureRequest.CONTROL_AWB_MODE_AUTO );

                        mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_LOCK, false);
                        mPreviewRequestBuilder.get(CaptureRequest.CONTROL_AE_EXPOSURE_COMPENSATION);
                        //mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_EXPOSURE_COMPENSATION, 12);
                        //mPreviewRequestBuilder.set(CaptureRequest.COLOR_CORRECTION_GAINS, new RggbChannelVector(86, 86, 86, 86));

                        mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, Range.create(0, 30));


                        mPreviewRequestBuilder.set(CaptureRequest.COLOR_CORRECTION_MODE, CaptureRequest.CONTROL_AE_MODE_ON);

                        //mPreviewRequestBuilder.set(CaptureRequest.COLOR_CORRECTION_, CaptureRequest.COLOR_CORRECTION_ABERRATION_MODE_FAST);

                        mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_MODE, CaptureRequest.CONTROL_AE_MODE_ON);

                        // Finally, we start displaying the camera preview.
                        mPreviewRequest = mPreviewRequestBuilder.build();
                        mCaptureSession.setRepeatingRequest(mPreviewRequest,
                                mCaptureCallback, null);
                    } catch (CameraAccessException e) {
                        e.printStackTrace();
                    }
                }

                @Override
                public void onConfigureFailed(
                        CameraCaptureSession cameraCaptureSession) {

                    Log.e(TAG, "Failed");
                }
            }, null);

        } catch (Exception e) {
            Log.e(TAG, e.toString());
        }
    }

    private boolean startedSlam = false;

    private Bitmap mBitmap;

    private ImageReader mImageReader;
    private final ImageReader.OnImageAvailableListener mOnImageAvailableListener  = new ImageReader.OnImageAvailableListener() {

        @Override
        public void onImageAvailable(ImageReader reader) {
            Image image;
            while (true) {
                image = reader.acquireLatestImage();

                if (image == null) return;

                Image.Plane Y = image.getPlanes()[0];
                Image.Plane U = image.getPlanes()[1];
                Image.Plane V = image.getPlanes()[2];

                int Yb = Y.getBuffer().remaining();
                int Ub = U.getBuffer().remaining();
                int Vb = V.getBuffer().remaining();

                byte[] data = new byte[Yb + Ub + Vb];


                Y.getBuffer().get(data, 0, Yb);
                U.getBuffer().get(data, Yb, Ub);
                V.getBuffer().get(data, Yb + Ub, Vb);


                if (!startedSlam) {
                    startedSlam = true;
                    (new Thread() {
                        public void run() {
                            nativeLoader.startSLAM();
                        }
                    }).start();

                }
                nativeLoader.pushImage(width, height, data);

                int[] rgba = new int[width*height];
                nativeLoader.getImage(rgba);

                //nativeLoader.convertImage(width, height, data, rgba);

                Bitmap bmp = mBitmap;
                bmp.setPixels(rgba, 0, width, 0, 0, width, height);

                Canvas canvas = mTextureView.lockCanvas();

                if (canvas != null) {
                    //
                    // canvas.drawBitmap(bmp, 0, 0, null );//configureTransform(width, height),  null);
                    //canvas.drawBitmap(bmp, configureTransform(width, height),  null);
                    canvas.drawBitmap(bmp, new Rect(0,0,320,240), new Rect(0,0,320*3, 240*3), null );
                    //canvas.drawBitmap(bmp, new Rect(0,0,240,320), new Rect(0,0,480*4, 640*4), null );

                    //canvas.drawBitmap(bmp, (canvas.getWidth() - 320) / 2, (canvas.getHeight() - 240) / 2, null);

                    mTextureView.unlockCanvasAndPost(canvas);
                }

                image.close();

            }
        }
    };

    private CameraCaptureSession.CaptureCallback mCaptureCallback
            = new CameraCaptureSession.CaptureCallback() {

        private void process(CaptureResult result) {

        }

        @Override
        public void onCaptureProgressed(CameraCaptureSession session,
                                        CaptureRequest request,
                                        CaptureResult partialResult) {
            process(partialResult);
        }

        @Override
        public void onCaptureCompleted(CameraCaptureSession session,
                                       CaptureRequest request,
                                       TotalCaptureResult result) {
            process(result);
        }
    };


}
