#include <cstdint>
#include <cstdio>

// Minimal implementation for testing
#define FLASH_STORAGE
namespace ui { namespace apps { namespace enhanced_drone_analyzer { namespace DiamondCore {

struct FrequencyParser {
    static constexpr uint64_t MHZ_TO_HZ = 1000000ULL;
    static constexpr uint64_t MAX_MHZ = 7200ULL;
    
    static constexpr uint64_t MULTIPLIERS[7] FLASH_STORAGE = {
        1000000ULL,
        100000ULL,
        10000ULL,
        1000ULL,
        100ULL,
        10ULL,
        1ULL
    };
    
    static inline uint64_t parse_mhz_string(const char* str) noexcept {
        if (!str || *str == '\0') return 0;
        
        while (*str == ' ' || *str == '\t') str++;
        
        uint64_t mhz = 0;
        while (*str >= '0' && *str <= '9') {
            uint8_t digit = static_cast<uint8_t>(*str - '0');
            if (mhz > (UINT64_MAX - digit) / 10) return 0;
            mhz = mhz * 10 + digit;
            str++;
        }
        
        if (mhz > MAX_MHZ) return 0;
        
        uint64_t hz_fraction = 0;
        if (*str == '.') {
            str++;
            
            uint8_t digits = 0;
            
            for (int i = 0; i < 6 && *str >= '0' && *str <= '9'; i++) {
                uint8_t digit = static_cast<uint8_t>(*str - '0');
                hz_fraction = hz_fraction * 10 + digit;
                digits++;
                str++;
            }
            
            hz_fraction *= MULTIPLIERS[digits];
        }
        
        uint64_t result = mhz * MHZ_TO_HZ;
        
        if (result > UINT64_MAX - hz_fraction) return 0;
        result += hz_fraction;
        
        return result;
    }
    
    static inline bool is_valid_frequency(uint64_t freq_hz) noexcept {
        return freq_hz >= 1000000ULL && freq_hz <= 7200000000ULL;
    }
};

}}}}

using namespace ui::apps::enhanced_drone_analyzer::DiamondCore;

