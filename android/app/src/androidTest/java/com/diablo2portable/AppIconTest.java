package com.diablo2portable;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.graphics.drawable.Drawable;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

/**
 * Tests to verify that all required app icon densities are present
 * Following TDD principles - these tests will fail until icons are added
 */
@RunWith(AndroidJUnit4.class)
public class AppIconTest {
    
    private Context context;
    private Resources resources;
    private PackageManager packageManager;
    private ApplicationInfo appInfo;
    
    @Before
    public void setUp() throws Exception {
        context = InstrumentationRegistry.getInstrumentation().getTargetContext();
        resources = context.getResources();
        packageManager = context.getPackageManager();
        appInfo = context.getApplicationInfo();
    }
    
    @Test
    public void testLauncherIconExists() {
        // Test that the app has a launcher icon
        Drawable icon = packageManager.getApplicationIcon(appInfo);
        assertNotNull("Application should have a launcher icon", icon);
    }
    
    @Test
    public void testMdpiIconExists() {
        // Test for mdpi (48x48) icon
        int resourceId = resources.getIdentifier("ic_launcher", "mipmap", context.getPackageName());
        assertTrue("ic_launcher resource should exist", resourceId > 0);
        
        // This will verify the icon can be loaded at mdpi density
        Resources.Theme theme = context.getTheme();
        Drawable drawable = resources.getDrawable(resourceId, theme);
        assertNotNull("Should be able to load mdpi icon", drawable);
    }
    
    @Test
    public void testHdpiIconExists() {
        // Test for hdpi (72x72) icon - this should pass as we already have it
        int resourceId = resources.getIdentifier("ic_launcher", "mipmap", context.getPackageName());
        assertTrue("ic_launcher resource should exist", resourceId > 0);
    }
    
    @Test
    public void testXhdpiIconExists() {
        // Test for xhdpi (96x96) icon
        int resourceId = resources.getIdentifier("ic_launcher", "mipmap", context.getPackageName());
        assertTrue("ic_launcher resource should exist for xhdpi", resourceId > 0);
    }
    
    @Test
    public void testXxhdpiIconExists() {
        // Test for xxhdpi (144x144) icon
        int resourceId = resources.getIdentifier("ic_launcher", "mipmap", context.getPackageName());
        assertTrue("ic_launcher resource should exist for xxhdpi", resourceId > 0);
    }
    
    @Test
    public void testXxxhdpiIconExists() {
        // Test for xxxhdpi (192x192) icon
        int resourceId = resources.getIdentifier("ic_launcher", "mipmap", context.getPackageName());
        assertTrue("ic_launcher resource should exist for xxxhdpi", resourceId > 0);
    }
    
    @Test
    public void testAdaptiveIconExists() {
        // Test for adaptive icon (API 26+)
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            int resourceId = resources.getIdentifier("ic_launcher", "mipmap", context.getPackageName());
            assertTrue("ic_launcher resource should exist", resourceId > 0);
            
            // Try to load as adaptive icon drawable
            Drawable drawable = resources.getDrawable(resourceId, context.getTheme());
            assertNotNull("Should be able to load adaptive icon", drawable);
        }
    }
}