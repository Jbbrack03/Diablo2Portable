package com.diablo2portable;

import android.content.Intent;
import androidx.test.espresso.intent.Intents;
import androidx.test.ext.junit.rules.ActivityScenarioRule;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.filters.LargeTest;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
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
public class AssetBrowserTest {
    
    @Rule
    public ActivityScenarioRule<AssetBrowserActivity> activityRule = 
        new ActivityScenarioRule<>(AssetBrowserActivity.class);
    
    @Before
    public void setUp() {
        Intents.init();
    }
    
    @After
    public void tearDown() {
        Intents.release();
    }
    
    @Test
    public void testDisplayAssetCategories() {
        // Check that all asset categories are displayed
        onView(withId(R.id.category_characters))
            .check(matches(isDisplayed()));
        onView(withId(R.id.category_monsters))
            .check(matches(isDisplayed()));
        onView(withId(R.id.category_items))
            .check(matches(isDisplayed()));
        onView(withId(R.id.category_ui))
            .check(matches(isDisplayed()));
    }
}