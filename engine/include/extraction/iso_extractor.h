#ifndef D2_ISO_EXTRACTOR_H
#define D2_ISO_EXTRACTOR_H

namespace d2 {

/**
 * ISO file extractor for extracting MPQ files from ISO images
 */
class ISOExtractor {
public:
    ISOExtractor() = default;
    ~ISOExtractor() = default;
    
    /**
     * Check if an ISO file is currently open
     * @return true if ISO is open, false otherwise
     */
    bool isOpen() const { return false; }
};

} // namespace d2

#endif // D2_ISO_EXTRACTOR_H