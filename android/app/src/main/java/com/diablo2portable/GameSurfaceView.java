package com.diablo2portable;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.MotionEvent;

public class GameSurfaceView extends GLSurfaceView {
    private GameRenderer gameRenderer;
    private NativeEngine nativeEngine;

    public GameSurfaceView(Context context, NativeEngine nativeEngine) {
        super(context);
        this.nativeEngine = nativeEngine;
        init();
    }

    public GameSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    private void init() {
        // Use OpenGL ES 3.0
        setEGLContextClientVersion(3);
        
        // Create the renderer
        gameRenderer = new GameRenderer(nativeEngine);
        setRenderer(gameRenderer);
        
        // Only render when surface is dirty (for efficiency)
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (nativeEngine != null) {
            float x = event.getX();
            float y = event.getY();
            int action = event.getAction();
            
            // Forward touch events to native engine
            nativeEngine.onTouchEvent(x, y, action);
            
            // Request a render
            requestRender();
            return true;
        }
        
        return super.onTouchEvent(event);
    }
}