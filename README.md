To build this project make a "build" and "extern" folder
In /extern run:
git clone git@github.com:wxWidgets/wxWidgets.git --recursive
git clone git@github.com:curl/curl.git --recursive
git clone git@github.com:nlohmann/json.git --recursive

then to build run:
cmake -B build -G "Ninja"
cmake --build build

then if you wish to install it into your bin folder (works on Linux Mint and similar only) to run it from anywhere using the terminal run:
sudo cmake --install build