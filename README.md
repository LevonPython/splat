#How to build
```
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/home/levonyeghiazaryan/Qt/6.6.3/gcc_64 .

cmake --build build --config Release
```

#How to run a test
```
./build/bin/sm_splat_manager --transmitter-name "meghu" --transmitter-lat 40.96400739 --transmitter-lon -44.66544376 --transmitter-alt 3.0 --receiver-height 2360.0 --start-angle 0.0 --end-angle 360.0 --radius 25.0 --frequency 1400.0 --fresnel-zone 40.0 --elev-path "/home/levonyeghiazaryan/.cache/splat/elev_data/" --dbm --olditm --sc --ngs --metric 
```
