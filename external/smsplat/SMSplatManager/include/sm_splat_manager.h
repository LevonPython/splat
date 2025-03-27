#ifndef SM_SPLAT_MANAGER_H
#define SM_SPLAT_MANAGER_H

#include "splat.h"
#include <memory>
#include <map>
#include <string>
#include <chrono>
#include <filesystem>
#include <spdlog/spdlog.h>
#include "sm_splat_info.h"
#include "gdal_handler.h"
#include "image_utils.h"

class SMSplatManager {
   public:
    SMSplatManager();
    ~SMSplatManager() = default;

    // Function to run SPLAT analysis with parameters
    SMSplatGenInfo generate(const SMSplatInputInfo& input_info);

    // Process SPLAT viewshed and return the generated info
    SMSplatGenInfo genSimulatedGenInfo(const SMSplatInputInfo& input);

    void setSplatLrpPath(const std::string& lrp_path);

   private:
    std::unique_ptr<SplatProcessor> splat_;
    std::unique_ptr<GdalHandler> gdal_handler_;
};

#endif  // SM_SPLAT_MANAGER_H
