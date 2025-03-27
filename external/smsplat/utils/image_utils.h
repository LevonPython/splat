#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cstring> // For std::memcpy
#include <sstream> // For std::stringstream

class ImageUtils {
public:
    // Zoom calculation constants and methods
    static constexpr double EARTH_CIRCUMFERENCE = 40008000.0;  // meters
    static constexpr int TILE_SIZE = 256;  // pixels

    static double calculateZoomLevel(
        int image_width,
        int image_height,
        double min_y,
        double min_x,
        double max_x,
        double max_y);

private:
    static double calculateResolution(
        double width_in_meters,
        double height_in_meters,
        int image_width,
        int image_height);
};

#endif // IMAGE_UTILS_H 