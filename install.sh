#!/bin/bash

mkdir -p ./libs
mkdir -p ./libs/include

path=./libs/include/CL/opencl.hpp
if [ ! -e $path ]
then
    printf '\ndownloading opencl.hpp\n'
    git clone https://github.com/KhronosGroup/OpenCL-CLHPP.git ./libs/include/.tmp
    cp -r ./libs/include/.tmp/include/* ./libs/include
    rm -rf ./libs/include/.tmp

fi

path=./libs/include/glm
if [ ! -e $path ]
then
    printf '\ninstalling glm\n'
    git clone https://github.com/g-truc/glm.git ./libs/include/.tmp
    mv -f ./libs/include/.tmp/glm ./libs/include/
    rm -rf ./libs/include/.tmp

fi

DEPENDENCY_FOLDER=~/.dep
LIB_FOLDER=$DEPENDENCY_FOLDER/usr/lib/x86_64-linux-gnu

echo DEPENDENCY_FOLDER = "$DEPENDENCY_FOLDER"
echo LIB_FOLDER = "$LIB_FOLDER"

install_lib()
{
    pkg_name=$1
    printf '\ninstalling %s\n' "$pkg_name"
    cd $DEPENDENCY_FOLDER
    for pkg in `apt-cache pkgnames "$pkg_name"`
    do 
        apt download $pkg
        dpkg -x "$pkg"*.deb $DEPENDENCY_FOLDER
        rm $pkg*.deb
    done
    cd -
    pkg-config --define-prefix "$DEPENDENCY_FOLDER/usr" $LIB_FOLDER/*
}

install_libs()
{
    for lib_name in "$@"
    do
        path=$LIB_FOLDER/$lib_name.so
        if [  ! -e $path ]; then
            install_lib $lib_name
        fi

    done
}

cpy_libs_include()
{
    for include_name in "$@"
    do
        cp -rf $DEPENDENCY_FOLDER/usr/include/$include_name ./libs/include
    done
}

if [  ! -e $DEPENDENCY_FOLDER ]
then
    printf "\ninit  $DEPENDENCY_FOLDER\n"
    mkdir $DEPENDENCY_FOLDER
    printf 'export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:%s/pkgconfig\n' "$LIB_FOLDER" >> ~/.bashrc
    printf 'export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:%s/pkgconfig\n' "$LIB_FOLDER" >> ~/.zshrc
fi


install_libs libglfw ocl-icd opencl-

cpy_libs_include GLFW CL