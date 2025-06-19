package com.diablo2portable;

import android.os.Bundle;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {
    private NativeEngine nativeEngine;
    private GameSurfaceView gameSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // Initialize native engine
        nativeEngine = new NativeEngine();
        
        // Create and set up the game surface view
        gameSurfaceView = new GameSurfaceView(this, nativeEngine);
        setContentView(gameSurfaceView);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        
        if (nativeEngine != null) {
            nativeEngine.cleanup();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (gameSurfaceView != null) {
            gameSurfaceView.onPause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (gameSurfaceView != null) {
            gameSurfaceView.onResume();
        }
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        // Handle gamepad button presses
        if ((event.getSource() & InputDevice.SOURCE_GAMEPAD) == InputDevice.SOURCE_GAMEPAD) {
            // Forward gamepad events to native engine
            if (nativeEngine != null) {
                // TODO: Implement native gamepad handling
                return true;
            }
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        // Handle gamepad button releases
        if ((event.getSource() & InputDevice.SOURCE_GAMEPAD) == InputDevice.SOURCE_GAMEPAD) {
            // Forward gamepad events to native engine
            if (nativeEngine != null) {
                // TODO: Implement native gamepad handling
                return true;
            }
        }
        return super.onKeyUp(keyCode, event);
    }

    @Override
    public boolean onGenericMotionEvent(MotionEvent event) {
        // Handle analog stick and trigger events
        if ((event.getSource() & InputDevice.SOURCE_JOYSTICK) == InputDevice.SOURCE_JOYSTICK
                && event.getAction() == MotionEvent.ACTION_MOVE) {
            
            // Process all historical movement samples in the batch
            final int historySize = event.getHistorySize();
            
            // Process historical samples
            for (int i = 0; i < historySize; i++) {
                processJoystickInput(event, i);
            }
            
            // Process current sample
            processJoystickInput(event, -1);
            
            return true;
        }
        return super.onGenericMotionEvent(event);
    }

    private void processJoystickInput(MotionEvent event, int historyPos) {
        if (nativeEngine == null) return;
        
        // Get axis values
        float x = getCenteredAxis(event, MotionEvent.AXIS_X, historyPos);
        float y = getCenteredAxis(event, MotionEvent.AXIS_Y, historyPos);
        float rx = getCenteredAxis(event, MotionEvent.AXIS_Z, historyPos);
        float ry = getCenteredAxis(event, MotionEvent.AXIS_RZ, historyPos);
        float lt = event.getAxisValue(MotionEvent.AXIS_LTRIGGER, historyPos);
        float rt = event.getAxisValue(MotionEvent.AXIS_RTRIGGER, historyPos);
        
        // TODO: Forward to native engine
        // nativeEngine.onGamepadInput(x, y, rx, ry, lt, rt);
    }

    private static float getCenteredAxis(MotionEvent event, int axis, int historyPos) {
        final InputDevice.MotionRange range = event.getDevice().getMotionRange(axis, event.getSource());
        
        // A joystick at rest does not always report an absolute position of (0,0).
        // Use a dead zone to filter out these small values.
        if (range != null) {
            final float flat = range.getFlat();
            final float value = historyPos < 0 ? event.getAxisValue(axis)
                    : event.getHistoricalAxisValue(axis, historyPos);
            
            // Ignore axis values that are within the dead zone
            if (Math.abs(value) > flat) {
                return value;
            }
        }
        return 0;
    }
}