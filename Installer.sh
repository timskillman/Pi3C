#!/bin/sh



echo "Installing SDL 2.0 libraries ..."

# Install packages
PACKAGES="libsdl2-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-net-dev"
apt-get update
apt-get upgrade -y
apt-get install $PACKAGES -y

