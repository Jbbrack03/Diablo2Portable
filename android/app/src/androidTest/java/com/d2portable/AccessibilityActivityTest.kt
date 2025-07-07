package com.d2portable

import androidx.test.espresso.Espresso.onView
import androidx.test.espresso.action.ViewActions.*
import androidx.test.espresso.assertion.ViewAssertions.matches
import androidx.test.espresso.matcher.ViewMatchers.*
import androidx.test.ext.junit.rules.ActivityScenarioRule
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.filters.LargeTest
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
@LargeTest
class AccessibilityActivityTest {

    @get:Rule
    val activityRule = ActivityScenarioRule(AccessibilityActivity::class.java)

    @Test
    fun testAccessibilitySettingsAreDisplayed() {
        // Check that the accessibility settings screen is displayed
        onView(withText("Accessibility Settings"))
            .check(matches(isDisplayed()))
        
        // Check text size option is visible
        onView(withText("Text Size"))
            .check(matches(isDisplayed()))
        
        // Check colorblind mode option is visible
        onView(withText("Colorblind Mode"))
            .check(matches(isDisplayed()))
        
        // Check controller remapping option is visible
        onView(withText("Button Remapping"))
            .check(matches(isDisplayed()))
    }
}