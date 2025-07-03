package com.diablo2portable;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.AsyncTask;
import android.os.Handler;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.LinearLayout;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.EditText;

public class OnboardingActivity extends Activity {
    private static final int REQUEST_SELECT_MPQ = 1;
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
                // Open file browser to select MPQ files
                Intent intent = new Intent(this, FileBrowserActivity.class);
                intent.putExtra("filter_extension", ".mpq");
                startActivityForResult(intent, REQUEST_SELECT_MPQ);
            });
        }
        
        Button usbStorage = findViewById(R.id.option_usb_storage);
        if (usbStorage != null) {
            usbStorage.setOnClickListener(v -> {
                // Show USB device selection screen
                showUSBDevices();
            });
        }
        
        Button networkLocation = findViewById(R.id.option_network_location);
        if (networkLocation != null) {
            networkLocation.setOnClickListener(v -> {
                // Show network location screen
                showNetworkLocation();
            });
        }
    }
    
    private void showUSBDevices() {
        // Show USB devices screen
        setContentView(R.layout.fragment_usb_devices);
        
        LinearLayout deviceList = findViewById(R.id.usb_device_list);
        TextView noDevicesText = findViewById(R.id.no_devices_text);
        Button refreshButton = findViewById(R.id.refresh_button);
        
        if (refreshButton != null) {
            refreshButton.setOnClickListener(v -> refreshUSBDevices());
        }
        
        // Refresh devices on screen load
        refreshUSBDevices();
    }
    
    private void refreshUSBDevices() {
        LinearLayout deviceList = findViewById(R.id.usb_device_list);
        TextView noDevicesText = findViewById(R.id.no_devices_text);
        
        if (deviceList == null || noDevicesText == null) return;
        
        // Clear existing device list
        deviceList.removeAllViews();
        
        // Get USB devices from native code
        String[] usbDevices = OnboardingManager.detectUSBStorage();
        
        if (usbDevices == null || usbDevices.length == 0) {
            // No devices found
            noDevicesText.setVisibility(View.VISIBLE);
            deviceList.setVisibility(View.GONE);
        } else {
            // Show device list
            noDevicesText.setVisibility(View.GONE);
            deviceList.setVisibility(View.VISIBLE);
            
            // Add each USB device as a clickable item
            for (String deviceInfo : usbDevices) {
                // Device info format: "path|label|totalSpace|freeSpace"
                String[] parts = deviceInfo.split("\\|");
                if (parts.length >= 2) {
                    String path = parts[0];
                    String label = parts[1];
                    
                    // Create device button
                    Button deviceButton = new Button(this);
                    deviceButton.setText(label + "\n" + path);
                    deviceButton.setLayoutParams(new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.MATCH_PARENT,
                        LinearLayout.LayoutParams.WRAP_CONTENT
                    ));
                    deviceButton.setPadding(16, 16, 16, 16);
                    
                    deviceButton.setOnClickListener(v -> {
                        // User selected this USB device
                        setSelectedSource(path);
                        startAssetExtraction();
                    });
                    
                    deviceList.addView(deviceButton);
                }
            }
        }
    }
    
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        
        if (requestCode == REQUEST_SELECT_MPQ && resultCode == RESULT_OK) {
            String selectedFile = data.getStringExtra("selected_file");
            if (selectedFile != null) {
                // Get directory containing the MPQ file
                java.io.File file = new java.io.File(selectedFile);
                setSelectedSource(file.getParent());
                startAssetExtraction();
            }
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
                    
                    // Mark onboarding as complete
                    OnboardingHelper helper = new OnboardingHelper(OnboardingActivity.this);
                    helper.setOnboardingComplete();
                    helper.setAssetPath(getFilesDir().getAbsolutePath() + "/assets");
                    
                    // Start main game
                    Intent intent = new Intent(OnboardingActivity.this, MainActivity.class);
                    startActivity(intent);
                    finish();
                } else {
                    // Check for missing files
                    String[] missingFiles = OnboardingManager.getMissingFiles();
                    if (missingFiles != null && missingFiles.length > 0) {
                        showMissingFilesError(missingFiles);
                    } else {
                        simulateExtractionError(new Exception("Asset extraction failed"));
                    }
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
    
    /**
     * Show extraction error UI
     */
    public void simulateExtractionError(Exception error) {
        runOnUiThread(() -> {
            setContentView(R.layout.fragment_extraction_error);
            
            TextView errorMessage = findViewById(R.id.error_message);
            TextView errorDetails = findViewById(R.id.error_details);
            
            if (errorMessage != null) {
                errorMessage.setText("Extraction failed");
            }
            
            if (errorDetails != null && error != null) {
                errorDetails.setText(error.getMessage());
            }
            
            setupErrorButtons();
        });
    }
    
    /**
     * Show missing files error
     */
    public void showMissingFilesError(String[] missingFiles) {
        runOnUiThread(() -> {
            setContentView(R.layout.fragment_extraction_error);
            
            TextView errorMessage = findViewById(R.id.error_message);
            LinearLayout missingFilesList = findViewById(R.id.missing_files_list);
            
            if (errorMessage != null) {
                errorMessage.setText("Missing required files");
            }
            
            if (missingFilesList != null && missingFiles != null) {
                missingFilesList.setVisibility(View.VISIBLE);
                missingFilesList.removeAllViews();
                
                for (String file : missingFiles) {
                    TextView fileText = new TextView(this);
                    fileText.setText("• " + file);
                    fileText.setPadding(16, 4, 16, 4);
                    missingFilesList.addView(fileText);
                }
            }
            
            setupErrorButtons();
        });
    }
    
    private void setupErrorButtons() {
        Button retryButton = findViewById(R.id.retry_button);
        if (retryButton != null) {
            retryButton.setOnClickListener(v -> {
                // Go back to file source selection
                setContentView(R.layout.fragment_file_source);
                setupFileSourceButtons();
            });
        }
        
        Button helpButton = findViewById(R.id.help_button);
        if (helpButton != null) {
            helpButton.setOnClickListener(v -> {
                Toast.makeText(this, 
                    "Please ensure you have all Diablo II MPQ files in the selected directory", 
                    Toast.LENGTH_LONG).show();
            });
        }
    }
    
    private void showNetworkLocation() {
        // Show network location configuration screen
        setContentView(R.layout.fragment_network_location);
        
        Spinner protocolSpinner = findViewById(R.id.protocol_spinner);
        EditText hostEdit = findViewById(R.id.host_edit);
        EditText shareEdit = findViewById(R.id.share_edit);
        EditText usernameEdit = findViewById(R.id.username_edit);
        EditText passwordEdit = findViewById(R.id.password_edit);
        Button connectButton = findViewById(R.id.connect_button);
        ProgressBar connectionProgress = findViewById(R.id.connection_progress);
        TextView connectionStatus = findViewById(R.id.connection_status);
        
        if (connectButton != null) {
            connectButton.setOnClickListener(v -> {
                // Get network location details
                String host = hostEdit != null ? hostEdit.getText().toString() : "";
                String share = shareEdit != null ? shareEdit.getText().toString() : "";
                String username = usernameEdit != null ? usernameEdit.getText().toString() : "";
                String password = passwordEdit != null ? passwordEdit.getText().toString() : "";
                int protocolIndex = protocolSpinner != null ? protocolSpinner.getSelectedItemPosition() : 0;
                
                if (host.isEmpty() || share.isEmpty()) {
                    Toast.makeText(this, "Please enter host and share path", Toast.LENGTH_SHORT).show();
                    return;
                }
                
                // Show progress
                if (connectionProgress != null) connectionProgress.setVisibility(View.VISIBLE);
                if (connectionStatus != null) {
                    connectionStatus.setVisibility(View.VISIBLE);
                    connectionStatus.setText("Connecting...");
                }
                if (connectButton != null) connectButton.setEnabled(false);
                
                // Try to connect in background
                new AsyncTask<Void, Void, Boolean>() {
                    @Override
                    protected Boolean doInBackground(Void... params) {
                        // Convert protocol index to type
                        String protocolType = protocolIndex == 0 ? "SMB" : 
                                            protocolIndex == 1 ? "FTP" : "HTTP";
                        
                        // Try to connect
                        return OnboardingManager.connectToNetwork(
                            protocolType, host, share, username, password
                        );
                    }
                    
                    @Override
                    protected void onPostExecute(Boolean success) {
                        if (connectionProgress != null) connectionProgress.setVisibility(View.GONE);
                        if (connectButton != null) connectButton.setEnabled(true);
                        
                        if (success) {
                            if (connectionStatus != null) {
                                connectionStatus.setText("Connected! Scanning for Diablo II files...");
                            }
                            
                            // Scan for D2 files
                            String networkPath = "\\\\" + host + "\\" + share;
                            setSelectedSource(networkPath);
                            startAssetExtraction();
                        } else {
                            if (connectionStatus != null) {
                                connectionStatus.setText("Connection failed. Please check your settings.");
                            }
                            Toast.makeText(OnboardingActivity.this, 
                                "Failed to connect to network location", Toast.LENGTH_LONG).show();
                        }
                    }
                }.execute();
            });
        }
    }
}