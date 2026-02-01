#include <iostream>
#include <cassert>
#include <cstdint>
#include "../../../rf_path.hpp"

using rf::Frequency;

enum class ThreatLevel {
    NONE = 0,
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    CRITICAL = 4,
    UNKNOWN = 5
};

enum class DroneType {
    UNKNOWN = 0,
    MAVIC = 1,
    DJI_P34 = 2,
    PHANTOM = 3,
    DJI_MINI = 4,
    PARROT_ANAFI = 5,
    PARROT_BEBOP = 6,
    PX4_DRONE = 7,
    MILITARY_DRONE = 8,
    DIY_DRONE = 9,
    FPV_RACING = 10
};

struct DroneSignal {
    Frequency frequency_hz;
    [[maybe_unused]] int32_t rssi_db;
};

namespace DroneConstants {
    constexpr Frequency MIN_HARDWARE_FREQ = 1'000'000ULL;
    constexpr Frequency MAX_HARDWARE_FREQ = 7'200'000'000ULL;
    constexpr Frequency MIN_24GHZ = 2'400'000'000ULL;
    constexpr Frequency MAX_24GHZ = 2'483'500'000ULL;
    constexpr Frequency MIN_58GHZ = 5'725'000'000ULL;
    constexpr Frequency MAX_58GHZ = 5'875'000'000ULL;
    constexpr Frequency MIN_900MHZ = 860'000'000ULL;
    constexpr Frequency MAX_900MHZ = 930'000'000ULL;
    constexpr Frequency MIN_433MHZ = 433'000'000ULL;
    constexpr Frequency MAX_433MHZ = 435'000'000ULL;
    constexpr int32_t CRITICAL_RSSI_DB = -50;
    constexpr int32_t HIGH_RSSI_DB = -60;
    constexpr int32_t MEDIUM_RSSI_DB = -70;
    constexpr int32_t LOW_RSSI_DB = -80;
}

class SimpleDroneValidation {
public:
    static bool validate_frequency_range(Frequency freq_hz) {
        return freq_hz >= DroneConstants::MIN_HARDWARE_FREQ && 
               freq_hz <= DroneConstants::MAX_HARDWARE_FREQ;
    }

    static bool validate_rssi_signal(int32_t rssi_db, ThreatLevel threat) {
        switch (threat) {
            case ThreatLevel::CRITICAL:
                return rssi_db >= DroneConstants::CRITICAL_RSSI_DB;
            case ThreatLevel::HIGH:
                return rssi_db >= DroneConstants::HIGH_RSSI_DB;
            case ThreatLevel::MEDIUM:
                return rssi_db >= DroneConstants::MEDIUM_RSSI_DB;
            case ThreatLevel::LOW:
                return rssi_db >= DroneConstants::LOW_RSSI_DB;
            case ThreatLevel::NONE:
            default:
                return false;
        }
    }

    static ThreatLevel classify_signal_strength(int32_t rssi_db) {
        if (rssi_db >= DroneConstants::CRITICAL_RSSI_DB) {
            return ThreatLevel::CRITICAL;
        } else if (rssi_db >= DroneConstants::HIGH_RSSI_DB) {
            return ThreatLevel::HIGH;
        } else if (rssi_db >= DroneConstants::MEDIUM_RSSI_DB) {
            return ThreatLevel::MEDIUM;
        } else if (rssi_db >= DroneConstants::LOW_RSSI_DB) {
            return ThreatLevel::LOW;
        } else {
            return ThreatLevel::NONE;
        }
    }

    static DroneType identify_drone_type(const DroneSignal& signal) {
        Frequency freq = signal.frequency_hz;
        
        if (freq >= DroneConstants::MIN_24GHZ && freq <= DroneConstants::MAX_24GHZ) {
            return DroneType::MAVIC;
        } else if (freq >= DroneConstants::MIN_58GHZ && freq <= DroneConstants::MAX_58GHZ) {
            return DroneType::FPV_RACING;
        } else if (freq >= DroneConstants::MIN_900MHZ && freq <= DroneConstants::MAX_900MHZ) {
            return DroneType::MILITARY_DRONE;
        } else if (freq >= DroneConstants::MIN_433MHZ && freq <= DroneConstants::MAX_433MHZ) {
            return DroneType::DIY_DRONE;
        }
        
        return DroneType::UNKNOWN;
    }
};

void test_valid_frequency_ranges() {
    assert(SimpleDroneValidation::validate_frequency_range(2'450'000'000ULL));
    assert(SimpleDroneValidation::validate_frequency_range(5'800'000'000ULL));
    assert(SimpleDroneValidation::validate_frequency_range(433'500'000ULL));
    std::cout << "PASS: Valid frequency ranges" << std::endl;
}

void test_invalid_frequency_ranges() {
    assert(!SimpleDroneValidation::validate_frequency_range(100'000ULL));
    assert(!SimpleDroneValidation::validate_frequency_range(10'000'000'000ULL));
    std::cout << "PASS: Invalid frequency ranges" << std::endl;
}

void test_rssi_signal_validation() {
    assert(SimpleDroneValidation::validate_rssi_signal(-50, ThreatLevel::CRITICAL));
    assert(!SimpleDroneValidation::validate_rssi_signal(-100, ThreatLevel::HIGH));
    assert(SimpleDroneValidation::validate_rssi_signal(-70, ThreatLevel::MEDIUM));
    std::cout << "PASS: RSSI signal validation" << std::endl;
}

void test_signal_strength_classification() {
    assert(SimpleDroneValidation::classify_signal_strength(-40) == ThreatLevel::CRITICAL);
    assert(SimpleDroneValidation::classify_signal_strength(-60) == ThreatLevel::HIGH);
    assert(SimpleDroneValidation::classify_signal_strength(-70) == ThreatLevel::MEDIUM);
    assert(SimpleDroneValidation::classify_signal_strength(-80) == ThreatLevel::LOW);
    assert(SimpleDroneValidation::classify_signal_strength(-95) == ThreatLevel::NONE);
    std::cout << "PASS: Signal strength classification" << std::endl;
}

void test_drone_type_identification() {
    DroneSignal mavic_signal{2'450'000'000ULL, -60};
    assert(SimpleDroneValidation::identify_drone_type(mavic_signal) == DroneType::MAVIC);
    
    DroneSignal fpv_signal{5'800'000'000ULL, -50};
    assert(SimpleDroneValidation::identify_drone_type(fpv_signal) == DroneType::FPV_RACING);
    
    DroneSignal diy_signal{433'500'000ULL, -70};
    assert(SimpleDroneValidation::identify_drone_type(diy_signal) == DroneType::DIY_DRONE);
    
    DroneSignal unknown_signal{1'000'000'000ULL, -60};
    assert(SimpleDroneValidation::identify_drone_type(unknown_signal) == DroneType::UNKNOWN);
    
    std::cout << "PASS: Drone type identification" << std::endl;
}

int main() {
    std::cout << "=== Running SimpleDroneValidation Tests ===" << std::endl;
    
    test_valid_frequency_ranges();
    test_invalid_frequency_ranges();
    test_rssi_signal_validation();
    test_signal_strength_classification();
    test_drone_type_identification();
    
    std::cout << "=== All tests passed! ===" << std::endl;
    return 0;
}
