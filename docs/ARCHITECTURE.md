# Skein: Architecture & Technical Design

This document details the internal design, memory models, data structures, and subsystem interactions powering the Skein knitting chart editor.

---

## 1. High-Level Architectural Pattern

Skein is built in pure C11 using an explicit **Model-View-Controller (MVC)** architectural pattern to prevent domain logic from becoming coupled to GTK4 widget lifecycles or Cairo rendering calls.

    +-------------------------------------------------------------+
    |                     CONTROLLER LAYER                        |
    |         src/canvas_events.c  |  src/toolbar.c               |
    |  - Translates GTK gestures and event controllers            |
    |  - Maps screen coordinates to grid coordinate space         |
    |  - Dispatches domain mutations to the model                 |
    +------------------------------+------------------------------+
                                   |
                         calls mutation APIs
                                   |
                                   v
    +-------------------------------------------------------------+
    |                        MODEL LAYER                          |
    |          src/pattern.c  |  include/pattern_types.h          |
    |  - Owns grid cell memory, repeat definitions, history       |
    |  - Pure data manipulation; zero GTK or Cairo imports        |
    |  - Atomic transactional undo/redo engine                    |
    |  - Non-recursive stack-based flood fill                     |
    +------------------------------+------------------------------+
                                   |
                       signals redraw on change
                                   |
                                   v
    +-------------------------------------------------------------+
    |                         VIEW LAYER                          |
    |                    src/canvas_render.c                      |
    |  - Subscribes to GtkDrawingArea draw signals                |
    |  - Renders vector stitches, symbols, and overlays in Cairo  |
    |  - Performs aggressive viewport bounds culling              |
    +-------------------------------------------------------------+

Supporting services:
* **Serialization (`src/file_io.c`):** Translates between internal model structs and portable JSON-GLib structures asynchronously.
* **Application Shell (`src/skein.c`, `src/skein_window.c`):** Boots the `GtkApplication`, manages top-level layout packing, and owns the root `AppState`.

---

## 2. Repository Layout

    skein/
    ├── bin/                 # Compiled executable output (gitignored)
    ├── docs/                # Architecture and technical design documentation
    ├── include/             # Public API declarations and pure data structures
    │   ├── pattern.h        # Model manipulation APIs
    │   ├── pattern_types.h  # Core structs (PatternData, HistoryTable, StitchData)
    │   ├── ui_types.h       # Viewport, camera, tool mode, and UI state
    │   ├── canvas.h         # Canvas draw and event declarations
    │   ├── file_io.h        # JSON serialization signatures
    │   └── resources.h      # Palette configurations and icon definitions
    ├── obj/                 # Intermediate compiled object files (gitignored)
    ├── src/                 # C implementation files
    │   ├── canvas_events.c  # Event handlers, gestures, and input dispatch (Controller)
    │   ├── canvas_render.c  # Cairo grid, stitch, and repeat rendering (View)
    │   ├── file_io.c        # Asynchronous JSON-GLib serialization
    │   ├── pattern.c        # Grid data structures, mutations, undo/redo (Model)
    │   ├── skein.c          # App lifecycle and main entry point
    │   ├── skein_window.c   # GTK window assembly and widget tree
    │   ├── toolbar.c        # Sidebar buttons, palettes, and swatch handling
    │   └── utils.c          # Icon loaders, swatch renderers, and dialog helpers
    └── Makefile             # Formatted, silent build system

---

## 3. Data Structures & Memory Layout

### Contiguous Grid Storage
Pattern cells are stored in `PatternData` as a single, contiguous heap-allocated array of `StitchData`:

    index = (y * width) + x

Each `StitchData` instance packs both visual layers into a compact struct:

    typedef struct {
        PatternColor stitch_color;  /* Yarn background RGBA */
        int stitch_type;            /* Technical symbol ID (Knit, Purl, YO, etc.) */
    } StitchData;

This linear layout maximizes CPU cache locality during Cairo render passes and serialization sweeps compared to fragmented pointer-to-pointer (`StitchData**`) 2D arrays.

