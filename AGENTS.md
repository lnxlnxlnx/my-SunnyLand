# SunnyLand — AGENTS.md

## Build & Run

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -- -j$(nproc)
./SunnyLand-Linux           # binary output to project root
```

- **C++20**, SDL3, glm, nlohmann_json, spdlog — all found via CMake presets
- New `.cpp` files **must be manually added** to `CMakeLists.txt` (no `GLOB`)
- Run from project root at runtime — assets are relative to CWD (e.g. `assets/config.json`, `assets/textures/...`)
- Build dir is `build/` (gitignored)

## Architecture

- Entrypoint: `src/main.cpp` → `engine::core::GameApp`
- Subsystem init order: `Config → SDL → Time → ResourceManager → Renderer → Camera → InputManager`
- Main loop: `time_manager_->update()` → `input_manager_->update()` → `handleEvents()` → `update(dt)` → `render()`
- Namespace convention: `engine::<subsystem>::ClassName`
- All classes **delete copy/move** constructors/operators (Rule of 5)
- Sprite is a data-only class (texture_id, optional source_rect, flip flag); Renderer does the actual drawing
- ResourceManager is a Facade over TextureManager/AudioManager/FontManager

## Input Quirks

- `input_manager_->update()` called in `GameApp::run()` directly, **not** inside `GameApp::update()`
- Action state machine: `INACTIVE → PRESSED_THIS_FRAME → HELD_DOWN → RELEASED_THIS_FRAME`
- Input mappings configured in `assets/config.json` keyed by action name → key name list
- Config supports key names (`"A"`, `"Space"`) and mouse buttons (`"MouseLeft"`, `"MouseRight"`)

## Config

- Loaded from `assets/config.json` at init; if file is missing, uses hardcoded defaults (does **not** auto-create)
- Window default: 1280×720, resizable, title "SunnyLand"
- vsync uses `SDL_RENDERER_VSYNC_ADAPTIVE` when enabled
- Logical resolution set to **half** window size (`SDL_SetRenderLogicalPresentation`)

## Testing / Dev State

- No test framework; no CI; no formatter/linter config committed
- Test code lives in `GameApp::test*()` methods called from `init()` and `update()` — remove/replace them
- Commit messages follow a `X.Y description` pattern (numbered lessons)

## Project Roots

- `.gitignore` covers macOS/IDE/build artifacts but **not** the `SunnyLand-Linux` binary
- Code comments in Chinese; file headers use the `@Date/@LastEditors` template from VSCode
