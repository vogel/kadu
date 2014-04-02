1. Install Visual Studio 2012 Express for Windows Desktop. You will need to
   register to use it for free for more than 30 days.
   http://go.microsoft.com/?linkid=9816758
2. Install Visual Studio 2012 Update 2.
   http://download.microsoft.com/download/7/8/8/78863D92-FAA5-4692-8B51-381901E9BE7F/VS2012.2.exe
3. Install Git to %ProgramFiles(x86)%\Git. Select Run Git from Windows Command
   Prompt option during installation.
   http://git-scm.com/
4. Install Subversion client. Make sure svn executable ends up in your PATH.
   http://www.sliksvn.com/en/download
5. Install CMake. Make sure cmake executable ends up in your PATH.
   http://www.cmake.org/cmake/resources/software.html
   
   To be able to target Windows XP using VC11, the minimum required version
   of CMake is 2.8.11, which has not been released yet
   at the time of writing; you will need to use nightly build:
   http://www.cmake.org/files/dev/?C=M;O=D
6. Install 7-Zip to %ProgramFiles%\7-Zip.
   http://www.7-zip.org/download.html
7. Install Wget for Windows to %ProgramFiles(x86)%\GnuWin32:
   Wget: http://gnuwin32.sourceforge.net/packages/wget.htm
8. Open cmd.exe and execute (TODO: remind about no spaces)
   > mkdir "%USERPROFILE%\kadu"
   > cd /D "%USERPROFILE%\kadu"
   > git clone git://gitorious.org/kadu/kadu.git git
   > git\varia\win32\build.bat
