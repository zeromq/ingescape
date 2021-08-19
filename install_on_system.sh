echo "Deploying ingescape library and its dependencies in $CODEROOT/sysroot"
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DOSX_UNIVERSAL=ON -DWITH_DEPS=ON
sudo make -j8 -C build install
sudo rm -Rf build
