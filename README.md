# Skein

A native Linux desktop application for creating and editing knitting patterns. Built with C and GTK4, this tool provides a vector-based grid interface for designing colorwork and texture charts with automatic contrast adjustment and infinite canvas navigation.

## Features

* **Vector-Based Rendering:** Patterns are rendered using Cairo for crisp scaling at any zoom level.
* **Dual-Layer Editing:** Support for distinct "Yarn Color" (background) and "Stitch Type" (foreground symbol) layers.
* **Smart Contrast:** Stitch symbols automatically adjust between black and white based on the luminance of the underlying yarn color.
* **File Persistence:** Save and load your work to a custom JSON format (`.skn`).
* **Infinite Navigation:** Pan support for navigating large pattern grids.
* **Tools & Modes:**
  * **Move:** Pan the camera view.
  * **Paint:** Apply yarn colors to the grid.
  * **Erase:** Remove colors and symbols via click or drag.
  * **Picker:** Sample colors from the grid (ignores empty space).
  * **Stitch:** Apply technical stitch symbols (Knit, Purl, YO, K2tog, SSK, etc).

## Dependencies

To build this project, you will need a C compiler (Clang/GCC), Make, and the GTK4 + JSON-GLib development libraries.

* **GTK4**
* **JSON-GLib-1.0**
* **Cairo**
* **Make**

### Installation (Arch Linux)

```bash
sudo pacman -S gtk4 json-glib base-devel
git clone https://github.com/keepo-dot/skein
cd skein
```

### Installation (Debian/Ubuntu)

```bash
sudo apt install libgtk-4-dev libjson-glib-dev build-essential
git clone https://github.com/keepo-dot/skein
cd skein
```

## Building

The project uses a standard C build process. To compile the executable into the `bin/` directory:

```bash
make
```

To clean up object files and binaries:

```bash
make clean
```

## Usage

1. Launch the application:

```bash
./bin/skein
```

1. **File Operations:** Use the file icons in the toolbar to Create New (with custom dimensions), Save, or Load patterns.
2. **Navigation:** Select the 'Move' tool (Arrow icon) to click and drag the canvas.
3. **Painting:** Select the 'Paint' tool (Brush icon) and choose a color from the palette to draw colorwork.
4. **Editing:** Use the 'Eraser' to remove stitches, or the 'Picker' (Eyedropper) to sample an existing color from the grid.
5. **Symbols:** Select the 'Stitch' tool (Grid icon) to overlay technical symbols. The palette will automatically switch to show available stitch types.

## Architecture

Skein utilizes a pragmatic, modular C architecture designed for readability and maintainability:

* **include/types.h**: Defines application-wide structures (AppState, StitchData) and enumerations.
* **src/resources.c**: Stores static arrays for the toolbar buttons, color palette, and stitch symbols.
* **src/skein.c**: Entry point. Initializes the GtkApplication and memory allocation.
* **src/canvas.c**: High-performance grid rendering. Implements viewport culling and tool logic (Paint, Erase, Picker).
* **src/toolbar.c**: Handles the creation of the tool sidebar and manages JSON File I/O.
* **src/skein_window.c**: Manages the main window shell and layout packing.
* **src/utils.c**: Shared UI utilities, including vector symbol drawing.

## Roadmap

* **Core Functionality:**
  * Export to PDF/Image.
  * Undo/Redo stack.
* **UI/UX:**
  * Keyboard shortcuts (Spacebar to pan, Alt+S for Stitch Type, etc).
  * Bundled OSS icon theme.
* **Platform Support:**
  * Windows Release (likely via MSYS2).

See [ROADMAP.md](ROADMAP.md) for details.

## License

**Skein** is licensed under the GNU General Public License v3.0 (GPLv3).
See the [LICENSE](LICENSE) file for details.

This application utilizes the GTK4 toolkit and the Adwaita Icon Theme, which are properties of the GNOME Project.
