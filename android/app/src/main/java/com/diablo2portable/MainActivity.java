package com.diablo2portable;

import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {
    private NativeEngine nativeEngine;
    private GameSurfaceView gameSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // Initialize native engine
        nativeEngine = new NativeEngine();
        
        // Create and set up the game surface view
        gameSurfaceView = new GameSurfaceView(this, nativeEngine);
        setContentView(gameSurfaceView);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        
        if (nativeEngine != null) {
            nativeEngine.cleanup();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (gameSurfaceView != null) {
            gameSurfaceView.onPause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (gameSurfaceView != null) {
            gameSurfaceView.onResume();
        }
    }
}