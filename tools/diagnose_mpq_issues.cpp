#include <iostream>
#include <filesystem>
#include <iomanip>
#include <vector>
#include <map>
#include "../engine/include/utils/stormlib_mpq_loader.h"

using namespace d2;
namespace fs = std::filesystem;

struct MPQExpectation {
    std::string filename;
    size_t min_expected_size;
    size_t max_expected_size;
    std::string description;
    bool essential;
};

// Known MPQ file expectations based on research
std::vector<MPQExpectation> mpq_expectations = {
    {"d2data.mpq", 250000000, 300000000, "Main game data (sprites, maps, text files)", true},
    {"d2char.mpq", 250000000, 270000000, "Character graphics and animations", true},
    {"d2exp.mpq", 240000000, 260000000, "Lord of Destruction expansion data", true},
    {"d2music.mpq", 320000000, 350000000, "Original Diablo II music files", false},
    {"d2xmusic.mpq", 50000000, 80000000, "Expansion music files", false},
    {"d2speech.mpq", 150000000, 180000000, "Voice acting and speech files", false},
    {"d2xtalk.mpq", 50000000, 70000000, "Expansion speech files", false},
    {"d2sfx.mpq", 40000000, 60000000, "Sound effects", false},
    {"d2video.mpq", 400000000, 500000000, "Cinematics and videos", false},
    {"d2xvideo.mpq", 100000000, 120000000, "Expansion videos", false},
    {"playd2.mpq", 500000, 1000000, "Play CD files", false},
    {"setup.mpq", 3000000, 5000000, "Installation files", false}
};

bool validateMPQFile(const std::string& mpq_path, const MPQExpectation& expectation) {
    if (!fs::exists(mpq_path)) {
        std::cout << "❌ " << expectation.filename << ": FILE NOT FOUND\n";
        return false;
    }
    
    auto file_size = fs::file_size(mpq_path);
    std::cout << "📁 " << expectation.filename << ": " 
              << (file_size / 1024 / 1024) << " MB";
    
    if (file_size < expectation.min_expected_size) {
        std::cout << " ❌ TOO SMALL (expected " 
                  << (expectation.min_expected_size / 1024 / 1024) << "-"
                  << (expectation.max_expected_size / 1024 / 1024) << " MB)\n";
        std::cout << "   Issue: " << expectation.description << "\n";
        std::cout << "   Status: " << (expectation.essential ? "CRITICAL" : "Optional") << "\n";
        return false;
    } else if (file_size > expectation.max_expected_size) {
        std::cout << " ⚠️  LARGER THAN EXPECTED\n";
    } else {
        std::cout << " ✅ OK\n";
    }
    
    // Try to open with StormLib
    StormLibMPQLoader loader;
    if (!loader.open(mpq_path)) {
        std::cout << "   ❌ Cannot open with StormLib: " << loader.getLastError() << "\n";
        return false;
    } else {
        auto files = loader.listFiles();
        std::cout << "   ✅ Contains " << files.size() << " files\n";
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <mpq_directory>\n";
        return 1;
    }
    
    std::string mpq_dir = argv[1];
    if (!fs::exists(mpq_dir) || !fs::is_directory(mpq_dir)) {
        std::cerr << "Invalid directory: " << mpq_dir << "\n";
        return 1;
    }
    
    std::cout << "=== Diablo II MPQ Files Diagnostic ===\n";
    std::cout << "Directory: " << mpq_dir << "\n\n";
    
    int total_files = 0;
    int valid_files = 0;
    int critical_issues = 0;
    
    for (const auto& expectation : mpq_expectations) {
        std::string mpq_path = (fs::path(mpq_dir) / expectation.filename).string();
        total_files++;
        
        if (validateMPQFile(mpq_path, expectation)) {
            valid_files++;
        } else if (expectation.essential) {
            critical_issues++;
        }
        std::cout << "\n";
    }
    
    std::cout << "=== SUMMARY ===\n";
    std::cout << "Total MPQ files checked: " << total_files << "\n";
    std::cout << "Valid files: " << valid_files << "/" << total_files << "\n";
    std::cout << "Critical issues: " << critical_issues << "\n\n";
    
    if (critical_issues > 0) {
        std::cout << "🚨 CRITICAL ISSUES DETECTED 🚨\n";
        std::cout << "Your Diablo II installation appears to be corrupted or incomplete.\n\n";
        
        std::cout << "SOLUTIONS:\n";
        std::cout << "1. 🔄 REINSTALL from original CDs or Battle.net client\n";
        std::cout << "2. 🛡️  DISABLE antivirus during installation\n";
        std::cout << "3. 👑 RUN installer as Administrator\n";
        std::cout << "4. 📀 COPY MPQ files manually from original CDs\n";
        std::cout << "5. 💾 ENSURE sufficient disk space (1.5GB+ required)\n\n";
        
        std::cout << "EXPLANATION:\n";
        std::cout << "Files under 100MB (especially d2char.mpq and d2music.mpq) are\n";
        std::cout << "corrupted placeholder files from failed installation. These should\n";
        std::cout << "be hundreds of megabytes containing game assets.\n";
        
        return 1;
    } else {
        std::cout << "✅ All essential MPQ files appear healthy!\n";
        std::cout << "Your Diablo II installation should work correctly.\n";
        return 0;
    }
}