#!/bin/bash
#
# To use this file type 'sh CreateProject.sh YourProjectName'
#

Template="Resources/Template_Project"
Projects="MyProjects/$1"

echo Creating Project MyProjects/$1 ...

mkdir $Projects
mkdir $Projects/source
mkdir $Projects/assets
mkdir $Projects/assets/shaders
mkdir $Projects/assets/textures
mkdir $Projects/assets/models

echo -Copying Shaders ...
echo -Creating makefile ...

cp -p $Template/assets/shaders/*.glsl $Projects/assets/shaders
cp -p $Template/makefile $Projects/makefile
sed -i s/PROJECT_NAME/$1/ $Projects/makefile

echo -Creating VisualStudio Solution ...

cp -p $Template/source/SOLUTION_NAME.cpp $Projects/source/$1.cpp
sed -i s/PROJECT_NAME/$1/ $Projects/source/$1.cpp

cp -p $Template/SOLUTION_NAME.sln $Projects/$1.sln
sed -i s/SOLUTION_NAME/$1/g $Projects/$1.sln

cp -p $Template/SOLUTION_NAME.vcxproj $Projects/$1.vcxproj
sed -i s/SOLUTION_NAME/$1/g $Projects/$1.vcxproj

cp -p $Template/SOLUTION_NAME.vcxproj.filters $Projects/$1.vcxproj.filters
sed -i s/SOLUTION_NAME/$1/g $Projects/$1.vcxproj.filters

cp -p $Template/SOLUTION_NAME.vcxproj.user $Projects/$1.vcxproj.user
cp -p $Template/dependencies.props $Projects/dependencies.props
