mkdir $1
mkdir $1/Source
mkdir $1/Shaders

cp -p Blank/Shaders/*.glsl $1/Shaders
cp -p Blank/makefile $1/makefile
sed -i s/PROJECT_NAME/$1/ $1/makefile

cp -p Blank/Source/Blank.cpp $1/Source/$1.cpp
sed -i s/PROJECT_NAME/$1/ $1/Source/$1.cpp

cp -p Blank/Blank.sln $1/$1.sln
sed -i s/SOLUTION_NAME/$1/g $1/$1.sln

cp -p Blank/Blank.vcxproj $1/$1.vcxproj
sed -i s/SOLUTION_NAME/$1/g $1/$1.vcxproj

cp -p Blank/Blank.vcxproj $1/$1.vcxproj.filters
sed -i s/SOLUTION_NAME/$1/g $1/$1.vcxproj.filters

cp -p Blank/Blank.vcxproj.user $1/$1.vcxproj.user
cp -p Blank/dependencies.props $1/dependencies.props
