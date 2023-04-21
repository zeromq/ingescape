echo "Deploying ingescape library and its dependencies on the system"
REM make sure all submodules are here
git submodule update --init --recursive 
mkdir build
cmake -S . -B build -A x64 -DCMAKE_INSTALL_PREFIX="%ProgramFiles%\ingescape\library" -DCMAKE_BUILD_TYPE=Release -DWITH_DEPS=ON
cmake --build build --target ALL_BUILD --config Release -- /nologo /verbosity:minimal /maxcpucount 
cmake --install build --config Release
rmdir /s /q build
