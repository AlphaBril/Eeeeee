# eeeeee

![GitHub last commit](https://img.shields.io/github/last-commit/AlphaBril/eeeeee?style=for-the-badge&labelColor=101418&color=9ccbfb)
![GitHub Repo stars](https://img.shields.io/github/stars/AlphaBril/eeeeee?style=for-the-badge&labelColor=101418&color=b9c8da)
![GitHub repo size](https://img.shields.io/github/repo-size/AlphaBril/eeeeee?style=for-the-badge&labelColor=101418&color=d3bfe6)
[![Ko-Fi donate](https://img.shields.io/badge/donate-kofi?style=for-the-badge&logo=ko-fi&logoColor=ffffff&label=ko-fi&labelColor=101418&color=f16061&link=https%3A%2F%2Fko-fi.com%2Fsoramane)](https://ko-fi.com/soramane)

---

## What is this sorcery?

**eeeeee** is a GTK4-powered popup for accent characters.
Hold down a vowel, and instead of nothing happening (👀 looking at you, QWERTY), a popup will appear letting you quickly pick accented characters — via keyboard shortcuts or mouse clicks.

Think of it as giving your keyboard the power of *French textbooks, Spanish novels, and German umlauts*, all without leaving your flow.

---

## ✨ Features

- Hold vowel → see accents → pick what you want → profit.
- Works with both **keyboard shortcuts** and **mouse clicks**.
- Fully configurable: choose your keyboard + tweak detection speed.
- Minimal deps: **GTK4**, **libevdev**, **GIO**.
- Runs natively, no bloat, just fast C.

---

## 📸 Screenshots / Demos

_(Replace with real ones later)_

**Popup in action:**
![screenshot placeholder](https://via.placeholder.com/600x300.png?text=Popup+with+accents)

**Config example:**
![screenshot placeholder](https://via.placeholder.com/600x300.png?text=Config+menu)

---

## ⚡ Installation

### 🔹 Nix (flake)

If you’re on **NixOS** or use flakes, you’re covered:

```bash
nix run github:AlphaBril/eeeeee
````

Or enter a dev shell:

```bash
nix develop github:AlphaBril/eeeeee
```

---

### 🔹 Arch Linux (AUR)

🚧 **Coming soon!**
Once the PKGBUILD lands, you’ll be able to:

```bash
yay -S eeeeee
```

---

### 🔹 Debian / Ubuntu (pkg)

🚧 **Coming soon!**
Will eventually be installable via:

```bash
sudo apt install eeeeee
```

---

## 🔧 Manual Build

If you don’t want to wait for packages, build it yourself:

```bash
git clone https://github.com/AlphaBril/eeeeee.git
cd eeeeee
meson setup build
ninja -C build
sudo ninja -C build install
```

### Dependencies

You’ll need:

* `gtk4`
* `glib`
* `libevdev`
* `gio-2.0`
* `meson`
* `ninja`
* `gcc` / `pkg-config`

(If you’re on Nix, just use the provided `flake.nix` or `shell.nix`.)

---

## 🔑 Permissions

Because eeeeee listens directly to your keyboard, you need to give it access:

* Make sure your user is in the **`input`** group:

  ```bash
  sudo usermod -aG input $USER
  ```
* Ensure `/dev/uinput` is accessible by `input`:

  ```bash
  ls -l /dev/uinput
  # should look like:
  # crw-rw---- 1 root input 10, 223 Sep 25 21:47 /dev/uinput
  ```

Logout/login (or reboot) after adding yourself to the group.

---

## 🚀 Usage

1. Run setup once to pick your keyboard:

   ```bash
   eeeeee --setup
   ```

2. Config file lives at:

   ```
   ~/.config/eeeeee/config
   ```

   You can edit:

   * **Keyboard device**
   * **Detection delay** (default = 200ms)

3. Start typing, hold down a vowel, and enjoy your éèêë dreams.

---

## 💡 Roadmap

* [ ] AUR package
* [ ] Debian/Ubuntu package
* [ ] More customization (themes? fancy animations?)
* [ ] Add support for consonant accents (ñ, ç, etc.)

---

## 🖤 Support

If you find **eeeeee** useful, consider [buying me a coffee](https://ko-fi.com/alphabril) ☕