# Skein

A native Linux desktop application for creating and editing knitting patterns. Built with C and GTK4, this tool provides a vector-based grid interface for designing colorwork and texture charts with automatic contrast adjustment and infinite canvas navigation.

## Features

* **Vector-Based Rendering:** Patterns are rendered using Cairo for crisp scaling at any zoom level.
* **Dual-Layer Editing:** Support for distinct "Yarn Color" (background) and "Stitch Type" (foreground symbol) layers.
* **Dynamic Contrast:** Stitch symbols automatically adjust between black and white based on the luminance of the underlying yarn color.
* **Navigation:** Pan and zoom support for navigating large pattern grids (defaulted to 450x600).
* **Tools & Modes:**
  * **Move:** Pan the camera view.
  * **Paint:** Apply yarn colors to the grid.
  * **Stitch:** Apply technical stitch symbols (Knit, Purl, YO, K2tog, SSK).

## Dependencies

To build this project, you will need a C compiler (Clang/GCC), Make, and the GTK4 development libraries.

* **GTK4**
* **GLib**
* **Cairo**
* **Make**

### Installation (Arch Linux)
```
    sudo pacman -S gtk4 base-devel
    git clone https://github.com/keepo-dot/skein
    cd skein
```

### Installation (Debian/Ubuntu)
```
    sudo apt install libgtk-4-dev build-essential
    git clone https://github.com/keepo-dot/skein
    cd skein
```
## Building

The project uses a modular build system. To compile:
```
    make
```
To clean up object files and binaries:
```
    make clean
```

## Usage

1. **Launch the application:**
```
./bin/skein
```
1. **Navigation:** Select the 'Move' tool to click and drag the canvas.
2. **Painting:** Select the 'Paint' tool and choose a color from the palette to draw colorwork.
3. **Symbols:** Select the 'Stitch' tool (Grid icon) to overlay technical symbols. The palette will automatically switch to show available stitch types.

## Architecture

Skein utilizes a pragmatic, modular C architecture designed for high performance and low header overhead:

* **include/types.h**: The central source of truth, containing all structs (AppState, StitchData) and enums.
* **src/skein.c**: Application entry point; handles GtkApplication lifecycle and state initialization.
* **src/canvas.c**: High-performance grid rendering. Implements viewport culling to ensure smooth interaction on 1,000,000-cell grids.
* **src/toolbar.c**: Dynamic UI generation for the tool sidebar, color swatches, and stitch selection.
* **src/skein_window.c**: Manages the main window shell and layout packing.
* **src/utils.c**: Shared utilities, including the global yarn color palette and button factory logic.

## Roadmap

* **Tool Implementation:**
  * Eraser tool (remove properties from cells).
  * Color Picker tool (sample properties from grid).
* **Core Functionality:**
  * File I/O (Save/Load functionality).
  * Export to PDF/Image.
  * Adjustable grid dimensions.
  * Undo/Redo stack.
* **UI/UX:**
  * Welcome screen for pattern creation/loading.
  * Keyboard shortcuts (Spacebar to pan, Alt+S for Stitch Type, etc).
  * Bundled OSS icon theme.
* **Platform Support:**
  * Windows Release (likely via MSYS2).

## License

**Skein** is licensed under the GNU General Public License v3.0 (GPLv3).
See the [LICENSE](LICENSE) file for details.

This application utilizes the GTK4 toolkit and the Adwaita Icon Theme, which are properties of the GNOME Project.
