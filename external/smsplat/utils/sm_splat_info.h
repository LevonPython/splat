#ifndef SM_SPLAT_INFO_H
#define SM_SPLAT_INFO_H

#include <iosfwd>  // For std::ostream forward declaration
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

struct SMSplatInputInfo {
    const char* transmitter_name;
    double transmitter_lat;
    double transmitter_lon;
    double transmitter_alt;
    const char* itm_cov_type;
    double receiver_height;  // receiver height for itm coverage loss analysis
    double start_angle;      // start angle for itm coverage loss analysis
    double end_angle;        // end angle for itm coverage loss analysis
    double radius;
    double frequency;
    double fresnel_zone;
};

struct SMSplatGenInfo {
    const char* coverage_name;
    const char* image_file;
    struct {
        int width;
        int height;
    } image_size;
    struct {
        double north;  // min_y
        double south;  // max_y
        double east;   // max_x
        double west;   // min_x
    } coordinates;
    double zoom_level;
    double transmitter_lon;
    double transmitter_lat;
    double coverage_radius;
    cv::Mat image_{};
};

// Stream insertion operator overloads
std::ostream& operator<<(std::ostream& os, const SMSplatInputInfo& info);
std::ostream& operator<<(std::ostream& os, const SMSplatGenInfo& info);

#endif  // SM_SPLAT_INFO_H