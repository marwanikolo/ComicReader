# ComicReader

A blazing-fast, lightweight, and Wayland-native comic/image reader built from scratch in C++ and Qt6. Designed specifically for Arch Linux and tiling window managers (like Hyprland), it focuses on speed, extreme memory efficiency, and keyboard-centric navigation.

## Key Features

* **Lazy-Loading Architecture:** Uses a custom `QStyledItemDelegate` to intercept draw calls, meaning thumbnails are only generated exactly when they become visible. Uses virtually zero RAM, even when opening directories with tens of thousands of images.
* **100% Wayland Safe:** Bypasses Qt's hidden background-thread directory scanners to guarantee no GPU/display server segmentation faults under Hyprland.
* **Yazi / Vim Keybindings:** Fully integrated `hjkl` navigation to jump seamlessly between the grid browser and the reading view without ever touching a mouse.
* **Dynamic Zoom:** Smooth `Ctrl + Mouse Wheel` zooming for high-density webtoons with tiny text.
* **Native Image Support:** Out-of-the-box support for `.jpg`, `.png`, and `.webp`.

## Dependencies (Arch Linux)

You will need the standard C++ build tools and the base Qt6 packages:

```bash
sudo pacman -S base-devel cmake qt6-base qt6-imageformats
```
*(Note: `qt6-imageformats` is explicitly required to decode `.webp` files).*

## Build Instructions

Clone the repository and compile using CMake:

```bash
git clone https://github.com/marwanikolo/ComicReader.git
cd ComicReader
mkdir build && cd build
cmake ..
make -j$(nproc)
```

**To Run:**
Point the executable to any folder containing your images or subfolders of images:

```bash
./ComicReader /path/to/your/comics
```

## Controls

**Browser View:**
* `j` / `k` : Move up and down the grid
* `h` / `Backspace` : Go up/out a directory level
* `l` / `Return` : Open a folder or start reading a comic
* `q` : Quit the application

**Reader View:**
* `l` / `Space` / `Right Arrow` : Next page
* `h` / `Left Arrow` : Previous page
* `Ctrl + Mouse Wheel` : Zoom in/out
* `Mouse Wheel` : Scroll down the page (or go to next page if zoomed out)
* `q` / `Escape` : Close the comic and return to the browser
