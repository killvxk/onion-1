call "%VS120COMNTOOLS%\VsDevCmd.bat"

msbuild onion.sln /t:Rebuild /p:Configuration=Release /p:Platform="Win32"
msbuild onion.sln /t:Rebuild /p:Configuration=Debug /p:Platform="Win32"