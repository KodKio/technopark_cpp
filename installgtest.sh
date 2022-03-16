git clone https://github.com/google/googletest.git -b release-1.11.0
cd googletest
mkdir build
cd build
cmake .. -DBUILD_GMOCK=OFF
make
sudo make install
cd ../..