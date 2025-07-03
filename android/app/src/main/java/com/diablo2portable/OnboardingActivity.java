package com.diablo2portable;

import android.app.Activity;
import android.os.Bundle;
import android.os.AsyncTask;
import android.os.Handler;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

public class OnboardingActivity extends Activity {
    private String selectedSource;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_onboarding);
        
        Button continueButton = findViewById(R.id.continue_button);
        continueButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Navigate to file source selection screen
                setContentView(R.layout.fragment_file_source);
                setupFileSourceButtons();
            }
        });
    }
    
    private void setupFileSourceButtons() {
        Button localFiles = findViewById(R.id.option_local_files);
        if (localFiles != null) {
            localFiles.setOnClickListener(v -> {
                setSelectedSource("/storage/emulated/0/d2files");
                startAssetExtraction();
            });
        }
    }
    
    public void setSelectedSource(String source) {
        this.selectedSource = source;
    }
    
    public void startAssetExtraction() {
        // Show extraction progress screen
        setContentView(R.layout.fragment_extraction_progress);
        
        ProgressBar progressBar = findViewById(R.id.extraction_progress);
        TextView progressText = findViewById(R.id.progress_text);
        TextView currentFileText = findViewById(R.id.current_file);
        
        if (progressText != null) {
            progressText.setText("Extracting assets from " + selectedSource);
        }
        
        // Start extraction in background
        new AsyncTask<Void, Float, Boolean>() {
            @Override
            protected Boolean doInBackground(Void... params) {
                String outputPath = getFilesDir().getAbsolutePath() + "/assets";
                return OnboardingManager.extractAssets(selectedSource, outputPath);
            }
            
            @Override
            protected void onProgressUpdate(Float... values) {
                if (progressBar != null && values.length > 0) {
                    progressBar.setProgress((int)(values[0] * 100));
                }
                
                String currentFile = OnboardingManager.getCurrentFile();
                if (currentFileText != null && !currentFile.isEmpty()) {
                    currentFileText.setText("Current file: " + currentFile);
                }
            }
            
            @Override
            protected void onPostExecute(Boolean success) {
                if (success) {
                    Toast.makeText(OnboardingActivity.this, 
                        "Extraction complete!", Toast.LENGTH_SHORT).show();
                    // TODO: Navigate to main game
                    finish();
                } else {
                    Toast.makeText(OnboardingActivity.this, 
                        "Extraction failed!", Toast.LENGTH_LONG).show();
                }
            }
        }.execute();
        
        // Update progress periodically
        Handler handler = new Handler();
        Runnable progressUpdater = new Runnable() {
            @Override
            public void run() {
                float progress = OnboardingManager.getProgress();
                if (progressBar != null) {
                    progressBar.setProgress((int)(progress * 100));
                }
                
                if (progress < 1.0f) {
                    handler.postDelayed(this, 100); // Update every 100ms
                }
            }
        };
        handler.postDelayed(progressUpdater, 100);
    }
}