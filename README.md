# Stockfish framework for iOS, iPadOS and Mac Catalyst

This repository builds `Stockfish.xcframework`: a modified version of the
[Stockfish](https://stockfishchess.org) 19 chess engine, packaged as a framework
that an app links and drives through a small C interface. It is the engine
used in the Chess King apps for iOS.

Branches: `stockfish-19` builds Stockfish 19; `main` keeps the Stockfish 16
framework (tags `sf_16-ck.1` and `sf_16-ck.2`) with its own README.

## License

Stockfish is free software distributed under the GNU General Public License
version 3 or later. The license text is in [Copying.txt](Copying.txt), the
authors of Stockfish are listed in [AUTHORS](AUTHORS).

- Stockfish (`Stockfish/`): Copyright (C) 2004-2026 The Stockfish developers (see AUTHORS).
- C interface (`Stockfish/stockfishEngine.cpp`, `Stockfish/stockfishEngine.hpp`):
  Copyright (C) 2021 Daniil Baranov, GPL version 3 or later.
- `Stockfish/incbin/`: incbin by Dale Weiler, public domain ([UNLICENCE](Stockfish/incbin/UNLICENCE)).
- `nn-1a298aa575a0.nnue`: the default neural network of Stockfish 19, see [Neural network](#neural-network).

The framework carries `Copying.txt` and `AUTHORS` inside its bundle, and its
`Info.plist` has the copyright notice (`NSHumanReadableCopyright`).

## Tagged builds

Every framework build that ships in an app is tagged; the tag is the complete
source of that build.

| Tag | Commit | Engine | Built |
|---|---|---|---|
| `sf_16-ck.1` | `58e4ab0` | Stockfish 16 | 2025-10 |
| `sf_16-ck.2` | `a5e19c2` | Stockfish 16 | 2026-10 |

`sf_16-ck.1` and `sf_16-ck.2` are on `main`; its README describes them.

Builds from 18 February 2022 to May 2024 were based on Stockfish 11. Their
sources are in the history of this repository, commits `9df9aaf` to `be024fd`.

## Changes from Stockfish 19

The base is the official tag [`sf_19`](https://github.com/official-stockfish/Stockfish/tree/sf_19)
(commit `edb0d9d`, 2026-09-05). Commit `880cd05` of this repository holds its
`src/` folder unmodified, so `git diff 880cd05 -- Stockfish` shows every change.
Files not listed below are identical to `sf_19`.

Modified on 2026-11-09:

- `main.cpp`: no `main()`. `stockfish_initialize()` creates the engine
  (`UCIEngine`) unless it is already running; command-line arguments are not
  used. `stockfish_cmd()` passes one command to `UCIEngine::execute()`; after
  `quit` the engine needs to be initialized again, which creates a new engine
  with the default options. Engine output goes to a message queue:
  `engine_message()` adds a message, `getSearchMessage()` takes the oldest one,
  `clearAllMessages()` empties the queue.
- `uci.cpp`, `uci.h`: the body of `UCIEngine::loop()` moved to the new
  `UCIEngine::execute()`, which handles one command per call (`loop()` calls it).
  `print_info_string()` and `on_bestmove()` write to `sync_out()` instead of
  `std::cout`. `bench` also reports its totals to the output and prints
  `bench END`.
- `misc.cpp`, `misc.h`: `sync_cout` writes to `sync_out()`: standard output, or,
  after `set_sync_output()`, a buffer whose contents go to a function at the end
  of each `sync_cout` block. `main.cpp` passes them to the message queue.
- `shm.h`: no system-wide shared memory on iOS, iPadOS and Mac Catalyst
  (`TARGET_OS_IPHONE`). The network is kept in the app's own memory (the
  fallback Stockfish uses where shared memory is not available) instead of POSIX
  shared memory with files in `/tmp`, a helper thread and an `atexit` handler.

Removed: `Makefile` (the framework is built with the Xcode project). The
`universal/` folder belongs to the Makefile's universal builds; it is kept
unmodified and not compiled.

Added:

- `Stockfish/stockfishEngine.cpp`, `Stockfish/stockfishEngine.hpp`: the C interface.
- `Stockfish/Stockfish.h`: the umbrella header of the framework.
- `Stockfish.xcodeproj`, `scripts/build-xcframework.sh`: the build.
- `PrivacyInfo.xcprivacy`: Apple privacy manifest, copied into the framework.
- `nn-1a298aa575a0.nnue`: the neural network.

The Stockfish 16 and Stockfish 11 based builds had the same kind of changes,
see the README on `main`.

## C interface

```c
void engine_initialize(void);              // starts the engine and sends "uci"
void engine_cmd(const char *cmd);          // one UCI command, e.g. "go depth 20"
const char *engine_getSearchMessage(void); // next output message or NULL
void engine_clearAllMessages(void);
```

- There is one engine per process.
- `go` starts the search on Stockfish's threads and returns at once; other
  commands run synchronously.
- All engine output goes to the queue, including `readyok` and the debugging
  commands (`d`, `eval`, `compiler`). A message may hold several lines (the
  reply to `uci` is two messages: the engine name and options, then `uciok`).
  `bench` and `speedtest` print their progress to standard error.
- Before each search Stockfish 19 sends `info string` lines about the
  processors, the threads and the network.
- The string returned by `engine_getSearchMessage()` is valid until the next call.

## Neural network

`nn-1a298aa575a0.nnue` (98,511,183 bytes) is the default network of
Stockfish 19 (`EvalFileDefaultName` in `Stockfish/evaluate.h`), from
<https://tests.stockfishchess.org/api/nn/nn-1a298aa575a0.nnue>. It is embedded
into the binary at build time with incbin; the engine keeps about 109 MiB of
memory for it. A network's file name starts with the first 12 hex digits of
its SHA-256:

```
shasum -a 256 nn-1a298aa575a0.nnue
```

## Building

Needs macOS with Xcode (last built with Xcode 26.6).

```
scripts/build-xcframework.sh [output-dir] [xcodebuild settings...]
```

The script archives the `Stockfish` scheme in Release for iOS, the iOS
Simulator and Mac Catalyst, and combines the three frameworks into
`<output-dir>/Stockfish.xcframework` (default `build/`). `BUILD-INFO.txt` next
to it records the commit and the Xcode version.

The project signs with the Chess King development team. Without access to it,
pass `CODE_SIGNING_ALLOWED=NO` or `DEVELOPMENT_TEAM=<your team>`.

The same steps by hand:

```
xcodebuild archive -scheme Stockfish -archivePath build/Stockfish-iphoneos.xcarchive -sdk iphoneos SKIP_INSTALL=NO
xcodebuild archive -scheme Stockfish -archivePath build/Stockfish-iphonesimulator.xcarchive -sdk iphonesimulator SKIP_INSTALL=NO
xcodebuild archive -scheme Stockfish -archivePath build/Stockfish-catalyst.xcarchive -destination 'platform=macOS,variant=Mac Catalyst' SKIP_INSTALL=NO
xcodebuild -create-xcframework \
  -framework build/Stockfish-iphoneos.xcarchive/Products/Library/Frameworks/Stockfish.framework \
  -framework build/Stockfish-iphonesimulator.xcarchive/Products/Library/Frameworks/Stockfish.framework \
  -framework build/Stockfish-catalyst.xcarchive/Products/Library/Frameworks/Stockfish.framework \
  -output build/Stockfish.xcframework
```

Compiler settings, as in the Makefile's `armv8` and `x86-64` builds:

- arm64: `IS_64BIT`, `USE_POPCNT`, `USE_NEON=8`. No dot product instructions
  (`USE_NEON_DOTPROD`): the framework runs on iOS 13, that is on A9 and later,
  and the A12 and older chips do not have them.
- x86_64 (Simulator and Mac Catalyst on Intel Macs): `IS_64BIT`, `USE_SSE2`.
- Release: `NDEBUG`, `-O3 -funroll-loops`; all configurations:
  `-fconstexpr-steps=500000000`.

Platforms: iOS 13 and later (arm64), iOS Simulator (arm64, x86_64), Mac Catalyst
(arm64, x86_64).
