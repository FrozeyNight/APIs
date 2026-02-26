![image alt](https://github.com/FrozeyNight/APIs/blob/d687b8581889faccfb0bfa9b80b5a19830630c1c/myWeatherAppImagePreview.png)
![image alt](https://github.com/FrozeyNight/APIs/blob/d687b8581889faccfb0bfa9b80b5a19830630c1c/myWeatherWindowsPreview.png)

## About
myWeather is a simple and lightweight cross-platform GUI and Console weather app, which displays the chosen weather data for specified coordinates.

It is a project I made to learn more about C++, making GUIs in said language, using CMake and making cross-platform applications.

This project was done with the help of:
OttoBotCode's wxWidgets course: https://www.youtube.com/watch?v=BjQhp0eHmJw&list=PLFk1_lkqT8MbVOcwEppCPfjGOGhLvcf9G
The wxWidgets documentation: https://docs.wxwidgets.org/3.2/
Various forum posts on the wxWidgets Discussion Forum: https://forums.wxwidgets.org/
The Cherno's videos about the C++ build process: https://www.youtube.com/@TheCherno/featured
ChatGPT and Gemini (I did not use the chat bots to generate code for me, only to explain how some functions worked, explain warnings and build process errors, help choose the right libraries or functions and verify the compatibility and safety of the program)

## Platforms
myWeather supports both running and building the project on:
- Windows 11
- Most Unix variants using the GTK+ toolkit
## Lincense
This project uses the GNU General Public License version 3.
## Building
1. CMake version 3.16+ and C++ standard 20+ are required to build the project.
2. Make a "build" and "extern" folder in the project's root directory
	In /extern clone the wxWidgets, curl and nlohmann json libraries, for example by running:
	
	git clone git@github.com:wxWidgets/wxWidgets.git --recursive
	
	git clone git@github.com:curl/curl.git --recursive
	
	git clone git@github.com:nlohmann/json.git --recursive
	
	Note that the --recursive flag is required for the build.
3. To build run CMake with your generator of choice, in this example Ninja:

	cmake -B build -G "Ninja"
	
	cmake --build build
	Note that this process might take over 10 minutes depending on your computer's processing power
4. If you wish to make the Appimage version on your Unix system: 
	you must additionally add the linuxdeploy-x86_64.Appimage and the linuxdeploy gtk plugin to the project's root directory. These can be found on their respective github websites:
	
	linuxdeploy: https://github.com/linuxdeploy/linuxdeploy/releases
	linuxdeploy gtk plugin (linuxdeploy-plugin-gtk.sh file): https://github.com/linuxdeploy/linuxdeploy-plugin-gtk

	Then run "cmake --build build --target appimage"

	Note that this feature is only available on most Unix variants and does not work on Windows.

