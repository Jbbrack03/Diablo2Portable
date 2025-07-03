package com.diablo2portable;

import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;

/**
 * AssetBrowserActivity - Visual browser for extracted game assets
 * 
 * This activity displays categorized game assets with thumbnails,
 * allowing users to browse sprites, preview animations, and view
 * asset metadata.
 */
public class AssetBrowserActivity extends AppCompatActivity {
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_asset_browser);
    }
}