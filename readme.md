<img src="rsc/icons/pngs/wifi-cli_64_lossy.png" align="left">

# Windows Wi-Fi CLI utility
CLI utility that centralises and streamlines Wi-Fi device interaction on Windows through a variety of simple and
automatable commands.

**This software is a "portfolio demo" since the `netsh wlan` command handles most win32 functions !**<br>
**It is safe to use, but there is no reason to use it instead of official *MS* tools.** 

## Features
* Interface management<br>
  * Listing of associated data in human or machine-readable formats<br>
    * Info (Name, GUID, status)<br>
    * ~~Properties (Background scanning toggle, RSSI, Chanel Number, ...)~~<br>
  * ~~Modification of previously mentioned data where applicable~~<br>
* Profile management<br>
  * Listing of info in human or machine-readable formats<br>
  * Deletion<br>
    * Per profile on a single interface<br>
    * All profiles per interface<br>
    * All profiles on all interfaces
* Connection management
  * Similar to `netsh wlan` commands

## Usage
TODO: Redo this section

<details open>
<summary><b>Main usage</b>&nbsp;&nbsp;&nbsp;&nbsp;<sub><sup>(Click to open/close)</sup></sub></summary>
123
</details>


### Interface listing
```
Usage:
  wifi ifaces list [-h|--help] [-a|--show-all] [-i|--show-index] [-g|--show-guid] [-d|--show-description]
                   [-s|--show-state] [-S|--show-state-text] [-D|--delimiter <DELIMITER>]

Options:
  -h, help         Shows this help text and exit.
  -a, --show-all          Shows all the possible fields.  (Same as -igds)
  -d, --show-description  Shows the interface's description.
  -g, --show-guid         Shows the interface's GUID.
  -i, --show-index        Shows the interface's index during the listing.
  -s, --show-state        Shows the interface's state.
  -S, --show-state-text   Shows the interface's state in a readable format.
  -D <DELIMITER>, --delimiter <DELIMITER>  Uses the given text as the delimiter in listings.
                                             (Defaults: " - " for generic listings, ";" for selective ones)

Remarks:
  * If no options, expect for the delimiter is given, the output will have the following format:
      `{guid}{delimiter}{description}`
  * The other fields will always appear in the following order with the delimiter in between:
      index, guid, description, state
  * If both '-s' and '-S' are used, '-s' will take precedence over '-S' and the state will be shown as a number.
```

## Building
Please refer to [building.md](building.md) for complete instruction on how to compile this project.

## Technical remarks
While running, the program sets the console's code page to `65001` and restores the original one when exiting cleanly.<br>
This is done in order to allow SSIDs or interface profiles with emojis to be shown properly.

This helps modern console prompts such as [ConEmu](https://conemu.github.io/) and [Windows Terminal](https://aka.ms/terminal).<br>
However, the old command prompts opened by `cmd.exe` and `ps.exe` do not support them and will just display the `?` character.<br>
If you still need to access something with such a character, you can use their index.

If you encounter any issue with this feature, you can disable it by uncommenting the line near the end of [CMakeLists.txt](CMakeLists.txt)
that sets the `NP_WIFI_NO_UTF8_CODE_PAGE` definition.

## License
This repo and all its dependencies are licensed under the [MIT License](LICENSE).
