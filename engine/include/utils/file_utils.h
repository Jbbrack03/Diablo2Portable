#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <cstdint>

namespace d2::utils {

/**
 * @brief Utility class for common file operations
 * 
 * This class provides centralized file I/O operations to reduce code duplication
 * across the codebase and provide consistent error handling.
 */
class FileUtils {
public:
    /**
     * @brief Safely opens a binary file for reading
     * @param path Path to the file
     * @param file Reference to the ifstream object
     * @return true if file was opened successfully, false otherwise
     */
    static bool safeOpenBinaryFileForReading(const std::string& path, std::ifstream& file);
    
    /**
     * @brief Safely opens a binary file for writing
     * @param path Path to the file
     * @param file Reference to the ofstream object
     * @return true if file was opened successfully, false otherwise
     */
    static bool safeOpenBinaryFileForWriting(const std::string& path, std::ofstream& file);
    
    /**
     * @brief Safely opens a text file for reading
     * @param path Path to the file
     * @param file Reference to the ifstream object
     * @return true if file was opened successfully, false otherwise
     */
    static bool safeOpenTextFileForReading(const std::string& path, std::ifstream& file);
    
    /**
     * @brief Safely opens a text file for writing
     * @param path Path to the file
     * @param file Reference to the ofstream object
     * @return true if file was opened successfully, false otherwise
     */
    static bool safeOpenTextFileForWriting(const std::string& path, std::ofstream& file);
    
    /**
     * @brief Validates that a file exists and is readable
     * @param path Path to the file
     * @return true if file exists and is readable, false otherwise
     */
    static bool validateFileExists(const std::string& path);
    
    /**
     * @brief Validates that a file has the expected header bytes
     * @param path Path to the file
     * @param expectedHeader Expected header bytes
     * @return true if header matches, false otherwise
     */
    static bool validateFileHeader(const std::string& path, const std::vector<uint8_t>& expectedHeader);
    
    /**
     * @brief Validates that a file has the expected header bytes (string version)
     * @param path Path to the file
     * @param expectedHeader Expected header as string
     * @return true if header matches, false otherwise
     */
    static bool validateFileHeader(const std::string& path, const std::string& expectedHeader);
    
    /**
     * @brief Reads the entire file into a vector of bytes
     * @param path Path to the file
     * @param data Vector to store the file data
     * @return true if file was read successfully, false otherwise
     */
    static bool readEntireFile(const std::string& path, std::vector<uint8_t>& data);
    
    /**
     * @brief Reads the entire file into a string
     * @param path Path to the file
     * @param data String to store the file data
     * @return true if file was read successfully, false otherwise
     */
    static bool readEntireFile(const std::string& path, std::string& data);
    
    /**
     * @brief Writes data to a file
     * @param path Path to the file
     * @param data Data to write
     * @return true if file was written successfully, false otherwise
     */
    static bool writeEntireFile(const std::string& path, const std::vector<uint8_t>& data);
    
    /**
     * @brief Writes data to a file
     * @param path Path to the file
     * @param data Data to write
     * @return true if file was written successfully, false otherwise
     */
    static bool writeEntireFile(const std::string& path, const std::string& data);
    
    /**
     * @brief Gets the size of a file
     * @param path Path to the file
     * @return Size of the file in bytes, or 0 if file doesn't exist
     */
    static size_t getFileSize(const std::string& path);
    
    /**
     * @brief Checks if a file is empty
     * @param path Path to the file
     * @return true if file is empty or doesn't exist, false otherwise
     */
    static bool isFileEmpty(const std::string& path);
    
    /**
     * @brief Creates all parent directories for a file path
     * @param path Path to the file
     * @return true if directories were created successfully, false otherwise
     */
    static bool createParentDirectories(const std::string& path);
    
    /**
     * @brief Safely closes a file stream and handles any errors
     * @param file File stream to close
     * @param path Path to the file (for error reporting)
     * @return true if file was closed successfully, false otherwise
     */
    static bool safeCloseFile(std::ifstream& file, const std::string& path = "");
    
    /**
     * @brief Safely closes a file stream and handles any errors
     * @param file File stream to close
     * @param path Path to the file (for error reporting)
     * @return true if file was closed successfully, false otherwise
     */
    static bool safeCloseFile(std::ofstream& file, const std::string& path = "");
    
    /**
     * @brief Gets the last error message
     * @return Last error message
     */
    static std::string getLastError();

private:
    static thread_local std::string lastError_;
    
    /**
     * @brief Sets the last error message
     * @param error Error message to set
     */
    static void setLastError(const std::string& error);
};

} // namespace d2::utils