void test_frequency_parser() {
    printf("=== Frequency Parser Verification Tests ===\n\n");
    
    bool all_passed = true;
    
    // Test 1: Critical bug fix - "2400.5" should be 2400500000 Hz, not 2400050000 Hz
    uint64_t result1 = FrequencyParser::parse_mhz_string("2400.5");
    uint64_t expected1 = 2400500000ULL;
    bool test1 = (result1 == expected1);
    printf("Test 1 (CRITICAL): \"2400.5\" MHz\n");
    printf("  Expected: %llu Hz (2.4 GHz + 500 kHz)\n", expected1);
    printf("  Got:      %llu Hz\n", result1);
    printf("  Status:   %s\n\n", test1 ? "PASS" : "FAIL");
    if (!test1) {
        printf("  ERROR: Lost precision! The old bug would give %llu Hz\n\n", 2400050000ULL);
        all_passed = false;
    }
    
    // Test 2: Full precision - "2400.500000" should be exactly 2400500000 Hz
    uint64_t result2 = FrequencyParser::parse_mhz_string("2400.500000");
    uint64_t expected2 = 2400500000ULL;
    bool test2 = (result2 == expected2);
    printf("Test 2: \"2400.500000\" MHz (full 6 decimal digits)\n");
    printf("  Expected: %llu Hz\n", expected2);
    printf("  Got:      %llu Hz\n", result2);
    printf("  Status:   %s\n\n", test2 ? "PASS" : "FAIL");
    all_passed = all_passed && test2;
    
    // Test 3: Single decimal digit - "1000.1" should be 1000100000 Hz
    uint64_t result3 = FrequencyParser::parse_mhz_string("1000.1");
    uint64_t expected3 = 1000100000ULL;
    bool test3 = (result3 == expected3);
    printf("Test 3: \"1000.1\" MHz (1 decimal digit = 100 kHz)\n");
    printf("  Expected: %llu Hz\n", expected3);
    printf("  Got:      %llu Hz\n", result3);
    printf("  Status:   %s\n\n", test3 ? "PASS" : "FAIL");
    all_passed = all_passed && test3;
    
    // Test 4: Two decimal digits - "1000.01" should be 1000010000 Hz
    uint64_t result4 = FrequencyParser::parse_mhz_string("1000.01");
    uint64_t expected4 = 1000010000ULL;
    bool test4 = (result4 == expected4);
    printf("Test 4: \"1000.01\" MHz (2 decimal digits = 10 kHz)\n");
    printf("  Expected: %llu Hz\n", expected4);
    printf("  Got:      %llu Hz\n", result4);
    printf("  Status:   %s\n\n", test4 ? "PASS" : "FAIL");
    all_passed = all_passed && test4;
    
    // Test 5: Three decimal digits (kHz precision) - "1000.001" should be 1000001000 Hz
    uint64_t result5 = FrequencyParser::parse_mhz_string("1000.001");
    uint64_t expected5 = 1000001000ULL;
    bool test5 = (result5 == expected5);
    printf("Test 5: \"1000.001\" MHz (3 decimal digits = 1 kHz)\n");
    printf("  Expected: %llu Hz\n", expected5);
    printf("  Got:      %llu Hz\n", result5);
    printf("  Status:   %s\n\n", test5 ? "PASS" : "FAIL");
    all_passed = all_passed && test5;
    
    // Test 6: No decimal point - "2400500000" should be 2400500000 Hz
    uint64_t result6 = FrequencyParser::parse_mhz_string("2400500000");
    uint64_t expected6 = 2400500000ULL;
    bool test6 = (result6 == expected6);
    printf("Test 6: \"2400500000\" (Hz string, no decimal)\n");
    printf("  Expected: %llu Hz\n", expected6);
    printf("  Got:      %llu Hz\n", result6);
    printf("  Status:   %s\n\n", test6 ? "PASS" : "FAIL");
    all_passed = all_passed && test6;
    
    // Test 7: MAX_MHZ boundary - "7200" should be valid
    uint64_t result7 = FrequencyParser::parse_mhz_string("7200");
    uint64_t expected7 = 7200000000ULL;
    bool test7 = (result7 == expected7);
    printf("Test 7: \"7200\" MHz (MAX_MHZ boundary)\n");
    printf("  Expected: %llu Hz\n", expected7);
    printf("  Got:      %llu Hz\n", result7);
    printf("  Status:   %s\n\n", test7 ? "PASS" : "FAIL");
    all_passed = all_passed && test7;
    
    // Test 8: Beyond MAX_MHZ - "7201" should return 0 (error)
    uint64_t result8 = FrequencyParser::parse_mhz_string("7201");
    bool test8 = (result8 == 0);
    printf("Test 8: \"7201\" MHz (beyond MAX_MHZ)\n");
    printf("  Expected: 0 (error)\n");
    printf("  Got:      %llu Hz\n", result8);
    printf("  Status:   %s\n\n", test8 ? "PASS" : "FAIL");
    all_passed = all_passed && test8;
    
    // Test 9: Empty string should return 0
    uint64_t result9 = FrequencyParser::parse_mhz_string("");
    bool test9 = (result9 == 0);
    printf("Test 9: \"\" (empty string)\n");
    printf("  Expected: 0 (error)\n");
    printf("  Got:      %llu Hz\n", result9);
    printf("  Status:   %s\n\n", test9 ? "PASS" : "FAIL");
    all_passed = all_passed && test9;
    
    // Test 10: Validation - is_valid_frequency
    bool test10a = FrequencyParser::is_valid_frequency(2400500000ULL);
    bool test10b = !FrequencyParser::is_valid_frequency(8000000000ULL);
    bool test10c = !FrequencyParser::is_valid_frequency(999999ULL);
    bool test10 = test10a && test10b && test10c;
    printf("Test 10: is_valid_frequency()\n");
    printf("  2.4005 GHz valid:   %s\n", test10a ? "PASS" : "FAIL");
    printf("  8.0 GHz invalid:    %s\n", test10b ? "PASS" : "FAIL");
    printf("  999.999 kHz invalid: %s\n", test10c ? "PASS" : "FAIL");
    printf("  Status:             %s\n\n", test10 ? "PASS" : "FAIL");
    all_passed = all_passed && test10;
    
    printf("=== Final Result: %s ===\n", all_passed ? "ALL TESTS PASSED" : "SOME TESTS FAILED");
}

int main() {
    test_frequency_parser();
    return 0;
}
