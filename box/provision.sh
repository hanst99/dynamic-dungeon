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

# pacman function to avoid repeating --noconfirm over and over
Pman() {
    pacman "$@" --noconfirm
}

# full system upgrade (this may take a while)
Pman -Syu

# install some required packages
Pman -S cmake mingw-w64-toolchain
