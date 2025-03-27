#include "image_utils.h"


double ImageUtils::calculateZoomLevel(
    int image_width,
    int image_height,
    double min_y,
    double min_x,
    double max_x,
    double max_y) {
    
    std::cout << "\n=== Calculating Zoom Level ===" << std::endl;

    // Calculate width and height in meters
    double width_in_meters = max_x - min_x;
    double height_in_meters = max_y - min_y;

    // Get resolution in meters per pixel
    double resolution = calculateResolution(
        width_in_meters,
        height_in_meters,
        image_width,
        image_height);

    // Calculate zoom level using the same formula as Python
    double zoom_level = std::log2(EARTH_CIRCUMFERENCE / (resolution * TILE_SIZE));
    
    // Format zoom_level with 14 decimal places using stringstream
    std::stringstream ss;
    ss << std::fixed << std::setprecision(14) << zoom_level;
    std::cout << "Calculated zoom level: " << ss.str() << std::endl;
    std::cout << "✅ Zoom Calculation Complete ===" << std::endl;
    
    return zoom_level;  // Return the actual double value
}

double ImageUtils::calculateResolution(
    double width_in_meters,
    double height_in_meters,
    int image_width,
    int image_height) {
    
    double resolution_x = width_in_meters / image_width;
    double resolution_y = height_in_meters / image_height;
    
    return std::max(resolution_x, resolution_y);
}
