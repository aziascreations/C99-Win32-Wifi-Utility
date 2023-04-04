# Windows Wi-Fi CLI utility
CLI utility that streamlines Wi-Fi device interaction on Windows through a variety of simple automatable commands.

## Usage

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

## License
This repo and all its dependencies are licensed under the [MIT License](LICENSE).
