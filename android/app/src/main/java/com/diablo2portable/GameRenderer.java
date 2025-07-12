package com.diablo2portable;

import android.content.Context;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GameRenderer implements GLSurfaceView.Renderer {
    private NativeEngine nativeEngine;
    private Context context;
    private boolean surfaceCreated = false;

    public GameRenderer(NativeEngine nativeEngine, Context context) {
        this.nativeEngine = nativeEngine;
        this.context = context;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        // Initialize OpenGL state
        GLES30.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        GLES30.glEnable(GLES30.GL_DEPTH_TEST);
        GLES30.glEnable(GLES30.GL_BLEND);
        GLES30.glBlendFunc(GLES30.GL_SRC_ALPHA, GLES30.GL_ONE_MINUS_SRC_ALPHA);
        
        surfaceCreated = true;
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        // Set up viewport
        GLES30.glViewport(0, 0, width, height);
        
        // Initialize native engine if not already done
        if (nativeEngine != null && !nativeEngine.isInitialized()) {
            nativeEngine.initialize(context);
        }
        
        // Notify native engine of surface dimensions
        if (nativeEngine != null && surfaceCreated) {
            nativeEngine.onSurfaceCreated(width, height);
        }
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        // Clear the screen
        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT | GLES30.GL_DEPTH_BUFFER_BIT);
        
        // Render the native game frame
        if (nativeEngine != null && surfaceCreated) {
            nativeEngine.renderFrame();
        }
    }
}