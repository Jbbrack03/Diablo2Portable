package com.diablo2portable;

import android.content.Context;
import android.content.res.AssetManager;

public class NativeEngine {
    static {
        System.loadLibrary("d2engine");
    }

    private long nativeHandle = 0;
    private boolean initialized = false;

    public NativeEngine() {
        nativeHandle = createEngine();
    }

    public boolean initialize(Context context) {
        if (nativeHandle == 0) {
            return false;
        }

        boolean result = initialize(nativeHandle);
        if (result) {
            // Get the path to extracted assets from OnboardingHelper
            OnboardingHelper onboardingHelper = new OnboardingHelper(context);
            String assetPath = onboardingHelper.getAssetPath();
            
            if (assetPath != null && !assetPath.isEmpty()) {
                // Use extracted assets
                result = loadAssets(nativeHandle, assetPath);
            } else {
                // Fallback to default location
                String defaultPath = context.getFilesDir().getAbsolutePath() + "/assets";
                result = loadAssets(nativeHandle, defaultPath);
            }
            initialized = result;
        }
        
        return result;
    }

    public void onSurfaceCreated(int width, int height) {
        if (nativeHandle != 0) {
            onSurfaceCreated(nativeHandle, width, height);
        }
    }

    public void onTouchEvent(float x, float y, int action) {
        if (nativeHandle != 0) {
            onTouchEvent(nativeHandle, x, y, action);
        }
    }

    public void renderFrame() {
        if (nativeHandle != 0) {
            renderFrame(nativeHandle);
        }
    }

    public void onGamepadInput(float leftX, float leftY, float rightX, float rightY, float leftTrigger, float rightTrigger) {
        if (nativeHandle != 0) {
            onGamepadInput(nativeHandle, leftX, leftY, rightX, rightY, leftTrigger, rightTrigger);
        }
    }

    public void onGamepadButton(int buttonCode, boolean pressed) {
        if (nativeHandle != 0) {
            onGamepadButton(nativeHandle, buttonCode, pressed);
        }
    }

    public void cleanup() {
        if (nativeHandle != 0) {
            destroyEngine(nativeHandle);
            nativeHandle = 0;
            initialized = false;
        }
    }

    public boolean isInitialized() {
        return initialized;
    }

    public long getEngineHandle() {
        return nativeHandle;
    }

    // Native method declarations
    public static native long createEngine();
    public static native void destroyEngine(long handle);
    public static native boolean initialize(long handle);
    public static native boolean loadAssets(long handle, String assetPath);
    public static native void onTouchEvent(long handle, float x, float y, int action);
    public static native void onSurfaceCreated(long handle, int width, int height);
    public static native void renderFrame(long handle);
    public static native void onGamepadInput(long handle, float leftX, float leftY, float rightX, float rightY, float leftTrigger, float rightTrigger);
    public static native void onGamepadButton(long handle, int buttonCode, boolean pressed);
}