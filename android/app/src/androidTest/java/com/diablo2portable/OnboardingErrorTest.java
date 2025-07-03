package com.diablo2portable;

import androidx.test.core.app.ActivityScenario;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.filters.LargeTest;

import org.junit.Test;
import org.junit.runner.RunWith;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.assertion.ViewAssertions.matches;
import static androidx.test.espresso.matcher.ViewMatchers.isDisplayed;
import static androidx.test.espresso.matcher.ViewMatchers.withId;
import static androidx.test.espresso.matcher.ViewMatchers.withText;
import static org.hamcrest.Matchers.containsString;

@RunWith(AndroidJUnit4.class)
@LargeTest
public class OnboardingErrorTest {
    
    @Test
    public void testExtractionErrorHandling() {
        ActivityScenario<OnboardingActivity> scenario = 
            ActivityScenario.launch(OnboardingActivity.class);
        
        scenario.onActivity(activity -> {
            // Simulate extraction error
            activity.simulateExtractionError(new Exception("Insufficient storage space"));
        });
        
        // Should show error message
        onView(withId(R.id.error_message))
            .check(matches(isDisplayed()));
        
        onView(withId(R.id.error_message))
            .check(matches(withText(containsString("storage space"))));
        
        // Should show retry button
        onView(withId(R.id.retry_button))
            .check(matches(isDisplayed()));
        
        // Should show help button
        onView(withId(R.id.help_button))
            .check(matches(isDisplayed()));
    }
    
    @Test
    public void testMissingFilesError() {
        ActivityScenario<OnboardingActivity> scenario = 
            ActivityScenario.launch(OnboardingActivity.class);
        
        scenario.onActivity(activity -> {
            // Simulate missing files error
            String[] missingFiles = {"d2sfx.mpq", "d2music.mpq"};
            activity.showMissingFilesError(missingFiles);
        });
        
        // Should show missing files message
        onView(withId(R.id.error_message))
            .check(matches(withText(containsString("Missing required files"))));
        
        // Should list missing files
        onView(withId(R.id.missing_files_list))
            .check(matches(isDisplayed()));
    }
}