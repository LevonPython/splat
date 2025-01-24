# SPLAT! RF Signal Propagation Tool

SPLAT! (Signal Propagation, Loss, And Terrain) is an RF Signal Propagation, Loss, And Terrain analysis tool for frequencies between 20 MHz and 20 GHz.

## Building SPLAT!

### Prerequisites

- C++ compiler (GCC recommended)
- CMake (version 3.10 or higher)
- bz2 library
- Google Test (for running tests)

On Ubuntu/Debian systems, install the prerequisites with:

bash
sudo apt-get update
sudo apt-get install build-essential cmake libbz2-dev libgtest-dev


### Basic Build Instructions

1. Clone the repository:
bash
git clone <repository-url>
cd splat


2. Create and enter the build directory:
bash
mkdir build
cd build


3. Configure and build with default settings:
bash
Configure
cmake ..
Build
cmake --build . --target build_all


### Configuration Options

SPLAT can be configured with the following options:

- `HD_MODE`: Enable/disable HD mode (OFF by default)
- `MAXPAGES`: Set maximum pages value (4 by default, range: 1-64)

To build with specific options:

bash
cmake -DHD_MODE=ON -DMAXPAGES=8 ..
cmake --build . --target build_all


### Build Variants

1. Standard Resolution Build (default):
bash
cmake -DHD_MODE=OFF -DMAXPAGES=4 ..
cmake --build . --target build_all

2. HD Resolution Build:
bash
cmake -DHD_MODE=ON -DMAXPAGES=8 ..
cmake --build . --target build_all



### Building Tests

To build and run the tests:
bash
Configure with testing enabled
cmake -DBUILD_TESTING=ON ..
Build
cmake --build . --target build_all
Run tests
ctest


### Project Structure

splat/
├── src/ # Source files
│ ├── splat.cpp
│ ├── splat.h
│ └── ...
├── include/ # Header files
│ └── splat_config.h
├── utils/ # Utility programs
│ ├── citydecoder.c
│ ├── usgs2sdf.c
│ └── ...
├── tests/ # Test files
│ └── splat_test.cpp
└── cmake/ # CMake configuration files
└── splat_config.h.in


### Build Outputs

After successful compilation, you'll find:
- Main SPLAT executable: `build/splat`
- Utility programs in `build/utils/`:
  - citydecoder
  - usgs2sdf
  - srtm2sdf
  - fontdata
  - bearing

### Installation

To install SPLAT and its utilities:
bash
sudo cmake --install .


This will install:
- Executables to `/usr/local/bin/`
- Headers to `/usr/local/include/splat/`

### Troubleshooting

1. If CMake can't find bz2:
bash
sudo apt-get install libbz2-dev



2. If you get compilation errors related to HD_MODE or MAXPAGES:
- Check that splat_config.h is being generated correctly
- Verify the values in your CMake configuration

3. For test-related issues:

bash
sudo apt-get install libgtest-dev


### Contributing

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

### License

[Add your license information here]

### Contact

[Add contact information here]
