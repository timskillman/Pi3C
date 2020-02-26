set Template=Resources/Template_Project
set Projects=MyProjects/%1

echo Creating Project MyProjects/%1 ...

mkdir "%Projects%"
mkdir "%Projects%\Source"
mkdir "%Projects%\Shaders"

xcopy "%Template%\Shaders\*" "%Projects%\Shaders" /i
copy "%Template%\makefile" "%Projects%\makefile"
powershell -Command "(gc %Projects%\makefile) -replace 'PROJECT_NAME','%1' | Out-File -encoding ASCII %Projects%\makefile"

echo -Creating VisualStudio Solution ...

copy "%Template%\Source\SOLUTION_NAME.cpp" "%Projects%\Source\%1.cpp"
powershell -Command "(gc %Projects%\Source\%1.cpp) -replace 'PROJECT_NAME','%1' | Out-File -encoding ASCII %Projects%\Source\%1.cpp"

copy "%Template%\SOLUTION_NAME.sln" "%Projects%\%1.sln"
powershell -Command "(gc %Projects%\%1.sln) -replace 'SOLUTION_NAME','%1' | Out-File -encoding ASCII %Projects%\%1.sln"

copy "%Template%\SOLUTION_NAME.vcxproj" "%Projects%\%1.vcxproj"
powershell -Command "(gc %Projects%\%1.vcxproj) -replace 'SOLUTION_NAME','%1' | Out-File -encoding ASCII %Projects%\%1.vcxproj"

copy "%Template%\SOLUTION_NAME.vcxproj.filters" "%Projects%\%1.vcxproj.filters"
powershell -Command "(gc %Projects%\%1.vcxproj.filters) -replace 'SOLUTION_NAME','%1' | Out-File -encoding ASCII %Projects%\%1.vcxproj.filters"

copy "%Template%\SOLUTION_NAME.vcxproj.user" "%Projects%\%1.vcxproj.user"
copy "%Template%\dependencies.props" "%Projects%\dependencies.props"
