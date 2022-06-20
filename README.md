# freetype_textdraw demo program
freetype_textdraw is demo program developed to learn how to use library Freetype to draw any text. 

## Features
Draw some text with customizations:
* select font file to draw
* enable/disable kerning
* enabled/disable ligatures
* enable/disable text antialiasing
* enable/disable hinting
* change font size
* change gamma of antialiasing
* change text or background color

Please note that library HarfBuzz is required to implement ligatures and kerning.
Also for selected font you can see for which languages there is a full set of glyphs.
<br/>
The most important: you can study sources of this project to write your own.

### LICENSE
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

### Dependencies
cmake, Freetype, HarfBuzz (optional), Qt5.

### Build Instructions
Follow standard cmake build instructions:
* make build directory:
```bash
    # mkdir build
    # cd build
```
* call cmake to configure:
```bash
    # cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_HARFBUZZ=ON ..
```
To omit HarfBuzz support replace "`-DUSE_HARFBUZZ=ON`" with "`-DUSE_HARFBUZZ=OFF`", but you lost ligatures and kerning features in modern fonts!
* call make to build:
```bash
    # make
```

### Screenshots
* Main window
![Main window](docs/mainwindow.png)
* Antialiasing
  - Without antialiasing:
![antialiasing-none](docs/antialias-none.png)
  - With antialiasing:
![antialiasing-gray](docs/antialias-gray.png)
* Kerning
  - Kerning disabled:
![kerning-off](docs/kerning-off.png)
  - Kerning enabled:
![kerning-on](docs/kerning-on.png)
* Ligatures
  - Ligatures disabled:
![ligatures-off](docs/ligatures-off.png)
  - Ligatures enabled:
![ligatures-on](docs/ligatures-on.png)
