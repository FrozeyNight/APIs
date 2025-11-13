cmake -B build -G "Ninja"
cmake --build build
cmake --install build --prefix "%LOCALAPPDATA%\Programs\callAPI"

:: Permanently add to PATH (for future sessions)
setx PATH "%PATH%;%LOCALAPPDATA%\Programs\callAPI\bin"

:: Temporarily add to current PATH (for this session)
set "PATH=%PATH%;%LOCALAPPDATA%\Programs\callAPI\bin"