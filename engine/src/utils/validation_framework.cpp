#include "utils/validation_framework.h"
#include "utils/file_utils.h"
#include <filesystem>
#include <algorithm>

namespace d2::utils {

// FileExistsValidator
ValidationResult FileExistsValidator::validate(const std::string& path) const {
    if (FileUtils::validateFileExists(path)) {
        return ValidationResult(true);
    }
    return ValidationResult(false, "File does not exist: " + path);
}

std::string FileExistsValidator::getDescription() const {
    return "File must exist";
}

// DirectoryExistsValidator
ValidationResult DirectoryExistsValidator::validate(const std::string& path) const {
    if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
        return ValidationResult(true);
    }
    return ValidationResult(false, "Directory does not exist: " + path);
}

std::string DirectoryExistsValidator::getDescription() const {
    return "Directory must exist";
}

// FileExtensionValidator
FileExtensionValidator::FileExtensionValidator(const std::vector<std::string>& allowed_extensions)
    : allowed_extensions_(allowed_extensions) {}

ValidationResult FileExtensionValidator::validate(const std::string& path) const {
    std::filesystem::path file_path(path);
    std::string extension = file_path.extension().string();
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    for (const auto& allowed : allowed_extensions_) {
        std::string allowed_lower = allowed;
        std::transform(allowed_lower.begin(), allowed_lower.end(), allowed_lower.begin(), ::tolower);
        
        if (extension == allowed_lower) {
            return ValidationResult(true);
        }
    }
    
    return ValidationResult(false, "Invalid file extension: " + extension);
}

std::string FileExtensionValidator::getDescription() const {
    std::string desc = "File must have extension: ";
    for (size_t i = 0; i < allowed_extensions_.size(); ++i) {
        if (i > 0) desc += ", ";
        desc += allowed_extensions_[i];
    }
    return desc;
}

// FileSizeValidator
FileSizeValidator::FileSizeValidator(size_t min_size, size_t max_size)
    : min_size_(min_size), max_size_(max_size) {}

ValidationResult FileSizeValidator::validate(const std::string& path) const {
    size_t file_size = FileUtils::getFileSize(path);
    
    if (file_size == 0 && !FileUtils::validateFileExists(path)) {
        return ValidationResult(false, "File does not exist: " + path);
    }
    
    if (file_size < min_size_) {
        return ValidationResult(false, "File too small: " + std::to_string(file_size) + " bytes (minimum: " + std::to_string(min_size_) + ")");
    }
    
    if (file_size > max_size_) {
        return ValidationResult(false, "File too large: " + std::to_string(file_size) + " bytes (maximum: " + std::to_string(max_size_) + ")");
    }
    
    return ValidationResult(true);
}

std::string FileSizeValidator::getDescription() const {
    return "File size must be between " + std::to_string(min_size_) + " and " + std::to_string(max_size_) + " bytes";
}

// FileHeaderValidator
FileHeaderValidator::FileHeaderValidator(const std::vector<uint8_t>& expected_header)
    : expected_header_(expected_header) {}

FileHeaderValidator::FileHeaderValidator(const std::string& expected_header)
    : expected_header_(expected_header.begin(), expected_header.end()) {}

ValidationResult FileHeaderValidator::validate(const std::string& path) const {
    if (FileUtils::validateFileHeader(path, expected_header_)) {
        return ValidationResult(true);
    }
    return ValidationResult(false, "Invalid file header: " + path);
}

std::string FileHeaderValidator::getDescription() const {
    return "File must have correct header";
}

// FunctionValidator
FunctionValidator::FunctionValidator(ValidatorFunction func, const std::string& description)
    : validator_func_(func), description_(description) {}

ValidationResult FunctionValidator::validate(const std::string& input) const {
    return validator_func_(input);
}

std::string FunctionValidator::getDescription() const {
    return description_;
}

// CompositeValidator
void CompositeValidator::addValidator(std::unique_ptr<Validator> validator) {
    validators_.push_back(std::move(validator));
}

ValidationResult CompositeValidator::validate(const std::string& input) const {
    for (const auto& validator : validators_) {
        ValidationResult result = validator->validate(input);
        if (!result.success) {
            return result;
        }
    }
    return ValidationResult(true);
}

std::string CompositeValidator::getDescription() const {
    std::string desc = "All conditions must be met: ";
    for (size_t i = 0; i < validators_.size(); ++i) {
        if (i > 0) desc += ", ";
        desc += validators_[i]->getDescription();
    }
    return desc;
}

// ValidationBuilder
ValidationBuilder& ValidationBuilder::fileExists() {
    validators_.push_back(std::make_unique<FileExistsValidator>());
    return *this;
}

ValidationBuilder& ValidationBuilder::directoryExists() {
    validators_.push_back(std::make_unique<DirectoryExistsValidator>());
    return *this;
}

ValidationBuilder& ValidationBuilder::hasExtension(const std::vector<std::string>& extensions) {
    validators_.push_back(std::make_unique<FileExtensionValidator>(extensions));
    return *this;
}

ValidationBuilder& ValidationBuilder::hasSize(size_t min_size, size_t max_size) {
    validators_.push_back(std::make_unique<FileSizeValidator>(min_size, max_size));
    return *this;
}

ValidationBuilder& ValidationBuilder::hasHeader(const std::vector<uint8_t>& expected_header) {
    validators_.push_back(std::make_unique<FileHeaderValidator>(expected_header));
    return *this;
}

ValidationBuilder& ValidationBuilder::hasHeader(const std::string& expected_header) {
    validators_.push_back(std::make_unique<FileHeaderValidator>(expected_header));
    return *this;
}

ValidationBuilder& ValidationBuilder::custom(
    std::function<ValidationResult(const std::string&)> func,
    const std::string& description) {
    validators_.push_back(std::make_unique<FunctionValidator>(func, description));
    return *this;
}

std::unique_ptr<Validator> ValidationBuilder::build() {
    auto composite = std::make_unique<CompositeValidator>();
    for (auto& validator : validators_) {
        composite->addValidator(std::move(validator));
    }
    validators_.clear();
    return composite;
}

// Convenience validation functions
namespace validation {

ValidationResult validateMPQFile(const std::string& path) {
    auto validator = ValidationBuilder()
        .fileExists()
        .hasExtension({".mpq", ".MPQ"})
        .hasSize(100)  // Minimum size for a valid MPQ
        .hasHeader("MPQ\x1A")
        .build();
    
    return validator->validate(path);
}

ValidationResult validateDC6File(const std::string& path) {
    auto validator = ValidationBuilder()
        .fileExists()
        .hasExtension({".dc6", ".DC6"})
        .hasSize(32)  // Minimum size for a valid DC6
        .build();
    
    return validator->validate(path);
}

ValidationResult validateISOFile(const std::string& path) {
    auto validator = ValidationBuilder()
        .fileExists()
        .hasExtension({".iso", ".ISO"})
        .hasSize(1024)  // Minimum size for a valid ISO
        .build();
    
    return validator->validate(path);
}

ValidationResult validateD2SFile(const std::string& path) {
    auto validator = ValidationBuilder()
        .fileExists()
        .hasExtension({".d2s", ".D2S", ".sav", ".SAV"})
        .hasSize(96)  // Minimum size for a valid D2S save
        .hasHeader(std::vector<uint8_t>{0x55, 0xAA, 0x55, 0xAA})  // D2S signature
        .build();
    
    return validator->validate(path);
}

} // namespace validation

} // namespace d2::utils