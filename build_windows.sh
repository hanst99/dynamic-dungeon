pushd $(dirname $0) > /dev/null
if vagrant up; then
    mkdir -p build
    vagrant ssh -c 'cd /vagrant/build; cmake -DCMAKE_TOOLCHAIN_FILE=/vagrant/src/Toolchain-mingw-w64-x86_64.cmake /vagrant/src;make'
    vagrant ssh -c 'cp /usr/x86_64-w64-mingw32/bin/{libgcc_s_seh-1,libstdc++-6,libwinpthread-1,SDL2,SDL2_image,libpng16-16}.dll /vagrant/build'
fi
popd >/dev/null
