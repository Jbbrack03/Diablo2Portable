package com.diablo2portable;

import androidx.test.core.app.ActivityScenario;
import androidx.test.ext.junit.rules.ActivityScenarioRule;
import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.assertion.ViewAssertions.matches;
import static androidx.test.espresso.matcher.ViewMatchers.isDisplayed;
import static androidx.test.espresso.matcher.ViewMatchers.withClassName;
import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

@RunWith(AndroidJUnit4.class)
public class MainActivityTest {

    @Rule
    public ActivityScenarioRule<MainActivity> activityRule = 
            new ActivityScenarioRule<>(MainActivity.class);

    @Test
    public void testActivityCreation() {
        // Test that activity is created successfully
        activityRule.getScenario().onActivity(activity -> {
            assertNotNull("Activity should be created", activity);
        });
    }

    @Test
    public void testNativeEngineInitialization() {
        // Test that native engine is initialized
        activityRule.getScenario().onActivity(activity -> {
            // Use reflection to access private field (for testing)
            try {
                java.lang.reflect.Field engineField = MainActivity.class.getDeclaredField("nativeEngine");
                engineField.setAccessible(true);
                NativeEngine engine = (NativeEngine) engineField.get(activity);
                
                assertNotNull("Native engine should be initialized", engine);
            } catch (Exception e) {
                throw new AssertionError("Failed to access native engine", e);
            }
        });
    }

    @Test
    public void testGameSurfaceViewCreation() {
        // Test that GameSurfaceView is created and displayed
        onView(withClassName(is(GameSurfaceView.class.getName())))
                .check(matches(isDisplayed()));
    }

    @Test
    public void testEngineRunning() {
        // Test that engine is running after activity creation
        activityRule.getScenario().onActivity(activity -> {
            try {
                java.lang.reflect.Field engineField = MainActivity.class.getDeclaredField("nativeEngine");
                engineField.setAccessible(true);
                NativeEngine engine = (NativeEngine) engineField.get(activity);
                
                // Verify engine handle is valid (non-zero)
                assertTrue("Engine should have valid handle", engine.getEngineHandle() != 0);
            } catch (Exception e) {
                throw new AssertionError("Failed to verify engine state", e);
            }
        });
    }
}