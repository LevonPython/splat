#include "gdal_handler.h"

#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <gdal_alg.h>
#include <fstream>
#include "image_utils.h"

GdalHandler::GdalHandler()
{
    GDALAllRegister();
};

GdalHandler::~GdalHandler(){};

bool GdalHandler::translate_and_reproject_image_buffer(const cv::Mat& input, cv::Mat& output, 
                                           ProjectedBounds& projected_bounds,
                                           const std::vector<double>& geoCoords) {
    assert(geoCoords.size() == 4);
    assert(!input.empty() && input.channels() == 3);
    int inWidth = input.cols;
    int inHeight = input.rows;
    int num_bands = 3;
    GDALAllRegister();
    GDALSetCacheMax(1024 * 1024 * 1024); // 1GB


    GDALDriver *memDriver = GetGDALDriverManager()->GetDriverByName("MEM");
    GDALDataset *dataset = memDriver->Create("", inWidth, inHeight, num_bands, GDT_Byte, nullptr);

    std::vector<cv::Mat> bgrChannels(3);
    cv::split(input, bgrChannels);

    dataset->GetRasterBand(1)->RasterIO(GF_Write, 0, 0, inWidth, inHeight, bgrChannels[2].data, inWidth, inHeight, GDT_Byte, 0, 0); // R
    dataset->GetRasterBand(2)->RasterIO(GF_Write, 0, 0, inWidth, inHeight, bgrChannels[1].data, inWidth, inHeight, GDT_Byte, 0, 0); // G
    dataset->GetRasterBand(3)->RasterIO(GF_Write, 0, 0, inWidth, inHeight, bgrChannels[0].data, inWidth, inHeight, GDT_Byte, 0, 0); // B
    double geoTransform[6] = { geoCoords[0],   (geoCoords[2]-geoCoords[0])/inWidth, 0.0, geoCoords[1], 0.0, (geoCoords[3]-geoCoords[1])/inHeight };
    dataset->SetGeoTransform(geoTransform);
    dataset->SetProjection("EPSG:4326");
    
    OGRSpatialReference dstSRS;
    dstSRS.importFromEPSG(3857); // Web Mercator
    char* dstWKT = nullptr;
    dstSRS.exportToWkt(&dstWKT);

    GDALWarpOptions* warpOptions = GDALCreateWarpOptions();
    warpOptions->eResampleAlg = GRA_NearestNeighbour;
    warpOptions->hSrcDS = dataset;
    warpOptions->nBandCount = num_bands;
    warpOptions->panSrcBands = (int*)CPLMalloc(num_bands * sizeof(int));
    warpOptions->panDstBands = (int*)CPLMalloc(num_bands * sizeof(int));
    for (int i = 0; i < num_bands; i++) {
        warpOptions->panSrcBands[i] = i + 1;
        warpOptions->panDstBands[i] = i + 1;
    }


    warpOptions->dfWarpMemoryLimit = 1024 * 1024 * 1024; // 1GB


    GDALDataset* warpedDataset = (GDALDataset*)GDALAutoCreateWarpedVRT(dataset,
                                                                       dataset->GetProjectionRef(), dstWKT,
                                                                       GRA_NearestNeighbour, 0.1, warpOptions);

    if (!warpedDataset) {
        std::cerr << "Failed to create warped dataset" << std::endl;
        GDALDestroyWarpOptions(warpOptions);
        GDALClose(dataset);
        CPLFree(dstWKT);
        return false;
    }

    double warpedGeoTransform[6];
    warpedDataset->GetGeoTransform(warpedGeoTransform);
    int warpedWidth = warpedDataset->GetRasterXSize();
    int warpedHeight = warpedDataset->GetRasterYSize();
    projected_bounds.min_x = warpedGeoTransform[0];
    projected_bounds.max_x = warpedGeoTransform[0] + (warpedGeoTransform[1] * warpedWidth);
    projected_bounds.min_y = warpedGeoTransform[3];
    projected_bounds.max_y = warpedGeoTransform[3] + (warpedGeoTransform[5] * warpedHeight);
    projected_bounds.width = warpedWidth;
    projected_bounds.height = warpedHeight;

    // Create a 4-channel output (RGBA)
    output.create(warpedHeight, warpedWidth, CV_8UC4); // 4 channels for RGBA


    std::vector<cv::Mat> outChannels(4);
    cv::split(output, outChannels);
    
    // > 1100 ms
    warpedDataset->GetRasterBand(1)->RasterIO(GF_Read, 0, 0, warpedWidth, warpedHeight, outChannels[2].data, warpedWidth, warpedHeight, GDT_Byte, 0, 0);
    warpedDataset->GetRasterBand(2)->RasterIO(GF_Read, 0, 0, warpedWidth, warpedHeight, outChannels[1].data, warpedWidth, warpedHeight, GDT_Byte, 0, 0);
    warpedDataset->GetRasterBand(3)->RasterIO(GF_Read, 0, 0, warpedWidth, warpedHeight, outChannels[0].data, warpedWidth, warpedHeight, GDT_Byte, 0, 0);


    // Set the alpha channel (transparency)
    for (int y = 0; y < warpedHeight; ++y) {
        for (int x = 0; x < warpedWidth; ++x) {
            // If the pixel is white (R == G == B == 255), set alpha to 0 (transparent)
            if (outChannels[0].at<uchar>(y, x) == 255 && outChannels[1].at<uchar>(y, x) == 255 && outChannels[2].at<uchar>(y, x) == 255) {
                outChannels[3].at<uchar>(y, x) = 0;  // Transparent
            } else {
                outChannels[3].at<uchar>(y, x) = 255;  // Opaque
            }
        }
    }
    
    cv::merge(outChannels, output);
    //cv::imwrite("warped_image.png", output);

    // CLeanup
    GDALDestroyWarpOptions(warpOptions);
    GDALClose(dataset);
    GDALClose(warpedDataset);
    CPLFree(dstWKT);

    return true;
}
