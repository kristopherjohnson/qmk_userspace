# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this repo is

A **QMK Userspace** repository: an external, self-contained set of QMK keymaps that compile against the upstream `qmk/qmk_firmware` repo *without* forking it. The firmware sources are not vendored here — QMK overlays this directory onto its own tree at build time.

Currently it contains one keymap: `keebio/iris/rev8:kristopherjohnson` (a 56-key split ergonomic keyboard, RP2040-based).

## Build & flash

Builds require the QMK CLI and a local `qmk_firmware` checkout. One-time setup links this directory in as the overlay:

```
qmk config user.overlay_dir="$(realpath .)"   # run from this repo's root
```

Then, from this directory:

```
qmk compile -kb keebio/iris/rev8 -km kristopherjohnson   # compile one keymap
make keebio/iris/rev8:kristopherjohnson                  # equivalent (see Makefile)
qmk userspace-compile                                    # build all targets listed in qmk.json
```

- `make` here is a thin shim (`Makefile`) that delegates to `$(qmk config user.qmk_home)` and passes `QMK_USERSPACE=<this dir>`. It errors if `user.qmk_home` is unset.
- `qmk.json` (schema `userspace_version` 1.1) is the authoritative list of build targets. Manage it with `qmk userspace-add` / `qmk userspace-remove` / `qmk userspace-list` rather than editing by hand.
- A successful flashable artifact is the `.uf2` file (an example, `keebio_iris_rev8_kristopherjohnson.uf2`, is committed at the root). Flash by copying the `.uf2` to the RP2040 mass-storage bootloader volume.

### CI

`.github/workflows/build_binaries.yaml` calls the reusable `qmk/.github` workflows to build every target in `qmk.json` and publish firmware to the repo's Releases tab on every push. To pin a different firmware base, edit `qmk_repo` / `qmk_ref` in that workflow.

### Dev container

`.devcontainer/` provides the `ghcr.io/qmk/qmk_cli` image; `setup.sh` installs the QMK CLI, clones `qmk_firmware` to `/workspaces/qmk_firmware`, and wires up `qmk config`. Use this for a zero-setup build environment.

## Keymap layout & structure

Each keymap lives at `keyboards/<vendor>/<board>/keymaps/<name>/` and consists of:
- `keymap.c` — the layer tables plus optional QMK user hooks.
- `config.h` — `#define`-style feature config (RGB defaults, split-sync flags, behavior toggles).
- `rules.mk` — feature enable/disable (`X_ENABLE = yes`).

Add a new keymap with `qmk new-keymap -kb <board> -km <name>`, then register it via `qmk userspace-add`. The `layouts/<layout>/<name>/keymap.*` form is also supported.

## Architecture notes for the iris/rev8:kristopherjohnson keymap

This keymap is the substance of the repo; understanding it requires reading `keymap.c` and `config.h` together.

- **5 layers**, switched mostly via layer-tap (`LT`) keys on the thumb cluster:
  - `[0]` base (QWERTY) — uses **home-row mods**: `LSFT_T`/`LCTL_T`/`LALT_T`/`LGUI_T` on the left home row and the mirror on the right, so the resting keys double as modifiers when held.
  - `[1]` Lower — numeric keypad on the right, symbols on the left.
  - `[2]` Raise — function keys and arrow/navigation cluster.
  - `[3]` Symbols — reached by holding Space, or as a tri-layer (Raise+Lower).
  - `[4]` Navigation — reached by holding Enter.
  - `[3]`/`[4]` are activated both directly (hold Space / hold Enter) and indirectly via tri-layer. `TRI_LAYER_ENABLE` (in `rules.mk`) makes holding `[1]`+`[2]` activate `[3]`.

- **Split-half state sync**: this is a split keyboard, so per-half state must be replicated. `config.h` enables `SPLIT_LAYER_STATE_ENABLE`, `SPLIT_LED_STATE_ENABLE`, `SPLIT_MODS_ENABLE`, and declares a custom transaction id `USER_SYNC_A` via `SPLIT_TRANSACTION_IDS_USER`.

- **Caps Word / Caps Lock LED indicators**: Caps Word state is *not* auto-synced across halves, so `keymap.c` implements it manually:
  - `caps_word_set_user()` detects state change on the master and pushes it to the slave with `transaction_rpc_send(USER_SYNC_A, ...)`.
  - `caps_word_sync_handler()` (registered in `keyboard_post_init_user`) receives it on the other half.
  - `rgb_matrix_indicators_advanced_user()` then lights all keylight LEDs **green** for Caps Lock and **blue** for Caps Word.
  - If you add any other cross-half custom state, follow this same RPC pattern and reuse/extend the `USER_SYNC_A` transaction.

- `CAPS_WORD_ENABLE = yes` plus `DOUBLE_TAP_SHIFT_TURNS_ON_CAPS_WORD` (in `config.h`) make a double-tap of Shift toggle Caps Word.

## Code style

- `.clang-format` governs C formatting; the keymap tables are wrapped in `// clang-format off` / `// clang-format on` so the hand-aligned layer grids are preserved — keep new layers inside those guards and aligned to the existing column layout.
- `.clangd` configures clangd for this Apple/ARM-cross context: it forces `Compiler: clang` and strips the GCC/AVR/ARM-only flags QMK injects (`-mmcu=`, `-mcpu=`, etc.) so the language server doesn't choke. This is editor tooling only and does not affect the actual firmware build (which uses QMK's real toolchain).
- `.editorconfig` defines whitespace rules.
