package com.diablo2portable;

import androidx.test.core.app.ActivityScenario;
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

@RunWith(AndroidJUnit4.class)
@LargeTest
public class HelpSystemTest {
    
    @Test
    public void testHelpLinkOnWelcomeScreen() {
        ActivityScenario<OnboardingActivity> scenario = 
            ActivityScenario.launch(OnboardingActivity.class);
        
        // Click help link
        onView(withId(R.id.help_link)).perform(click());
        
        // Should show help dialog with title
        onView(withText("Horadric Wisdom"))
            .check(matches(isDisplayed()));
        
        // Should show required files section
        onView(withText("Required Files"))
            .check(matches(isDisplayed()));
        
        // Should show close button
        onView(withId(R.id.close_help_button))
            .check(matches(isDisplayed()));
    }
    
    @Test
    public void testHelpButtonOnErrorScreen() {
        ActivityScenario<OnboardingActivity> scenario = 
            ActivityScenario.launch(OnboardingActivity.class);
        
        scenario.onActivity(activity -> {
            // Simulate extraction error
            activity.simulateExtractionError(new Exception("Test error"));
        });
        
        // Click help button
        onView(withId(R.id.help_button)).perform(click());
        
        // Should show help dialog
        onView(withText("Horadric Wisdom"))
            .check(matches(isDisplayed()));
    }
}