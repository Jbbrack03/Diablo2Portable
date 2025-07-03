package com.diablo2portable;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

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
        
        // Start extraction in background
        // TODO: Implement actual extraction using OnboardingWizard JNI
        ProgressBar progressBar = findViewById(R.id.extraction_progress);
        TextView progressText = findViewById(R.id.progress_text);
        
        if (progressText != null) {
            progressText.setText("Extracting assets from " + selectedSource);
        }
    }
}