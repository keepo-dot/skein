# Skein

A native Linux desktop application for creating and editing knitting patterns. Built with C and GTK4, this tool provides a vector-based grid interface for designing colorwork and texture charts with automatic contrast adjustment and infinite canvas navigation.

## Features

* **Vector-Based Rendering:** Patterns are rendered using Cairo for crisp scaling at any zoom level.
* **Dual-Layer Editing:** Support for distinct "Yarn Color" (background) and "Stitch Type" (foreground symbol) layers.
* **Dynamic Contrast:** Stitch symbols automatically adjust between black and white based on the luminance of the underlying yarn color.
* **Navigation:** Pan and zoom support for navigating large pattern grids (currently defaulted to 1000x1000).
* **Tools & Modes:**
  * **Move:** Pan the camera view.
  * **Paint:** Apply yarn colors to the grid.
  * **Stitch:** Apply technical stitch symbols (Knit, Purl, YO, K2tog, SSK).

## Dependencies

To build this project, you will need a C compiler, Make, and the GTK4 development libraries.

* **GTK4**
* **GLib**
* **Cairo**
* **Make**

On Arch Linux:

```bash
sudo pacman -S gtk4 base-devel
git clone http://www.github.com/keepo-dot/skein
cd skein
```

On Debian/Ubuntu:

```bash
sudo apt install libgtk-4-dev build-essential
git clone http://www.github.com/keepo-dot/skein
cd skein
```

## Building

A `Makefile` is included for easy compilation. Simply run:

```bash
make
```

To clean up build artifacts:

```bash
make clean
```

## Usage

1. **Launch the application:**

    ```bash
    ./skein
    ```

2. **Navigation:** Select the **Move** tool to click and drag the canvas.
3. **Painting:** Select the **Paint** tool and choose a color from the palette to draw colorwork.
4. **Symbols:** Select the **Stitch** tool (Grid icon) to overlay technical symbols. The palette will automatically switch to show available stitch types.

## Architecture

The application follows a pragmatic C-style Model-View-Controller pattern:

* **AppState:** The central source of truth, holding pointers to the Pattern Data and UI State.
* **PatternData:** A flat array of `StitchData` structs representing the grid. This allows for O(1) access times regardless of grid size.
* **Rendering:**
  * `draw_grid`: Handles the viewport, culling, and background color rendering.
  * `draw_stitch_swatch`: A geometry-only helper that renders stitch symbols. It is decoupled from color logic to support reusability in toolbar icons.
* **Event Handling:** Input is processed via GTK Gesture controllers (`GtkGestureClick`, `GtkGestureDrag`) which modify the state and queue redraws.

## Roadmap

* **Tool Implementation:**
  * Eraser tool (remove properties from cells).
  * Color Picker tool (sample properties from grid).
* **Startup Flow:** Welcome screen to create new patterns or load existing files on launch.
* **Keyboard Shortcuts:** (Spacebar to pan, hotkeys for tools).
* File I/O (Save/Load functionality).
* Export to PDF/Image.
* Adjustable grid dimensions.
* Undo/Redo stack.
* Bundle OSS icon theme.

## License

**Skein** is licensed under the GNU General Public License v3.0 (GPLv3).
See the [LICENSE](LICENSE) file for details.

This application utilizes the GTK4 toolkit and the Adwaita Icon Theme, which are properties of the GNOME Project.
