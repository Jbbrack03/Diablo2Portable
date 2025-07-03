package com.diablo2portable;

import android.content.SharedPreferences;
import androidx.test.core.app.ActivityScenario;
import androidx.test.core.app.ApplicationProvider;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.filters.LargeTest;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.assertFalse;

@RunWith(AndroidJUnit4.class)
@LargeTest
public class OnboardingCompletionTest {
    
    @Before
    public void setUp() {
        // Clear any previous onboarding state
        SharedPreferences prefs = ApplicationProvider.getApplicationContext()
            .getSharedPreferences("d2portable_prefs", 0);
        prefs.edit().clear().commit();
    }
    
    @Test
    public void testFirstRunDetection() {
        OnboardingHelper helper = new OnboardingHelper(
            ApplicationProvider.getApplicationContext());
        
        // Should detect first run
        assertTrue(helper.isFirstRun());
        
        // After completing onboarding, should not be first run
        helper.setOnboardingComplete();
        assertFalse(helper.isFirstRun());
    }
    
    @Test
    public void testMainActivityStartsOnboardingOnFirstRun() {
        // Set up as first run
        OnboardingHelper helper = new OnboardingHelper(
            ApplicationProvider.getApplicationContext());
        assertTrue(helper.isFirstRun());
        
        // Launch MainActivity
        ActivityScenario<MainActivity> scenario = 
            ActivityScenario.launch(MainActivity.class);
        
        scenario.onActivity(activity -> {
            // Should have started onboarding
            assertTrue(activity.isFinishing());
        });
    }
    
    @Test
    public void testMainActivityStartsGameAfterOnboarding() {
        // Mark onboarding as complete
        OnboardingHelper helper = new OnboardingHelper(
            ApplicationProvider.getApplicationContext());
        helper.setOnboardingComplete();
        
        // Launch MainActivity
        ActivityScenario<MainActivity> scenario = 
            ActivityScenario.launch(MainActivity.class);
        
        scenario.onActivity(activity -> {
            // Should not be finishing - game should start
            assertFalse(activity.isFinishing());
        });
    }
}