#include "sm_splat_manager.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include "image_utils.h"
#include <chrono>

SMSplatManager::SMSplatManager() 
    : splat_(std::make_unique<SplatProcessor>()),
      gdal_handler_(std::make_unique<GdalHandler>())
{}

void SMSplatManager::setSplatLrpPath(const std::string& lrp_path) {
    // Simply check if the string is not empty
    if (!lrp_path.empty()) {
        // Call the setLRPFilePath method
        splat_->setLRPFilePath(lrp_path);
    }
}

SMSplatGenInfo SMSplatManager::generate(const SMSplatInputInfo& input_info) {
    auto total_start = std::chrono::high_resolution_clock::now();
    
    try {
        // SPLAT Processing timing
        auto splat_start = std::chrono::high_resolution_clock::now();

        SMSplatGenInfo generatedImageInfo;

        // Run SPLAT analysis directly with input_info
        splat_->setParameters(input_info);
        splat_->process();
        auto splat_end = std::chrono::high_resolution_clock::now();
        auto splat_duration = std::chrono::duration_cast<std::chrono::milliseconds>(splat_end - splat_start);
        std::cout << "TIME:::SPLAT Processing time----------: " << splat_duration.count() << " ms" << std::endl;
        // Get the generated image info
        generatedImageInfo = splat_->getGeneratedImageInfo();
        splat_->resetSplat();

        // Get the image buffer from SPLAT
       const cv::Mat &inputBuffer = splat_->getImageBuffer();
       cv::Mat outputBuffer;
       ProjectedBounds projected_bounds;

       if (inputBuffer.empty())
       {
           std::cerr << "Error: Input image buffer is empty" << std::endl;
           throw std::runtime_error("Empty image buffer from SPLAT");
        }
        
        std::cout << "Input buffer size: " << inputBuffer.size() << " bytes" << std::endl;

        auto gdal_start = std::chrono::high_resolution_clock::now();
        if (gdal_handler_->translate_and_reproject_image_buffer(
                inputBuffer,
                outputBuffer,
                projected_bounds,
                {generatedImageInfo.coordinates.west,
                generatedImageInfo.coordinates.north,
                generatedImageInfo.coordinates.east,
                generatedImageInfo.coordinates.south})) {
            
            
            // Update the image size in generatedImageInfo with the warped image size
            generatedImageInfo.image_size.width = projected_bounds.width;
            generatedImageInfo.image_size.height = projected_bounds.height;
            std::cout << "✅ GDAL Processing Complete ===" << std::endl;

            
        } else {
            throw std::runtime_error("Failed to create reprojected PNG");
        }
        auto gdal_end = std::chrono::high_resolution_clock::now();
        auto gdal_duration = std::chrono::duration_cast<std::chrono::milliseconds>(gdal_end - gdal_start);
        std::cout << "TIME:::GDAL Processing time----------: " << gdal_duration.count() << " ms" << std::endl;

        // Calculate zoom level from projected bounds
        generatedImageInfo.zoom_level = ImageUtils::calculateZoomLevel(
            projected_bounds.width,
            projected_bounds.height,
            projected_bounds.min_y,
            projected_bounds.min_x,
            projected_bounds.max_x,
            projected_bounds.max_y);

        generatedImageInfo.image_ = outputBuffer;
        cv::imwrite("output.png", outputBuffer);
        generatedImageInfo.image_file = "output.png";
        
        std::cout << "\nGenerated image info:"
                  << "\n  Image file: " << generatedImageInfo.image_file
                  << "\n  Image size: " << generatedImageInfo.image_size.width
                  << "x" << generatedImageInfo.image_size.height
                  << "\n  Coordinates: N:" << generatedImageInfo.coordinates.north
                  << " S:" << generatedImageInfo.coordinates.south
                  << " E:" << generatedImageInfo.coordinates.east
                  << " W:" << generatedImageInfo.coordinates.west
                  << "\n  Warped Image bounds: "
                  << "\n    Width: " << projected_bounds.width
                  << "\n    Height: " << projected_bounds.height
                  << "\n    Min Y: " << projected_bounds.min_y
                  << "\n    Min X: " << projected_bounds.min_x
                  << "\n    Max X: " << projected_bounds.max_x
                  << "\n    Max Y: " << projected_bounds.max_y
                  << "\n    Zoom Level: " << std::fixed << std::setprecision(14) << generatedImageInfo.zoom_level
                  << std::endl;

        // Total timing
        auto total_end = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start);
        std::cout << "TIME:::Total Processing time----------: " << total_duration.count() << " ms" << std::endl;

        std::cout << "✅ SMSplatManager::generate Complete ===" << std::endl;
        return generatedImageInfo;
    } catch (const std::exception &e) {
        spdlog::error("Error in Generate processing: {}", e.what());
        throw;
    }
}


SMSplatGenInfo SMSplatManager::genSimulatedGenInfo(const SMSplatInputInfo& input) {
    spdlog::info("Simulating SPLAT processing (mock implementation)");

    // Create a mock SMSplatGenInfo based on the input
    SMSplatGenInfo result;

    // Use input data for the result fields where appropriate
    if (input.transmitter_name) {
        result.coverage_name = input.transmitter_name;
    } else {
        result.coverage_name = "mock_coverage";
    }
    
    // Set mock image size
    result.image_size.width = 800;
    result.image_size.height = 600;

    // Set mock coordinates based on transmitter site
    // In a real implementation, these would be calculated based on the coverage area
    result.coordinates.north = 40.7128;  // Mock for New York area
    result.coordinates.south = 40.6128;
    result.coordinates.east = -73.9644;
    result.coordinates.west = -74.0644;

    // Set mock zoom level
    result.zoom_level = 12;

    // Set transmitter coordinates (in a real implementation, these would come from parsing the QTH file)
    result.transmitter_lat = 40.7128;
    result.transmitter_lon = -74.0060;

    // Set coverage radius from input
    result.coverage_radius = input.radius;
    result.image_file = "";

    // Generate a dummy image buffer using OpenCV Mat instead of raw vector
    // Create a gradient pattern
    cv::Mat image(result.image_size.height, result.image_size.width, CV_8UC3);
    
    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            // Create a gradient pattern (blue to red, horizontally)
            uchar b = static_cast<uchar>((1.0 - static_cast<double>(x) / image.cols) * 255);
            uchar r = static_cast<uchar>(static_cast<double>(x) / image.cols * 255);
            uchar g = static_cast<uchar>(static_cast<double>(y) / image.rows * 255);
            
            // Set pixel color (BGR format in OpenCV)
            image.at<cv::Vec3b>(y, x) = cv::Vec3b(b, g, r);
        }
    }
    
    // Add a simulated transmitter location marker at the center
    int centerX = image.cols / 2;
    int centerY = image.rows / 2;
    int radius = 10;
    cv::circle(image, cv::Point(centerX, centerY), radius, cv::Scalar(0, 0, 255), -1); // Filled red circle
    cv::circle(image, cv::Point(centerX, centerY), radius + 2, cv::Scalar(0, 0, 0), 2); // Black border

    // Add some simulated signal strength rings
    for (int r = 50; r <= 200; r += 50) {
        cv::circle(image, cv::Point(centerX, centerY), r, cv::Scalar(0, 255, 0), 2);
    }
    
    // Assign the OpenCV Mat to the result image_ field
    result.image_ = image;

    return result;
}
