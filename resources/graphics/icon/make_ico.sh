#!/bin/sh

# https://learn.microsoft.com/en-us/windows/apps/design/style/iconography/app-icon-construction

# Apps should have, at the bare minimum: 16x16, 24x24, 32x32, 48x48, and 256x256. This covers the most common icon sizes, and by providing a 256px icon, ensures Windows should only ever scale your icon down, never up.

magick icon_16.png icon_24.png icon_32.png icon_48.png icon_256.png icon.ico

