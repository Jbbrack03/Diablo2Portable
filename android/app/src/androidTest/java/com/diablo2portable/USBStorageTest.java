package com.diablo2portable;

import androidx.test.core.app.ActivityScenario;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.filters.LargeTest;

import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.List;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.action.ViewActions.click;
import static androidx.test.espresso.assertion.ViewAssertions.matches;
import static androidx.test.espresso.matcher.ViewMatchers.isDisplayed;
import static androidx.test.espresso.matcher.ViewMatchers.withId;
import static androidx.test.espresso.matcher.ViewMatchers.withText;
import static org.hamcrest.Matchers.not;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

@RunWith(AndroidJUnit4.class)
@LargeTest
public class USBStorageTest {
    
    @Test
    public void testUSBStorageButtonOpensDeviceList() {
        ActivityScenario<OnboardingActivity> scenario = 
            ActivityScenario.launch(OnboardingActivity.class);
        
        // Navigate to file source screen
        onView(withId(R.id.continue_button)).perform(click());
        
        // Click USB storage option
        onView(withId(R.id.option_usb_storage)).perform(click());
        
        // Should show USB device list instead of toast
        onView(withId(R.id.usb_device_list))
            .check(matches(isDisplayed()));
    }
}