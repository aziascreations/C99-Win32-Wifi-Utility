# Building Windows Wi-Fi CLI utility

## Cloning
Firstly, you need to clone the repo:
```bash
git clone https://github.com/aziascreations/C99-Win32-Wifi-Utility.git
```

And secondly, you need to get the git submodule that contain the launch arguments parsing library:
```bash
cd C99-Win32-Wifi-Utility
git submodule update --init
```

## Compiling
In order to compile this application, you need the [Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/)
as well as [CMake 3.16+](https://cmake.org/) and [Visual Studio](https://visualstudio.microsoft.com/)'s compiler and linker.<br>
Please note that [MinGW]() won't work since it errors out when trying to compile the [.rc files](rsc/rc) for some reason.

Once ready, you need to open the [Visual Studio Developer Command Prompt](https://learn.microsoft.com/en-us/visualstudio/ide/reference/command-prompt-powershell?view=vs-2022).

Afterward, make a new directory and enter it:
```bash
mkdir build
cd build
```

Now, you need to prepare the CMake project:
```bash
cmake ..
```

If you configured your toolchain properly, you should see the following line early on in CMake's output:
```
The C compiler identification is MSVC x.x.x.x
```

Finally, you can run one of the following commands depending on your desired build type:
```bash
cmake --build . --config Debug
cmake --build . --config Release
cmake --build . --config RelWithDebInfo
cmake --build . --config MinSizeRel
```

Now your executables should be located in one of the following folders:
* [build/Debug](build/Debug)
* [build/Release](build/Release)
* [build/RelWithDebInfo](build/RelWithDebInfo)
* [build/MinSizeRel](build/MinSizeRel)

## Updating repository
If you need to update the repository, you can just run the following commands:
```bash
git pull
git submodule foreach git pull
```
