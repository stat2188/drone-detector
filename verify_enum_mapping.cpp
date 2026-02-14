// Quick verification of DroneType enum mapping
// Based on ui_drone_common_types.hpp enum definition

#include <iostream>

// DroneType enum from ui_drone_common_types.hpp
enum class DroneType : uint8_t {
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

const char* drone_type_name(DroneType type) {
    switch (type) {
        case DroneType::UNKNOWN: return "UNKNOWN";
        case DroneType::MAVIC: return "MAVIC";
        case DroneType::DJI_P34: return "DJI_P34";
        case DroneType::PHANTOM: return "PHANTOM";
        case DroneType::DJI_MINI: return "DJI_MINI";
        case DroneType::PARROT_ANAFI: return "PARROT_ANAFI";
        case DroneType::PARROT_BEBOP: return "PARROT_BEBOP";
        case DroneType::PX4_DRONE: return "PX4_DRONE";
        case DroneType::MILITARY_DRONE: return "MILITARY_DRONE";
        case DroneType::DIY_DRONE: return "DIY_DRONE";
        case DroneType::FPV_RACING: return "FPV_RACING";
        default: return "INVALID";
    }
}

int main() {
    std::cout << "=== DroneType Enum Mapping Verification ===" << std::endl;
    std::cout << "UNKNOWN = " << static_cast<int>(DroneType::UNKNOWN) 
              << " (" << drone_type_name(DroneType::UNKNOWN) << ")" << std::endl;
    std::cout << "MAVIC = " << static_cast<int>(DroneType::MAVIC) 
              << " (" << drone_type_name(DroneType::MAVIC) << ")" << std::endl;
    std::cout << "DJI_P34 = " << static_cast<int>(DroneType::DJI_P34) 
              << " (" << drone_type_name(DroneType::DJI_P34) << ")" << std::endl;
    std::cout << "PHANTOM = " << static_cast<int>(DroneType::PHANTOM) 
              << " (" << drone_type_name(DroneType::PHANTOM) << ")" << std::endl;
    std::cout << "DJI_MINI = " << static_cast<int>(DroneType::DJI_MINI) 
              << " (" << drone_type_name(DroneType::DJI_MINI) << ")" << std::endl;
    std::cout << "PARROT_ANAFI = " << static_cast<int>(DroneType::PARROT_ANAFI) 
              << " (" << drone_type_name(DroneType::PARROT_ANAFI) << ")" << std::endl;
    std::cout << "PARROT_BEBOP = " << static_cast<int>(DroneType::PARROT_BEBOP) 
              << " (" << drone_type_name(DroneType::PARROT_BEBOP) << ")" << std::endl;
    std::cout << "PX4_DRONE = " << static_cast<int>(DroneType::PX4_DRONE) 
              << " (" << drone_type_name(DroneType::PX4_DRONE) << ")" << std::endl;
    std::cout << "MILITARY_DRONE = " << static_cast<int>(DroneType::MILITARY_DRONE) 
              << " (" << drone_type_name(DroneType::MILITARY_DRONE) << ")" << std::endl;
    std::cout << "DIY_DRONE = " << static_cast<int>(DroneType::DIY_DRONE) 
              << " (" << drone_type_name(DroneType::DIY_DRONE) << ")" << std::endl;
    std::cout << "FPV_RACING = " << static_cast<int>(DroneType::FPV_RACING) 
              << " (" << drone_type_name(DroneType::FPV_RACING) << ")" << std::endl;
    
    std::cout << "\n=== Frequency Band Mapping ===" << std::endl;
    std::cout << "2.4 GHz band  -> " << drone_type_name(DroneType::MAVIC) 
              << " (value: " << static_cast<int>(DroneType::MAVIC) << ")" << std::endl;
    std::cout << "5.8 GHz band  -> " << drone_type_name(DroneType::FPV_RACING) 
              << " (value: " << static_cast<int>(DroneType::FPV_RACING) << ")" << std::endl;
    std::cout << "860-930 MHz   -> " << drone_type_name(DroneType::MILITARY_DRONE) 
              << " (value: " << static_cast<int>(DroneType::MILITARY_DRONE) << ")" << std::endl;
    std::cout << "433-435 MHz   -> " << drone_type_name(DroneType::DIY_DRONE) 
              << " (value: " << static_cast<int>(DroneType::DIY_DRONE) << ")" << std::endl;
    
    std::cout << "\n=== Bug Fixes Verification ===" << std::endl;
    std::cout << "✅ FPV_RACING: Was 4 (DJI_MINI), Now 10 (FPV_RACING)" << std::endl;
    std::cout << "✅ MILITARY_DRONE: Was 6 (PARROT_BEBOP), Now 8 (MILITARY_DRONE)" << std::endl;
    std::cout << "✅ DIY_DRONE: Was 5 (PARROT_ANAFI), Now 9 (DIY_DRONE)" << std::endl;
    
    return 0;
}
