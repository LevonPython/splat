#ifndef GDAL_HANDLER_H
#define GDAL_HANDLER_H

#include <gdal/gdal_alg.h>
#include <gdal/gdal_priv.h>
#include <gdal/gdal_utils.h>
#include <gdal/gdalwarper.h>
#include <gdal/cpl_conv.h>
#include <gdal/ogrsf_frmts.h>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <opencv2/opencv.hpp>

struct ProjectedBounds {
    int width;
    int height;
    double min_y;
    double min_x;
    double max_x;
    double max_y;
};

class GdalHandler {
public:
    GdalHandler();
    ~GdalHandler();

    bool translate_and_reproject_image_buffer(const cv::Mat& input, cv::Mat& output, 
                                           ProjectedBounds& projected_bounds,
                                           const std::vector<double>& geoCoords);

};

#endif // GDAL_HANDLER_H
