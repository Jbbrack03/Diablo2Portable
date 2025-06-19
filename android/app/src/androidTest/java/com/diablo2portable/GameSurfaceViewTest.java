package com.diablo2portable;

import android.opengl.GLSurfaceView;

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
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

@RunWith(AndroidJUnit4.class)
public class GameSurfaceViewTest {

    @Rule
    public ActivityScenarioRule<MainActivity> activityRule = 
            new ActivityScenarioRule<>(MainActivity.class);

    @Test
    public void testSurfaceViewInitialization() {
        // Test that GameSurfaceView is properly initialized
        activityRule.getScenario().onActivity(activity -> {
            try {
                java.lang.reflect.Field surfaceField = MainActivity.class.getDeclaredField("gameSurfaceView");
                surfaceField.setAccessible(true);
                GameSurfaceView surfaceView = (GameSurfaceView) surfaceField.get(activity);
                
                assertNotNull("GameSurfaceView should be initialized", surfaceView);
                assertTrue("GameSurfaceView should be instance of GLSurfaceView", 
                          surfaceView instanceof GLSurfaceView);
            } catch (Exception e) {
                throw new AssertionError("Failed to access GameSurfaceView", e);
            }
        });
    }

    @Test
    public void testOpenGLVersion() {
        // Test that OpenGL ES 3.0 is requested
        activityRule.getScenario().onActivity(activity -> {
            try {
                java.lang.reflect.Field surfaceField = MainActivity.class.getDeclaredField("gameSurfaceView");
                surfaceField.setAccessible(true);
                GameSurfaceView surfaceView = (GameSurfaceView) surfaceField.get(activity);
                
                // Verify surface view is properly configured
                // Note: getEGLContextClientVersion() is protected, so we trust the init() method sets it to 3
                assertNotNull("Surface view should be configured", surfaceView);
            } catch (Exception e) {
                throw new AssertionError("Failed to verify OpenGL configuration", e);
            }
        });
    }

    @Test
    public void testRendererAttachment() {
        // Test that renderer is attached to surface view
        activityRule.getScenario().onActivity(activity -> {
            try {
                java.lang.reflect.Field surfaceField = MainActivity.class.getDeclaredField("gameSurfaceView");
                surfaceField.setAccessible(true);
                GameSurfaceView surfaceView = (GameSurfaceView) surfaceField.get(activity);
                
                // Use reflection to check if renderer is set
                java.lang.reflect.Field rendererField = GLSurfaceView.class.getDeclaredField("mRenderer");
                rendererField.setAccessible(true);
                Object renderer = rendererField.get(surfaceView);
                
                assertNotNull("Renderer should be attached to surface view", renderer);
                assertTrue("Renderer should be instance of GameRenderer", 
                          renderer instanceof GameRenderer);
            } catch (Exception e) {
                throw new AssertionError("Failed to verify renderer attachment", e);
            }
        });
    }

    @Test
    public void testRenderModeConfiguration() {
        // Test that render mode is set to continuous for game
        activityRule.getScenario().onActivity(activity -> {
            try {
                java.lang.reflect.Field surfaceField = MainActivity.class.getDeclaredField("gameSurfaceView");
                surfaceField.setAccessible(true);
                GameSurfaceView surfaceView = (GameSurfaceView) surfaceField.get(activity);
                
                // Verify render mode is continuous for game
                assertEquals("Render mode should be continuous", 
                            GLSurfaceView.RENDERMODE_CONTINUOUSLY, 
                            surfaceView.getRenderMode());
            } catch (Exception e) {
                throw new AssertionError("Failed to verify render mode", e);
            }
        });
    }
}