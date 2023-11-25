# NGT
This is the "NGT" (New Game Toolkit).

This is a game engine that is similar to BGT in its staged functions and classes, but it has some changes.

The engine is written in c++.
In addition, the engine is open source, which allows you to add something new and unique to this game engine.

Also, "BGT" is no longer supported, which caused difficulties in the development of games on "BGT". However, in "NGT", you will not have any problems with this.

Enjoy!New open source game toolkit

# start
To start developing a game using "NGT", you need to know the C++ programming language. If so, you can move on to the next step.
Install Visual Studio Build Tools for NGT Game Development

1. Download it from the official Microsoft website.
https://visualstudio.microsoft.com/ru/thank-you-downloading-visual-studio/?sku=Community&channel=Release&version=VS2022&source=VSLandingPage&cid=2030&passive=false
2. Run the installer and select "Custom" installation.
3. Check only "Desktop development with C++".
4. Click "Install" and wait for the installation to complete.
5. Once the installation is complete, launch Visual Studio.
6. Click on "Create a new project".
7. Select "C++" from the list of project templates.
8. Choose the type of project "Empty project".
9. Enter a name for your project and choose a location to save it.
10. Check this checkbox "Place solution and project in the same directory".
11. Click "Create" to create your project.

Congratulations! You have successfully installed Visual Studio for NGT development and created a new project.
Next;
1. Copy all the contents of the ngt folder in this repository to the project folder you initially chose when creating the project. 
2. Open the Solution Explorer by pressing "Control+Alt+L". 
3. In the tree view, focus on Header files.Open the context menu, click Add, then Existing item, and choose the files NGT.h, BASS.H, NVDACONTROLLERCLIENT.H. 
4. Now, in the tree view, focus on Source files. 
5. Also open the context menu, click Add, then Existing item, and this time choose only NGT.CPP. 
6. Open the menu by pressing Alt and right arrow to focus on the project submenu. 
7. Click Properties. 
8. Select the Release x64 configuration in the comboboxes. 
9. Then, in the tree view, find the Linker category. Expand it by clicking the right arrow. 
10. Focus in the subsystems, and tab to subsystem feelds and select Windows subsystem.
11. Focus on the Input item. 
12. Finally, in the Additional dependencies field, write the following: "$(CoreLibraryDependencies);%(AdditionalDependencies);bass.lib;nvdaControllerClient64.lib"
13. Click apply and OK to save changes.
You have successfully setted up your game project.
Note. You can see the example game project in this repository.
