package com.diablo2portable;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.ArrayAdapter;
import android.widget.AdapterView;
import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class FileBrowserActivity extends Activity {
    private TextView currentPath;
    private ListView fileList;
    private Button upButton;
    private Button selectButton;
    private ArrayAdapter<String> adapter;
    private List<String> fileNames;
    private String filterExtension;
    private File currentDirectory;
    private File selectedFile;
    
    // Test mode support
    private boolean testMode = false;
    private List<String> testFiles = new ArrayList<>();
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_browser);
        
        currentPath = findViewById(R.id.current_path);
        fileList = findViewById(R.id.file_list);
        upButton = findViewById(R.id.up_button);
        selectButton = findViewById(R.id.select_button);
        
        filterExtension = getIntent().getStringExtra("filter_extension");
        if (filterExtension == null) filterExtension = "";
        
        fileNames = new ArrayList<>();
        adapter = new ArrayAdapter<>(this, R.layout.file_list_item, R.id.file_name, fileNames);
        fileList.setAdapter(adapter);
        
        // Start in external storage
        currentDirectory = new File("/storage/emulated/0");
        if (!currentDirectory.exists()) {
            currentDirectory = getExternalFilesDir(null);
        }
        
        refreshFileList();
        
        fileList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                String fileName = fileNames.get(position);
                File file = new File(currentDirectory, fileName);
                
                if (file.isDirectory()) {
                    currentDirectory = file;
                    refreshFileList();
                } else {
                    selectedFile = file;
                    if (selectButton != null) {
                        selectButton.setVisibility(View.VISIBLE);
                    }
                }
            }
        });
        
        upButton.setOnClickListener(v -> {
            File parent = currentDirectory.getParentFile();
            if (parent != null && parent.canRead()) {
                currentDirectory = parent;
                refreshFileList();
            }
        });
        
        if (selectButton != null) {
            selectButton.setVisibility(View.GONE);
            selectButton.setOnClickListener(v -> {
                if (selectedFile != null) {
                    Intent result = new Intent();
                    result.putExtra("selected_file", selectedFile.getAbsolutePath());
                    setResult(RESULT_OK, result);
                    finish();
                }
            });
        }
    }
    
    public void refreshFileList() {
        fileNames.clear();
        
        if (testMode) {
            // Use test files
            for (String testFile : testFiles) {
                File f = new File(testFile);
                fileNames.add(f.getName());
            }
        } else {
            // Use real files
            if (currentDirectory != null && currentDirectory.exists()) {
                File[] files = currentDirectory.listFiles();
                if (files != null) {
                    for (File file : files) {
                        if (file.isDirectory() || 
                            (filterExtension.isEmpty() || file.getName().endsWith(filterExtension))) {
                            fileNames.add(file.getName());
                        }
                    }
                }
            }
        }
        
        adapter.notifyDataSetChanged();
        
        if (currentPath != null) {
            currentPath.setText(currentDirectory != null ? currentDirectory.getPath() : "/");
        }
    }
    
    // Test support methods
    public void setTestMode(boolean testMode) {
        this.testMode = testMode;
    }
    
    public void addTestFile(String path) {
        testFiles.add(path);
    }
}