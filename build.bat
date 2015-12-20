set path=%PATH%;C:\program files (x86)\mingw\bin
set path=%PATH%;C:\program files (x86)\cmake\bin
cd "C:\users\oleg b\rpg\build\cmakefiles\main.dir"
del main.cpp.obj
cd ../../../
call install.bat
cd build
mingw32-make
move main.exe "C:\users\oleg b\rpg\build\1"
pause