# Skein

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/keepo-dot/skein/actions)
![Status](https://img.shields.io/badge/status-active-brightgreen.svg)
![GitHub last commit](https://img.shields.io/github/last-commit/keepo-dot/skein)

![C](https://img.shields.io/badge/C-00599C?style=flat-square&logo=c&logoColor=white)
![GTK4](https://img.shields.io/badge/UI-GTK4-orange.svg)
[![Clang Format](https://img.shields.io/badge/code%20style-clang-blue.svg)](https://clang.llvm.org/docs/ClangFormat.html)

![GitHub repo size](https://img.shields.io/github/repo-size/keepo-dot/skein)
![GitHub Repo stars](https://img.shields.io/github/stars/keepo-dot/skein?style=social)
![GitHub forks](https://img.shields.io/github/forks/keepo-dot/skein?style=social)

[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](https://github.com/keepo-dot/skein/pulls)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://github.com/keepo-dot/skein/blob/main/LICENSE)

A lightweight, native Linux desktop application for designing and standardizing knitting charts, built entirely in C with GTK4.

This project was built to showcase manual memory management, modular C architecture, and native desktop GUI development, while providing a highly functional, vector-based grid interface for designing colorwork and texture charts.

## Features

### Core Capabilities

* **Vector-Based Rendering:** Patterns are rendered using Cairo for crisp, lossless scaling at any zoom level. No pixelation, no matter how large the chart gets.
* **Dual-Layer Editing:** Support for distinct "Yarn Color" (background layer) and "Stitch Type" (foreground symbol layer) to accurately represent complex knitting techniques.
* **Smart Contrast:** Stitch symbols automatically calculate the luminance of the underlying yarn color and dynamically switch between black and white to ensure maximum readability.
* **Infinite Navigation:** `Ctrl + Scroll` to zoom and click-and-drag panning support for navigating massive, multi-page pattern grids.

### Tools & Modes

* **Move (Arrow):** Pan the camera view effortlessly across the canvas.
* **Paint (Brush):** Apply yarn colors to the grid.
* **Erase (Eraser):** Remove colors and symbols via single click or click-and-drag.
* **Picker (Eyedropper):** Sample colors directly from the grid (intelligently ignores empty/transparent space).
* **Stitch (Grid):** Overlay technical knitting symbols (Knit, Purl, Yarn Over, K2tog, SSK, M1, Slip, etc.). The palette automatically adapts to show available technical stitches when this tool is active.

### Data & Safety

* **File Persistence:** Save and load your work to a custom JSON format (`.skn`) via `json-glib`, using modern asynchronous native GTK file dialogs.
* **Visual Feedback & Sanity Checks:** Integrated alert dialogs confirm successful save and load operations. The app proactively warns you if you attempt to save an empty canvas to prevent accidental file overwriting.
* **Memory Safe:** Thoroughly tested with AddressSanitizer (ASan) to ensure zero memory leaks during intensive file I/O, canvas manipulation, and JSON serialization.

## Dependencies

To build this project, you will need a C compiler (Clang or GCC), Make, and the GTK4 + JSON-GLib development libraries.

* **GTK4**
* **JSON-GLib-1.0**
* **Cairo** (usually bundled with GTK4)
* **Make**

### Installation (Arch Linux)

```bash
sudo pacman -S gtk4 json-glib base-devel
git clone https://github.com/keepo-dot/skein
cd skein
```

### Installation (Debian/Ubuntu)

```bash
sudo apt update
sudo apt install libgtk-4-dev libjson-glib-dev build-essential
git clone https://github.com/keepo-dot/skein
cd skein
```

## Building

The project uses a standard, modular C build process via a `Makefile`.

To compile the executable into the `bin/` directory:

```bash
make
```

To clean up object files and binaries for a fresh build:

```bash
make clean
```

*Note for Developers: You can uncomment the `SANITIZER_FLAGS` in the Makefile to build with AddressSanitizer and UndefinedBehaviorSanitizer for debugging.*

## Usage

1. **Launch the application:** Run `./bin/skein` from your terminal. You will be greeted by a custom-generated startup logo confirming the canvas is ready.
2. **File Operations:** Use the file icons in the toolbar to Create New (with custom width/height grid dimensions), Save, or Load `.skn` patterns.
3. **Navigation:** Select the 'Move' tool (Arrow icon) to click and drag the canvas. Use `Ctrl + Scroll` to zoom in for detailed work or zoom out for a macro view.
4. **Painting:** Select the 'Paint' tool (Brush icon) and choose a color from the palette to draw colorwork motifs.
5. **Editing:** Use the 'Eraser' to remove mistakes, or the 'Picker' to quickly switch your active color to one already on the board.
6. **Symbols:** Select the 'Stitch' tool (Grid icon) to overlay technical symbols. The palette will automatically switch to show available stitch types.

## Architecture

Skein utilizes a pragmatic, highly modular C architecture designed for separation of concerns, readability, and maintainability:

* **include/ (`.h` files):** Contains all headers mapping the project's data structures (`types.h`) and public function signatures. `AppState` tracks the main window for global access by asynchronous dialogs.
* **src/skein.c & src/skein_window.c:** The application entry points. Initializes the `GtkApplication`, manages the main window shell, layout packing, and memory allocation.
* **src/canvas.c:** High-performance grid rendering. Implements viewport culling (only drawing what is visible on screen) and translates user input into Paint, Erase, and Picker actions.
* **src/toolbar.c:** Handles the creation of the tool sidebar, button states, and triggers UI interactions.
* **src/utils.c:** The heavy-lifting utility module. Manages asynchronous JSON File I/O, serialization/deserialization, UI dialog generation, empty-canvas safety validations, and dynamic vector rendering.
* **src/resources.c:** Stores static configurations, including arrays for the toolbar buttons, global color palettes, and stitch symbol definitions.

## Roadmap

Skein is actively being developed with a focus on becoming a complete pattern-drafting suite. Upcoming features include:

**Quality of Life & Editing Workflow:**

* Undo/Redo stack (Highest priority).
* Selection Tool and internal Copy/Paste for duplicating motifs.
* Export grid to Image (`.PNG`/`.JPG`) and PDF Print Layouts.
* Keyboard shortcuts (e.g., `B` for Brush, `Spacebar` to pan, `Ctrl+Z` to undo).

**Knitting-Specific Advanced Features:**

* **Written Instruction Generation:** Automatically generate standardized knitting text (e.g., "Row 1: K2, P2, K2...") directly from the visual grid state.
* **"Wrong Side" Logic:** Toggle between "Chart View" (always looking from the front) and "Flat View" (showing purls as they are actually worked on the wrong side).
* **Fair Isle Checker:** An algorithmic warning tool that highlights color floats that are too long for practical knitting.
* **Gauge Calculator & Repeats:** Input stitches-per-inch to see real-world dimensions and mark bounding boxes for repeating sections.

**Community & Cloud:**

* Custom Stitch Dictionary (allow users to define and draw their own symbols, like complex cables).
* Save/Load custom color palettes.
* Ravelry API integration for pattern fetching.

*See [ROADMAP.md](/ROADMAP.md) for the full, detailed feature pipeline.*

## Contributing

As this is a soft-release to the open-source community, pull requests, issue reports, and feedback are highly encouraged! Whether it's adding a new technical stitch drawing to `utils.c` or helping implement the Undo/Redo stack, contributions are welcome.

## License

**Skein** is licensed under the GNU General Public License v3.0 (GPLv3).
See the [LICENSE](/LICENSE) file for full details.

This application utilizes the GTK4 toolkit and the Adwaita Icon Theme, which are properties of the GNOME Project.
