echo "Deploying ingescape library and its dependencies on the system"
git submodule update --init --recursive #make sure all dependecies are here
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DOSX_UNIVERSAL=ON -DWITH_DEPS=ON
sudo make -j8 -C build install
sudo rm -Rf build