### Repeat Boundaries
Pattern repeats are managed by a dedicated dynamic table:

    typedef struct {
        int start_row;
        int end_row;
    } RepeatSection;

    typedef struct {
        size_t table_size;
        size_t num_repeats;
        RepeatSection *repeat_section;
    } RepeatTable;

All table lengths and active counts use unsigned `size_t` primitives to guarantee sign-safety against memory allocators on 64-bit platforms.

---

## 4. Key Subsystems

### Non-Recursive Stack Flood Fill
To avoid stack overflow vulnerabilities on large charts, `pattern_apply_fill` runs an iterative, depth-first traversal using an explicit heap-allocated coordinate stack:

1. **Stack Allocation:** A single `Coords*` buffer sized to the chart's total cell count ($W \times H$) is allocated upfront. Dynamic reallocation is never needed during the traversal.
2. **In-Place Mutation:** Stitches that match the target criteria are immediately updated to the replacement state upon being visited. This acts as an implicit "visited" set and prevents identical cells from being queued multiple times.
3. **Compound History Recording:** As each cell is modified, an entry is appended to a temporary `StitchDelta` buffer recording its index, `before_state`, and `after_state`.
4. **Buffer Right-Sizing:** Once the stack is depleted, the delta buffer is reallocated down to the exact number of modified cells and committed as a single `ActionGroup` in the history table.

### History & Undo/Redo Engine
Undo and redo are modeled as discrete transactions using an `ActionGroup` abstraction:

    typedef struct {
        size_t cell_num;
        StitchData before_state;
        StitchData after_state;
    } StitchDelta;

    typedef struct {
        ActionType type;       /* ACTION_STITCH_EDIT or ACTION_REPEAT_EDIT */
        size_t group_size;
        StitchDelta *action;
    } ActionGroup;

    typedef struct {
        size_t table_size;
        size_t current_position;
        size_t history_count;
        ActionGroup *group;
    } HistoryTable;

* **Future Truncation:** If a user makes a new edit while `current_position < history_count`, all existing action groups ahead of `current_position` are immediately freed to prevent branching tree corruption.
* **Atomic Rollback:** Reversing an operation simply iterates backward through `agroup->action[i]`, writing `before_state` directly to `pattern->stitch_data[cell_num]`. Redo applies `after_state` in forward order.

### Viewport Culling & Dynamic Glyphs
The Cairo rendering loop in `canvas_render.c` minimizes draw overhead:

1. **Frustum Culling:** The view transforms window bounds into grid coordinates using `camera_x`, `camera_y`, and `zoom`. Any cell falling outside the visible viewport is skipped entirely.
2. **Adaptive Contrast:** When drawing technical symbols over colored stitches, the perceived relative luminance $L$ of the underlying yarn color is calculated:

    L = 0.299 * R + 0.587 * G + 0.114 * B

   If $L > 0.5$, the symbol is drawn in near-black (`#111111`); otherwise, it is drawn in crisp white (`#FFFFFF`).

---

## 5. Build Pipeline & Quality Assurance

### Descriptive Makefile
Compilation is handled by a custom `Makefile` supporting quiet compilation, ANSI status tags (`[CC]`, `[LD]`, `[CLEAN]`), and directory dependency separation.

    make             # Compiles with -Wall -Wextra -g -Iinclude
    make clean       # Purges obj/ and bin/ directories

### Compiler Warnings
The project enforces a zero-warning policy across Clang and GCC:
* Explicit `(void)` casts on unused parameters in GTK/GLib signal callbacks.
* Strict `size_t` consistency across bounds comparisons to eliminate signed/unsigned comparison bugs (`-Wsign-compare`).
* Scoped deprecation silencers for transitionary GTK functions.

### Dynamic & Static Instrumentation
* **AddressSanitizer (ASan):** Can be enabled instantly by uncommenting `SANITIZER_FLAGS` in the `Makefile` to detect heap out-of-bounds, use-after-free, or memory leaks during runtime testing.
* **GitHub CodeQL:** Continuous static analysis scans every C source file using AST semantic extraction (`build-mode: none`) on every push to detect invalid pointer lifecycles and boundary flaws.