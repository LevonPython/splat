#include <gtest/gtest.h>
#include "splat.h"
#include "splat_config.h"
#include <cstring>
#include <string>
#include <memory>

class SplatTest : public ::testing::Test {
protected:
    void SetUp() override {
        splat = std::make_unique<SplatProcessor>();
    }

    // Helper function to create a default params struct
    SplatProcessor::SplatProcessorParams createDefaultParams() {
        return {
            "/home/levonyeghiazaryan/repos/view_shed_project/scripts/output/splat/meghu.qth",  // transmitter_site
            "full",                                 // itm_cov_type
            2360.0,                                 // receiver_height
            1.0,                                    // start_angle
            360.0,                                  // end_angle
            true,                                   // dbm
            true,                                   // olditm
            true,                                   // sc
            true,                                   // ngs
            25.0,                                   // radius
            1400.0,                                 // frequency
            40.0,                                   // fresnel_zone
            true,                                   // metric
            "/home/levonyeghiazaryan/repos/view_shed_project/elev_data/", // elev_path
            "/home/levonyeghiazaryan/repos/view_shed_project/scripts/output/splat/coverage_map_meghu.ppm", // ppm_path
            "/home/levonyeghiazaryan/repos/view_shed_project/scripts/output/splat/coverage_map_meghu.kml"  // kml_path
        };
    }

    std::unique_ptr<SplatProcessor> splat;

    void TearDown() override {
        // Cleanup code if needed
    }
};

// Test HD_MODE configuration
TEST_F(SplatTest, ConfigurationTest) {
    #if HD_MODE
        EXPECT_EQ(HD_MODE, 1);
    #else
        EXPECT_EQ(HD_MODE, 0);
    #endif
    
    // Test MAXPAGES is within valid range
    EXPECT_GE(MAXPAGES, 1);
    EXPECT_LE(MAXPAGES, 64);

}
// Test receiver height parameter
TEST_F(SplatTest, ReceiverHeightValidation) {
    auto params = createDefaultParams();
    
    // Test valid height
    params.receiver_height = 100.0;
    EXPECT_NO_THROW(splat->setParameters(params));
    EXPECT_NO_THROW(splat->process());

    // Test zero height
    params.receiver_height = 0.0;
    EXPECT_NO_THROW(splat->setParameters(params));
 //   EXPECT_NO_THROW(splat->process());

    // Test negative height (should throw)
    params.receiver_height = -1.0;
    EXPECT_THROW(splat->setParameters(params), std::invalid_argument);

}
// Transmitter site Check 
TEST_F(SplatTest, TransmitterSiteValidation) {
    auto params = createDefaultParams();

    // Test valid transmitter site
    params.transmitter_site = "/home/levonyeghiazaryan/repos/view_shed_project/scripts/output/splat/meghu.qth";
    EXPECT_NO_THROW(splat->setParameters(params));
//    EXPECT_NO_THROW(splat->process());

    // Test invalid transmitter site
    params.transmitter_site = "/home/levonyeghiazaryan/repos/view_shed_project/scripts/output/splat/throw_error.qth";
    EXPECT_THROW(splat->setParameters(params), std::invalid_argument);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
