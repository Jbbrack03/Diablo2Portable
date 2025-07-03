package com.diablo2portable;

import android.content.Context;
import android.content.SharedPreferences;
import java.io.File;

/**
 * Helper class to manage onboarding state
 */
public class OnboardingHelper {
    private static final String PREFS_NAME = "d2portable_prefs";
    private static final String KEY_ONBOARDING_COMPLETE = "onboarding_complete";
    private static final String KEY_ASSET_PATH = "asset_path";
    
    private final SharedPreferences prefs;
    private final Context context;
    
    public OnboardingHelper(Context context) {
        this.context = context;
        this.prefs = context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE);
    }
    
    /**
     * Check if this is the first run of the app
     */
    public boolean isFirstRun() {
        return !prefs.getBoolean(KEY_ONBOARDING_COMPLETE, false) || !hasValidAssets();
    }
    
    /**
     * Mark onboarding as complete
     */
    public void setOnboardingComplete() {
        prefs.edit().putBoolean(KEY_ONBOARDING_COMPLETE, true).apply();
    }
    
    /**
     * Save the path to extracted assets
     */
    public void setAssetPath(String path) {
        prefs.edit().putString(KEY_ASSET_PATH, path).apply();
    }
    
    /**
     * Get the saved asset path
     */
    public String getAssetPath() {
        return prefs.getString(KEY_ASSET_PATH, null);
    }
    
    /**
     * Check if we have valid extracted assets
     */
    public boolean hasValidAssets() {
        String assetPath = getAssetPath();
        if (assetPath == null) {
            // Check default location
            File defaultAssets = new File(context.getFilesDir(), "assets");
            if (defaultAssets.exists()) {
                // Check for required files
                return new File(defaultAssets, "d2data").exists() &&
                       new File(defaultAssets, "d2exp").exists();
            }
            return false;
        }
        
        File assetDir = new File(assetPath);
        return assetDir.exists() && assetDir.isDirectory();
    }
}