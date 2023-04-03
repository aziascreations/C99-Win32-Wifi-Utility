@echo off

cd /d %~dp0

call "D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

rmdir build /s /q
mkdir build
cd build
cmake ..
cmake --build . --config Debug
cmake --build . --config Release
cmake --build . --config RelWithDebInfo
cmake --build . --config MinSizeRel
cd ..

::cd build
::mkdir x64
::mkdir arm
::
::cd x64
::cmake ../.. -A Win64
::cmake --build . --config Debug
::cmake --build . --config Release
::cmake --build . --config RelWithDebInfo
::cmake --build . --config MinSizeRel
::
::cd ../arm
::cmake ../.. -A ARM
::cmake --build . --config Debug
::cmake --build . --config Release
::cmake --build . --config RelWithDebInfo
::cmake --build . --config MinSizeRel
::
::cd ../..

pause
