#include "sm_splat_info.h"
#include <iostream>
#include <iomanip>

std::ostream& operator<<(std::ostream& os, const SMSplatInputInfo& info) {
    os << "SMSplatInputInfo {\n";
    os << "  transmitter_name: " << (info.transmitter_name ? info.transmitter_name : "nullptr")
       << "\n";
    os << "  transmitter_lat: " << info.transmitter_lat << "\n";
    os << "  transmitter_lon: " << info.transmitter_lon << "\n";
    os << "  transmitter_alt: " << info.transmitter_alt << "\n";
    os << "  itm_cov_type: " << (info.itm_cov_type ? info.itm_cov_type : "nullptr") << "\n";
    os << "  receiver_height: " << info.receiver_height << "\n";
    os << "  start_angle: " << info.start_angle << "\n";
    os << "  end_angle: " << info.end_angle << "\n";
    os << "  radius: " << info.radius << "\n";
    os << "  frequency: " << info.frequency << "\n";
    os << "  fresnel_zone: " << info.fresnel_zone << "\n";
    os << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const SMSplatGenInfo& info) {
    os << "SMSplatGenInfo:\n";
    os << "  coverage_name: " << (info.coverage_name ? "nullptr" : info.coverage_name) << "\n";
    //os << "  image_file: " << (info.image_file ? "nullptr" : info.image_file) << "\n";
    os << "  image_size: " << info.image_size.width << "x" << info.image_size.height << "\n";
    os << "  coordinates: N:" << info.coordinates.north
       << " S:" << info.coordinates.south
       << " E:" << info.coordinates.east
       << " W:" << info.coordinates.west << "\n";
    os << "  zoom_level: " << info.zoom_level << "\n";
    os << "  transmitter_lon: " << info.transmitter_lon << "\n";
    os << "  transmitter_lat: " << info.transmitter_lat << "\n";
    os << "  coverage_radius: " << info.coverage_radius << "\n";
    os << "  Image file: " << info.image_file << "\n";
    os << "  Image size: " << info.image_size.width << "x" << info.image_size.height << "\n";
    os << "  Coordinates: "
       << "N:" << info.coordinates.north
       << " S:" << info.coordinates.south
       << " E:" << info.coordinates.east
       << " W:" << info.coordinates.west << "\n";
    os << "  Warped Image bounds: \n"
       << "    Width: " << info.image_size.width << "\n"
       << "    Height: " << info.image_size.height << "\n"
       << "    Min Y: " << info.coordinates.south << "\n"
       << "    Min X: " << info.coordinates.west << "\n"
       << "    Max X: " << info.coordinates.east << "\n"
       << "    Max Y: " << info.coordinates.north << "\n"
       << "    Zoom Level: " << std::fixed << std::setprecision(14) << info.zoom_level;
    return os;
}