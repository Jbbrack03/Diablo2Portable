package com.diablo2portable;

import androidx.test.core.app.ActivityScenario;
import androidx.test.espresso.matcher.ViewMatchers;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.filters.LargeTest;

import org.junit.Test;
import org.junit.runner.RunWith;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.action.ViewActions.click;
import static androidx.test.espresso.assertion.ViewAssertions.matches;
import static androidx.test.espresso.matcher.ViewMatchers.isDisplayed;
import static androidx.test.espresso.matcher.ViewMatchers.withId;
import static androidx.test.espresso.matcher.ViewMatchers.withText;
import static org.hamcrest.Matchers.containsString;
import static org.junit.Assert.assertTrue;

@RunWith(AndroidJUnit4.class)
@LargeTest
public class OnboardingActivityTest {
    
    @Test
    public void testWelcomeScreen() {
        // Launch the onboarding activity
        ActivityScenario<OnboardingActivity> scenario = 
            ActivityScenario.launch(OnboardingActivity.class);
        
        // Check that welcome title is displayed
        onView(withId(R.id.welcome_title))
            .check(matches(withText("Welcome to Diablo II Portable")));
        
        // Check that continue button is displayed
        onView(withId(R.id.continue_button))
            .check(matches(isDisplayed()));
    }
    
    @Test
    public void testFileSourceSelection() {
        ActivityScenario<OnboardingActivity> scenario = 
            ActivityScenario.launch(OnboardingActivity.class);
        
        // Navigate to file source screen
        onView(withId(R.id.continue_button)).perform(click());
        
        // Should show source options
        onView(withId(R.id.option_local_files))
            .check(matches(isDisplayed()));
        onView(withId(R.id.option_usb_storage))
            .check(matches(isDisplayed()));
        onView(withId(R.id.option_network_location))
            .check(matches(isDisplayed()));
    }
    
    @Test
    public void testAssetExtractionProgress() {
        ActivityScenario<OnboardingActivity> scenario = 
            ActivityScenario.launch(OnboardingActivity.class);
        
        scenario.onActivity(activity -> {
            // Simulate file selection and start extraction
            activity.setSelectedSource("/storage/emulated/0/d2files");
            activity.startAssetExtraction();
        });
        
        // Progress should be displayed
        onView(withId(R.id.extraction_progress))
            .check(matches(isDisplayed()));
        
        onView(withId(R.id.progress_text))
            .check(matches(withText(containsString("Extracting"))));
    }
}