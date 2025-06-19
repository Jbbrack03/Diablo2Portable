package com.diablo2portable;

import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;

import androidx.test.core.app.ActivityScenario;
import androidx.test.ext.junit.rules.ActivityScenarioRule;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertTrue;

@RunWith(AndroidJUnit4.class)
public class ControllerNavigationTest {

    @Rule
    public ActivityScenarioRule<MainActivity> activityRule = 
            new ActivityScenarioRule<>(MainActivity.class);

    private long downTime;

    @Before
    public void setUp() {
        downTime = System.currentTimeMillis();
    }

    @Test
    public void testGamepadDPadNavigation() {
        // Test D-pad navigation events
        activityRule.getScenario().onActivity(activity -> {
            // Simulate D-pad up press
            KeyEvent upEvent = new KeyEvent(downTime, System.currentTimeMillis(),
                    KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_DPAD_UP, 0,
                    0, 0, 0, KeyEvent.FLAG_FROM_SYSTEM, InputDevice.SOURCE_GAMEPAD);
            
            boolean handled = activity.dispatchKeyEvent(upEvent);
            assertTrue("D-pad up should be handled", handled);

            // Simulate D-pad up release
            KeyEvent upReleaseEvent = new KeyEvent(downTime, System.currentTimeMillis(),
                    KeyEvent.ACTION_UP, KeyEvent.KEYCODE_DPAD_UP, 0,
                    0, 0, 0, KeyEvent.FLAG_FROM_SYSTEM, InputDevice.SOURCE_GAMEPAD);
            
            handled = activity.dispatchKeyEvent(upReleaseEvent);
            assertTrue("D-pad up release should be handled", handled);
        });
    }

    @Test
    public void testGamepadButtonEvents() {
        // Test gamepad button events
        activityRule.getScenario().onActivity(activity -> {
            // Simulate A button press (typically mapped to BUTTON_A)
            KeyEvent buttonAEvent = new KeyEvent(downTime, System.currentTimeMillis(),
                    KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_BUTTON_A, 0,
                    0, 0, 0, KeyEvent.FLAG_FROM_SYSTEM, InputDevice.SOURCE_GAMEPAD);
            
            boolean handled = activity.dispatchKeyEvent(buttonAEvent);
            assertTrue("Button A should be handled", handled);

            // Simulate B button press
            KeyEvent buttonBEvent = new KeyEvent(downTime, System.currentTimeMillis(),
                    KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_BUTTON_B, 0,
                    0, 0, 0, KeyEvent.FLAG_FROM_SYSTEM, InputDevice.SOURCE_GAMEPAD);
            
            handled = activity.dispatchKeyEvent(buttonBEvent);
            assertTrue("Button B should be handled", handled);
        });
    }

    @Test
    public void testGamepadAnalogStickEvents() {
        // Test analog stick motion events
        activityRule.getScenario().onActivity(activity -> {
            // Create a motion event for left analog stick movement
            MotionEvent.PointerProperties[] pointerProperties = new MotionEvent.PointerProperties[1];
            pointerProperties[0] = new MotionEvent.PointerProperties();
            pointerProperties[0].id = 0;
            pointerProperties[0].toolType = MotionEvent.TOOL_TYPE_UNKNOWN;

            MotionEvent.PointerCoords[] pointerCoords = new MotionEvent.PointerCoords[1];
            pointerCoords[0] = new MotionEvent.PointerCoords();
            pointerCoords[0].setAxisValue(MotionEvent.AXIS_X, 0.5f); // Half right
            pointerCoords[0].setAxisValue(MotionEvent.AXIS_Y, -0.5f); // Half up

            MotionEvent motionEvent = MotionEvent.obtain(
                    downTime, System.currentTimeMillis(),
                    MotionEvent.ACTION_MOVE, 1,
                    pointerProperties, pointerCoords,
                    0, 0, 1.0f, 1.0f, 0, 0,
                    InputDevice.SOURCE_JOYSTICK, 0
            );

            boolean handled = activity.dispatchGenericMotionEvent(motionEvent);
            assertTrue("Analog stick motion should be handled", handled);
            
            motionEvent.recycle();
        });
    }

    @Test
    public void testGamepadTriggerEvents() {
        // Test trigger button events
        activityRule.getScenario().onActivity(activity -> {
            // Create a motion event for trigger press
            MotionEvent.PointerProperties[] pointerProperties = new MotionEvent.PointerProperties[1];
            pointerProperties[0] = new MotionEvent.PointerProperties();
            pointerProperties[0].id = 0;
            pointerProperties[0].toolType = MotionEvent.TOOL_TYPE_UNKNOWN;

            MotionEvent.PointerCoords[] pointerCoords = new MotionEvent.PointerCoords[1];
            pointerCoords[0] = new MotionEvent.PointerCoords();
            pointerCoords[0].setAxisValue(MotionEvent.AXIS_LTRIGGER, 1.0f); // Left trigger fully pressed
            pointerCoords[0].setAxisValue(MotionEvent.AXIS_RTRIGGER, 0.5f); // Right trigger half pressed

            MotionEvent motionEvent = MotionEvent.obtain(
                    downTime, System.currentTimeMillis(),
                    MotionEvent.ACTION_MOVE, 1,
                    pointerProperties, pointerCoords,
                    0, 0, 1.0f, 1.0f, 0, 0,
                    InputDevice.SOURCE_JOYSTICK, 0
            );

            boolean handled = activity.dispatchGenericMotionEvent(motionEvent);
            assertTrue("Trigger events should be handled", handled);
            
            motionEvent.recycle();
        });
    }
}