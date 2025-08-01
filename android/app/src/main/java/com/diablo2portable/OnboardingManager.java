package com.diablo2portable;

/**
 * JNI bridge for onboarding system
 * Provides native methods for asset extraction and validation
 */
public class OnboardingManager {
    
    static {
        System.loadLibrary("d2engine");
    }
    
    /**
     * Extract assets from source path to output path
     * @param sourcePath Path to D2 installation or MPQ files
     * @param outputPath Path to extract assets to
     * @return true if extraction successful
     */
    public static native boolean extractAssets(String sourcePath, String outputPath);
    
    /**
     * Get current extraction progress
     * @return Progress from 0.0 to 1.0
     */
    public static native float getProgress();
    
    /**
     * Get current file being extracted
     * @return Filename or empty string
     */
    public static native String getCurrentFile();
    
    /**
     * Validate MPQ files at given path
     * @param path Path containing MPQ files
     * @return true if valid D2 installation found
     */
    public static native boolean validateAssets(String path);
    
    /**
     * Get list of missing files
     * @return Array of missing file names
     */
    public static native String[] getMissingFiles();
    
    /**
     * Detect available USB storage devices
     * @return Array of USB device info strings in format "path|label|totalSpace|freeSpace"
     */
    public static native String[] detectUSBStorage();
    
    /**
     * Connect to network location
     * @param protocol Protocol type (SMB, FTP, HTTP)
     * @param host Host or IP address
     * @param share Share name or path
     * @param username Username (can be empty)
     * @param password Password (can be empty)
     * @return true if connection successful
     */
    public static native boolean connectToNetwork(String protocol, String host, 
                                                  String share, String username, String password);
}