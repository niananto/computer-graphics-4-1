@REM REM How to Use this bat file 
@REM REM Follow step by step below
:: Install or Download MingGW and save it in a specific folder
:: Download Freeglut and config it below
::   -copy include and bin folder into MinGW
::   -go to "bin folder" and copy freeglut.dll in to System32 and SysWOW64
:: Extract the bat.(zip) and copy it into your folder
:: set your project name like this "main.c"
:: Run This.bat 
:: Done

del %1.exe
del %1.o

@REM
echo "Running \m"
@REM gcc -c -o main.o main.cpp -I\"D:\CodeBlocks\MinGW\x86_64-w64-mingw32\include"
g++ -c -o %1.o %1.cpp -I\"C:\msys64\mingw64\bin"
@REM cls
echo "Compiling Graphic"
g++ -o %1.exe  %1.o -L\"C:\msys64\mingw64\lib -lopengl32 -lglu32 -lfreeglut"
@REM cls
echo "App Running"
%1.exe