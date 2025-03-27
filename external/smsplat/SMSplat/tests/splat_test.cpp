#include <gtest/gtest.h>
#include "splat.h"
#include "splat_config.h"
#include <cstring>
#include <string>
#include <memory>
#include <cstdlib>
#include "sm_splat_info.h"

class SplatTest : public ::testing::Test {
protected:
    // Add this member variable
    std::string elev_path;

    void SetUp() override {
        splat = std::make_unique<SplatProcessor>();
        std::string homeDir = std::getenv("HOME") ? std::getenv("HOME") : "";
        elev_path = homeDir + "/.cache/splat/elev_data/";
    }

    // Helper function to create a default params struct
    SMSplatInputInfo createDefaultParams() {
         std::string homeDir = std::getenv("HOME") ? std::getenv("HOME") : ""; // Get home directory

        return SMSplatInputInfo{
            "meghu",                                 // transmitter_name
            40.96400739,                               // transmitter_lat
            -44.66544376,                              // transmitter_lon
            3.0,                                  // transmitter_alt
            "full",                                 // itm_cov_type
            2360.0,                                 // receiver_height
            1.0,                                    // start_angle
            360.0,                                  // end_angle
            25.0,                                   // radius
            1400.0,                                 // frequency
            40.0,                                   // fresnel_zone
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
}

TEST_F(SplatTest, BasicFunctionality) {
    auto params = createDefaultParams();
    params.transmitter_name = "meghu";
    params.transmitter_lat = 40.96400739;
    params.transmitter_lon = -44.66544376;
    params.transmitter_alt = 3.0;
    params.itm_cov_type = "full";
    params.receiver_height = 2360.0;
    params.start_angle = 1.0;
    params.end_angle = 360.0;
    params.radius = 25.0;
    params.frequency = 1400.0;
    params.fresnel_zone = 40.0;

    EXPECT_NO_THROW(splat->setParameters(params));
    EXPECT_NO_THROW(splat->process());
}

TEST_F(SplatTest, InvalidParameters) {
    auto params = createDefaultParams();

    // Test invalid receiver height
    params.receiver_height = -1.0;
    EXPECT_THROW(splat->setParameters(params), std::invalid_argument);

    // Test invalid angles
    params = createDefaultParams();
    params.start_angle = -1.0;
    EXPECT_THROW(splat->setParameters(params), std::invalid_argument);

    params = createDefaultParams();
    params.end_angle = 361.0;
    EXPECT_THROW(splat->setParameters(params), std::invalid_argument);

    // Test invalid radius
    params = createDefaultParams();
    params.radius = -1.0;
    EXPECT_THROW(splat->setParameters(params), std::invalid_argument);

    // Test invalid frequency
    params = createDefaultParams();
    params.frequency = -1.0;
    EXPECT_THROW(splat->setParameters(params), std::invalid_argument);

    // Test invalid Fresnel zone
    params = createDefaultParams();
    params.fresnel_zone = -1.0;
    EXPECT_THROW(splat->setParameters(params), std::invalid_argument);

    // Test invalid ITM coverage type
    params = createDefaultParams();
    params.itm_cov_type = "invalid";
    EXPECT_THROW(splat->setParameters(params), std::invalid_argument);

}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
