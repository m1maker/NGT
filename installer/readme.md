# NGT Installer Maker
This directory is all the necessary things to make NGT as an installer version.
You need NSIS installer.

[Download NSIS Latest](https://sourceforge.net/projects/nsis/files/latest/download)

## How to build
First, compile your NGT into executable file.

Next, create a folder called `build` if doesn't already exist.

Next, put all needed files to the build directory. Don't worry, that file will not include on commit, so you don't need to worry about unnecessary files in the repository.

Next, open installer.nsi in your text editor, and modify the version.

Next, right click on installer.nsi file and click `Compile NSIS Script`.

That's it!

All your files will not be included in commits so you don't need to worry about unnecessary files in the repository. Git will ignore all files, except the installer.nsi, readme.md.

Note. NGTScript.exe should be in the build directory, along with DLL files. All main files must not be in subdirectory. For example, build/NGTScript.exe, not build/NGT_Main/NGTScript.exe.