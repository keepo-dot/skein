# Skein Long-Term Roadmap

## Phase 1: Foundation

### Core Editing & Navigation

- [x] **Create a new pattern**
  - Starts with a default grid size (e.g., 20x20 or user-selectable).
- [x] **Paint stitch colors**
  - Left-click to paint a cell with the current color.
- [x] **Paint stitch symbols**
  - Toggle between “color only” and “symbol” mode.
- [x] **Move the grid / pan the view**
  - Click-and-drag panning.
- [ ] **Zoom Canvas**
  - Zoom in/out to handle larger patterns or detailed work.
- [ ] **Resize Grid**
  - Change dimensions of an active pattern (add/remove rows or cols) without clearing it.

### Basic Toolset

- [x] **Active Tools UI**
  - Visible selection for Brush, Eraser, Picker, and Palette.
- [ ] **Eraser tool**
  - Clear a cell back to “empty” (handling transparent/no-stitch state).
- [ ] **Color picker**
  - Click a cell to set the current active color.
- [ ] **Flood Fill (Paint Bucket)**
  - Fill an enclosed area with a selected color/stitch.

### File I/O
- [x] **New pattern**
  - Adjust grid size from user input, reset canvas, draw new grid. *(Only draws transparent grid, need to update draw_grid)*
- [x] **Save pattern (Native)** (implementation created)
  - Serialize grid, dimensions, and palette to JSON.
- [x] **Load pattern** (implementation created)
  - Restore grid size, colors, and symbols from file.
- [ ] **Visual Feedback on Save**
  - UI notification ("File Saved") so the user isn't guessing.
- [ ] **Basic Error Handling**
  - Prevent crashes on corrupt files; show error dialogs.

---

## Phase 2: Quality of Life

### Editing Workflow

- [ ] **Undo / Redo**
  - The single most critical feature for user retention.
- [ ] **Selection Tool**
  - Select a rectangle of cells to Move, Copy, or Delete.
- [ ] **Copy / Paste**
  - Internal clipboard to duplicate motifs.
- [ ] **Keyboard Shortcuts**
  - `B` (Brush), `E` (Eraser), `Space` (Pan), `Ctrl+Z` (Undo).

### Pattern Management

- [ ] **Export to Image**
  - Save the grid as a `.PNG` or `.JPG` for sharing.
- [ ] **Bundled Demo File**
  - A "Welcome" pattern so new users immediately see what the app can do.
- [ ] **Recent Files Menu**
  - Quickly access the last 5 opened patterns.

---

## Phase 3: Knitting Specific Features

### Pattern Logic

- [ ] **Written Instruction Generation**
  - Automatically generate text ("Row 1: K2, P2, K2...") from the grid state.
- [ ] **Repeats Support**
  - Mark a section of the grid as a "Repeat" (boxed outline) for the text generator.
- [ ] **"Wrong Side" Logic**
  - Toggle between "Chart View" (always looking from front) and "Flat View" (showing Purls as they are worked on the wrong side).
- [ ] **Gauge Calculator**
  - Input "Stitches per inch" to see real-world dimensions of the pattern.

### Stitch Library

- [ ] **Custom Stitch Dictionary**
  - Allow users to define their own symbols/colors and name them (e.g., define a cable symbol).
- [ ] **Legend Generation**
  - Auto-generate a key (Symbol = "Knit 2 Together") to display alongside the pattern.

### Advanced Grid Tools

- [ ] **Grid Highlighting (Ruler)**
  - Highlight the active row to help users follow along while knitting.
- [ ] **Mirror / Flip**
  - Horizontally or vertically flip selected areas (essential for symmetrical motifs).
- [ ] **Magic Wand Select**
  - Select all connected stitches of the same color.

---

## Phase 4: Release Candidate

### UI / UX Deep Dive

- [ ] **Dark Mode / Theming**
  - Respect system theme preferences.
- [ ] **Tracing Layer**
  - Load a background image (translucent) behind the grid to trace a photo into a pattern.
- [ ] **Print Layout**
  - A dedicated "Print" dialog that formats the grid and legend onto A4/Letter paper.
- [ ] **Palette Management**
  - Save/Load custom color palettes (e.g., "Fall Colors").
  - Import palettes from images.

### Architecture & Distribution

- [ ] **Plugin System (Lua or Python)**
  - Allow community scripts (e.g., "Generate Checkerboard").
- [ ] **Cross-Platform Packaging**
  - Automated builds for Windows (.exe) and macOS (.dmg).

---

## Phase 5: Cloud & Community

### Integration

- [ ] **Ravelry Integration**
  - Import patterns directly from Ravelry (if API allows).
- [ ] **Cloud Sync**
  - Save patterns to Google Drive / Dropbox.
- [ ] **Pattern Browser**
  - A visual "start screen" showing thumbnails of all your local patterns.

### Advanced Algorithmic Tools

- [ ] **Fair Isle Checker**
  - Warning tool that highlights floats (gaps between colors) that are too long for practical knitting.
