package com.diablo2portable;

import android.content.Intent;
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
import static org.hamcrest.Matchers.allOf;
import static org.hamcrest.Matchers.containsString;

@RunWith(AndroidJUnit4.class)
@LargeTest
public class FileBrowserTest {
    
    @Test
    public void testOpenFileBrowser() {
        ActivityScenario<FileBrowserActivity> scenario = 
            ActivityScenario.launch(FileBrowserActivity.class);
        
        // Should show current directory
        onView(withId(R.id.current_path))
            .check(matches(isDisplayed()));
        
        // Should show file list
        onView(withId(R.id.file_list))
            .check(matches(isDisplayed()));
        
        // Should have navigation controls
        onView(withId(R.id.up_button))
            .check(matches(isDisplayed()));
    }
    
    @Test
    public void testSelectMPQFile() {
        Intent intent = new Intent();
        intent.putExtra("filter_extension", ".mpq");
        
        ActivityScenario<FileBrowserActivity> scenario = 
            ActivityScenario.launch(FileBrowserActivity.class, intent);
        
        scenario.onActivity(activity -> {
            // Simulate having MPQ files in test directory
            activity.setTestMode(true);
            activity.addTestFile("/test/d2data.mpq");
            activity.addTestFile("/test/d2exp.mpq");
            activity.refreshFileList();
        });
        
        // Should show MPQ files
        onView(allOf(withId(R.id.file_name), withText("d2data.mpq")))
            .check(matches(isDisplayed()));
        
        // Click on MPQ file
        onView(allOf(withId(R.id.file_name), withText("d2data.mpq")))
            .perform(click());
        
        // Should enable select button
        onView(withId(R.id.select_button))
            .check(matches(isDisplayed()));
    }
}