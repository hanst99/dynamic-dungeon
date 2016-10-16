#!/bin/bash

# adjusts mirror list to prefer german mirrors
# If you're building from outside of germany,
# or mirror resolution fails for some reason,
# you might want to change this list
cat <<'EOF' >/etc/pacman.d/mirrorlist
Server = http://mirror.23media.de/archlinux/$repo/os/$arch
Server = https://arch.32g.eu/$repo/os/$arch
Server = http://artfiles.org/archlinux.org/$repo/os/$arch
Server = http://mirror5.bastelfreak.org/archlinux/$repo/os/$arch

#backup default mirror
Server = https://mirrors.kernel.org/archlinux/$repo/os/$arch
EOF

cat <<EOF
####################################################################
# Performing full system upgrade and installing required packages. #
# This may take a while.                                           #
####################################################################
EOF

pacman -Syu --noconfirm
pacman -S cmake mingw-w64-toolchain --needed --noconfirm

cat <<EOF
####################################################################
# Downloading SDL Windows libraries                                #
####################################################################
EOF

downloadAndExtract() {
    echo Downloading "$1"
    curl --silent --fail --show-error -L "$1" | tar xzC /tmp
}

fixAccessRightsAndInstall() {
    dir="$1/x86_64-w64-mingw32"
    # root can read and write, others can read but not write,
    # everyone gets to execute if file was originally executable
    chmod -R u+rwX,go+rX,go-w "$dir"
    rsync -avr "$dir" /usr/
}

downloadAndExtract https://www.libsdl.org/release/SDL2-devel-2.0.4-mingw.tar.gz \
    && fixAccessRightsAndInstall /tmp/SDL2-2.0.4 &
downloadAndExtract https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.1-mingw.tar.gz \
    && fixAccessRightsAndInstall /tmp/SDL2_image-2.0.1 &
