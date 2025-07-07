package com.diablo2portable;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.rule.ActivityTestRule;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;
import static androidx.test.espresso.Espresso.*;
import static androidx.test.espresso.action.ViewActions.*;
import static androidx.test.espresso.assertion.ViewAssertions.*;
import static androidx.test.espresso.matcher.ViewMatchers.*;
import static org.hamcrest.Matchers.*;
import static org.junit.Assert.*;
import android.view.View;
import android.view.accessibility.AccessibilityNodeInfo;
import java.util.ArrayList;
import java.util.List;

// Task 24.3: User Experience Testing
@RunWith(AndroidJUnit4.class)
public class UserExperienceTest {
    
    @Rule
    public ActivityTestRule<OnboardingActivity> activityRule = 
        new ActivityTestRule<>(OnboardingActivity.class);
    
    // Test 1: Onboarding completion time
    @Test
    public void testOnboardingCompletionTime() {
        long startTime = System.currentTimeMillis();
        
        // Wait for activity to be ready
        onView(withId(R.id.welcome_text)).check(matches(isDisplayed()));
        
        // Auto-detect assets (should be fast)
        onView(withId(R.id.auto_detect_button)).perform(click());
        
        // Wait for detection to complete (max 10 seconds)
        waitForCondition(() -> {
            OnboardingActivity activity = activityRule.getActivity();
            return activity != null && activity.hasDetectedAssets();
        }, 10000);
        
        // Start extraction
        onView(withId(R.id.start_extraction_button)).perform(click());
        
        // Wait for extraction to complete (max 2 minutes)
        waitForCondition(() -> {
            OnboardingActivity activity = activityRule.getActivity();
            return activity != null && activity.isExtractionComplete();
        }, 120000);
        
        long completionTime = System.currentTimeMillis() - startTime;
        
        // Onboarding should complete within 5 minutes for typical setup
        assertTrue("Onboarding took too long: " + completionTime + "ms",
            completionTime < 300000); // 5 minutes
    }
    
    // Test 2: Error recovery
    @Test
    public void testErrorRecovery() {
        OnboardingActivity activity = activityRule.getActivity();
        
        // Simulate extraction failure
        activity.runOnUiThread(() -> {
            activity.simulateExtractionError();
        });
        
        // Wait for UI to update
        try {
            Thread.sleep(500);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        
        // Should show clear error message
        onView(withId(R.id.error_message))
            .check(matches(isDisplayed()));
        
        // Should offer recovery options
        onView(withId(R.id.retry_button))
            .check(matches(isDisplayed()));
        onView(withId(R.id.help_button))
            .check(matches(isDisplayed()));
        
        // Recovery should work
        onView(withId(R.id.retry_button)).perform(click());
        
        // Wait for retry to start
        waitForCondition(() -> {
            return activity.getExtractionStatus() == OnboardingActivity.Status.IN_PROGRESS;
        }, 5000);
    }
    
    // Test 3: Accessibility compliance
    @Test
    public void testAccessibilityCompliance() {
        OnboardingActivity activity = activityRule.getActivity();
        
        // Find all interactive views
        List<View> interactiveViews = findAllInteractiveViews(activity.getWindow().getDecorView());
        
        for (View view : interactiveViews) {
            // Check if view has content description or text
            AccessibilityNodeInfo nodeInfo = view.createAccessibilityNodeInfo();
            boolean hasAccessibilityInfo = nodeInfo.getContentDescription() != null || 
                                          nodeInfo.getText() != null;
            
            assertTrue("View missing accessibility info: " + view.getId() + 
                      " (" + view.getClass().getSimpleName() + ")",
                      hasAccessibilityInfo);
        }
        
        // Verify minimum touch target size (48dp)
        float minTouchSize = 48 * activity.getResources().getDisplayMetrics().density;
        for (View view : interactiveViews) {
            if (view.isClickable() || view.isFocusable()) {
                assertTrue("View too small for touch: " + view.getId(),
                          view.getWidth() >= minTouchSize && view.getHeight() >= minTouchSize);
            }
        }
    }
    
    // Helper method to wait for a condition
    private void waitForCondition(Condition condition, long timeoutMs) {
        long endTime = System.currentTimeMillis() + timeoutMs;
        while (!condition.check() && System.currentTimeMillis() < endTime) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                break;
            }
        }
    }
    
    // Helper interface for conditions
    private interface Condition {
        boolean check();
    }
    
    // Helper method to find all interactive views
    private List<View> findAllInteractiveViews(View root) {
        List<View> views = new ArrayList<>();
        if (root instanceof android.view.ViewGroup) {
            android.view.ViewGroup group = (android.view.ViewGroup) root;
            for (int i = 0; i < group.getChildCount(); i++) {
                views.addAll(findAllInteractiveViews(group.getChildAt(i)));
            }
        }
        if (root.isClickable() || root.isFocusable() || root.isLongClickable()) {
            views.add(root);
        }
        return views;
    }
}