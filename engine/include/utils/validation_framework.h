#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace d2::utils {

/**
 * @brief Validation result containing success status and error message
 */
struct ValidationResult {
    bool success;
    std::string error_message;
    
    ValidationResult(bool success = true, const std::string& error = "")
        : success(success), error_message(error) {}
    
    operator bool() const { return success; }
};

/**
 * @brief Base validator interface
 * 
 * Provides a standard interface for validation operations.
 */
class Validator {
public:
    virtual ~Validator() = default;
    
    /**
     * @brief Validate the input
     * @param input Input to validate
     * @return Validation result
     */
    virtual ValidationResult validate(const std::string& input) const = 0;
    
    /**
     * @brief Get a description of what this validator checks
     * @return Description string
     */
    virtual std::string getDescription() const = 0;
};

/**
 * @brief File existence validator
 */
class FileExistsValidator : public Validator {
public:
    ValidationResult validate(const std::string& path) const override;
    std::string getDescription() const override;
};

/**
 * @brief Directory existence validator
 */
class DirectoryExistsValidator : public Validator {
public:
    ValidationResult validate(const std::string& path) const override;
    std::string getDescription() const override;
};

/**
 * @brief File extension validator
 */
class FileExtensionValidator : public Validator {
public:
    explicit FileExtensionValidator(const std::vector<std::string>& allowed_extensions);
    
    ValidationResult validate(const std::string& path) const override;
    std::string getDescription() const override;

private:
    std::vector<std::string> allowed_extensions_;
};

/**
 * @brief File size validator
 */
class FileSizeValidator : public Validator {
public:
    explicit FileSizeValidator(size_t min_size, size_t max_size = SIZE_MAX);
    
    ValidationResult validate(const std::string& path) const override;
    std::string getDescription() const override;

private:
    size_t min_size_;
    size_t max_size_;
};

/**
 * @brief File header validator
 */
class FileHeaderValidator : public Validator {
public:
    explicit FileHeaderValidator(const std::vector<uint8_t>& expected_header);
    explicit FileHeaderValidator(const std::string& expected_header);
    
    ValidationResult validate(const std::string& path) const override;
    std::string getDescription() const override;

private:
    std::vector<uint8_t> expected_header_;
};

/**
 * @brief Custom function validator
 */
class FunctionValidator : public Validator {
public:
    using ValidatorFunction = std::function<ValidationResult(const std::string&)>;
    
    explicit FunctionValidator(ValidatorFunction func, const std::string& description);
    
    ValidationResult validate(const std::string& input) const override;
    std::string getDescription() const override;

private:
    ValidatorFunction validator_func_;
    std::string description_;
};

/**
 * @brief Composite validator that runs multiple validators
 */
class CompositeValidator : public Validator {
public:
    /**
     * @brief Add a validator to the composite
     * @param validator Validator to add
     */
    void addValidator(std::unique_ptr<Validator> validator);
    
    /**
     * @brief Validate using all validators
     * @param input Input to validate
     * @return Validation result (fails if any validator fails)
     */
    ValidationResult validate(const std::string& input) const override;
    
    std::string getDescription() const override;

private:
    std::vector<std::unique_ptr<Validator>> validators_;
};

/**
 * @brief Validation builder for easy validator creation
 */
class ValidationBuilder {
public:
    /**
     * @brief Add file existence check
     * @return Reference to builder for chaining
     */
    ValidationBuilder& fileExists();
    
    /**
     * @brief Add directory existence check
     * @return Reference to builder for chaining
     */
    ValidationBuilder& directoryExists();
    
    /**
     * @brief Add file extension check
     * @param extensions Allowed file extensions
     * @return Reference to builder for chaining
     */
    ValidationBuilder& hasExtension(const std::vector<std::string>& extensions);
    
    /**
     * @brief Add file size check
     * @param min_size Minimum file size
     * @param max_size Maximum file size
     * @return Reference to builder for chaining
     */
    ValidationBuilder& hasSize(size_t min_size, size_t max_size = SIZE_MAX);
    
    /**
     * @brief Add file header check
     * @param expected_header Expected header bytes
     * @return Reference to builder for chaining
     */
    ValidationBuilder& hasHeader(const std::vector<uint8_t>& expected_header);
    
    /**
     * @brief Add file header check
     * @param expected_header Expected header string
     * @return Reference to builder for chaining
     */
    ValidationBuilder& hasHeader(const std::string& expected_header);
    
    /**
     * @brief Add custom validation function
     * @param func Custom validation function
     * @param description Description of the validation
     * @return Reference to builder for chaining
     */
    ValidationBuilder& custom(
        std::function<ValidationResult(const std::string&)> func,
        const std::string& description);
    
    /**
     * @brief Build the composite validator
     * @return Unique pointer to the built validator
     */
    std::unique_ptr<Validator> build();

private:
    std::vector<std::unique_ptr<Validator>> validators_;
};

/**
 * @brief Convenience functions for common validation scenarios
 */
namespace validation {

/**
 * @brief Validate MPQ file
 * @param path Path to MPQ file
 * @return Validation result
 */
ValidationResult validateMPQFile(const std::string& path);

/**
 * @brief Validate DC6 sprite file
 * @param path Path to DC6 file
 * @return Validation result
 */
ValidationResult validateDC6File(const std::string& path);

/**
 * @brief Validate ISO file
 * @param path Path to ISO file
 * @return Validation result
 */
ValidationResult validateISOFile(const std::string& path);

/**
 * @brief Validate D2S save file
 * @param path Path to D2S file
 * @return Validation result
 */
ValidationResult validateD2SFile(const std::string& path);

} // namespace validation

} // namespace d2::utils