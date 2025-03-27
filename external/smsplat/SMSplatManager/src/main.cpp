#include "sm_splat_manager.h"
#include <iostream>
#include <string>

void print_usage() {
    std::cout << "Usage: sm_splat_manager [options]\n"
              << "Options:\n"
              << "  --transmitter-name <name>    Set transmitter name\n"
              << "  --transmitter-lat <value>    Set transmitter latitude (degrees)\n"
              << "  --transmitter-lon <value>    Set transmitter longitude (degrees)\n"
              << "  --transmitter-alt <value>    Set transmitter altitude (meters)\n"
              << "  --receiver-height <value>    Set receiver height (meters)\n"
              << "  --start-angle <value>        Set start angle (degrees)\n"
              << "  --end-angle <value>          Set end angle (degrees)\n"
              << "  --radius <value>             Set coverage radius (kilometers)\n"
              << "  --frequency <value>          Set frequency (MHz, default: 1400.0)\n"
              << "  --fresnel-zone <value>       Set Fresnel zone (default: 40.0)\n";
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        print_usage();
        return 1;
    }

    try {
        SMSplatInputInfo user_input_info;
        
        // Set default values
        user_input_info.itm_cov_type = "full";
        user_input_info.start_angle = 0.0;
        user_input_info.end_angle = 360.0;
        user_input_info.radius = 25;
        user_input_info.frequency = 1400.0;
        user_input_info.fresnel_zone = 40.0;

        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            
            // Handle parameters with values
            if (i + 1 >= argc) {
                std::cerr << "Missing value for " << arg << std::endl;
                return 1;
            }

            if (arg == "--transmitter-name") {
                user_input_info.transmitter_name = argv[++i];
            } else if (arg == "--transmitter-lat") {
                user_input_info.transmitter_lat = std::stod(argv[++i]);
            } else if (arg == "--transmitter-lon") {
                user_input_info.transmitter_lon = std::stod(argv[++i]);
            } else if (arg == "--transmitter-alt") {
                user_input_info.transmitter_alt = std::stod(argv[++i]);
            } else if (arg == "--receiver-height") {
                user_input_info.receiver_height = std::stod(argv[++i]);
            } else if (arg == "--start-angle") {
                user_input_info.start_angle = std::stod(argv[++i]);
            } else if (arg == "--end-angle") {
                user_input_info.end_angle = std::stod(argv[++i]);
            } else if (arg == "--radius") {
                user_input_info.radius = std::stod(argv[++i]);
            } else if (arg == "--frequency") {
                user_input_info.frequency = std::stod(argv[++i]);
            } else if (arg == "--fresnel-zone") {
                user_input_info.fresnel_zone = std::stod(argv[++i]);
            } else {
                std::cerr << "Unknown option: " << arg << std::endl;
                print_usage();
                return 1;
            }
        }

        // Validate required parameters
        if (user_input_info.receiver_height == 0) {
            std::cerr << "Missing required parameters" << std::endl;
            print_usage();
            return 1;
        }

        SMSplatManager manager;
        auto generated_info = manager.generate(user_input_info);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 