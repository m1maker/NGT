# NGT: (New Game Toolkit)

Welcome to NGT! An open source game engine using [AngelScript](https://www.angelcode.com/angelscript/) as its internal scripting language.

Currently, NGT is positioned as a game engine for users with visual impairments, but we should think about the possibility of making it possible to create visual games.

## Features

- **Support for Multiple Screen Readers:** Accessibility is key for users with visual impairments, allowing user to receive speech messages with any screen reader.
- **3D Soundsystem:** Use 3D audio positioning, reverbs, lowpass, highpass and notch filters to achieve realistic sound in games.
- **File and Filesystem:** Use this to work with the user progress (For example, to save a game).
- **Encryption:** Use the AES256 algorithm to protect user passwords, game resources, etc.
- **Networking system:** Create online games.


# Building and Contribution

The following sections will help you build the engine from source or contribute to the engine's development.

## Installation of Visual Studio Community 2022

Download from [here](https://visualstudio.microsoft.com/vs/)

1. Run the exe file.
   - You may need to click "Yes" to run in admin mode.
2. In the license dialog, click "Continue".
   - You may need to wait some time upon clicking on it.
3. In the window, check the checkbox "Desktop development with C++".
4. In the installation mode dropdown, choose the option which is best for you.
   - Choose "Install while downloading" if you have a stable connection, or choose "Download all, then install" if you have an unstable or slow connection.
5. Click on "Install" button.
   - Wait until the downloading and installation is complete.

Congratulations! You have now installed Visual Studio on your computer. You will now be able to build the engine from source.

## Compilation

To compile the source into the executable,

1. Run the NGTScript.sln file.
2. Press the "File" menu (Alt key).
3. Next, go right to the "Build" submenu.
4. Next, click "Config Manager".
5. In the platform section, choose "x64".
6. In the compile section, choose "Release".
7. Now, exit the file window, and press Ctrl + B key.

Congratulations! The result window will appear, and the engine will try to be compiled.

The compiled executable can be found in the x64 folder.

# Additional Resources

- [Official News Telegram Channel](https://t.me/newgametoolkit)
- [Website](https://ngtcode.dev/)
- [Documentation Repository](https://github.com/m1maker/ngt-docs)
