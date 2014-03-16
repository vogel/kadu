1. Install Visual Studio 2013 Express for Windows Desktop. You will need to
   register to use it for free for more than 30 days.
   http://go.microsoft.com/?linkid=9832280
2. Install Git to %ProgramFiles(x86)%\Git. Select the `Run Git from Windows
   Command Prompt' option during installation.
   http://git-scm.com/
3. Install CMake. Make sure the cmake executable ends up in your PATH.
   http://www.cmake.org/cmake/resources/software.html
4. Install 7-Zip to %ProgramFiles%\7-Zip.
   http://www.7-zip.org/download.html
5. Install Wget for Windows to %ProgramFiles(x86)%\GnuWin32:
   Wget: http://gnuwin32.sourceforge.net/packages/wget.htm
6. Open cmd.exe and execute the following. (You can use other root directory
   than "%USERPROFILE%\kadu". Only make sure the path does not contain spaces).
   > mkdir "%USERPROFILE%\kadu"
   > cd /D "%USERPROFILE%\kadu"
   > git clone git://gitorious.org/kadu/kadu.git git
   > git\varia\win32\build.bat
