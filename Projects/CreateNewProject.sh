mkdir $1
mkdir $1/Source
mkdir $1/Shaders

cp -p Blank/Shaders/*.glsl $1/Shaders
cp -p Blank/makefile $1/makefile
sed -i s/PROJECT_NAME/$1/ $1/makefile
cp -p Blank/Source/Blank.cpp $1/Source/$1.cpp
sed -i s/PROJECT_NAME/$1/ $1/Source/$1.cpp
