# Stockfish framework for iOS, iPadOS and Mac Catalyst

This repository builds `Stockfish.xcframework`: a modified version of the
[Stockfish](https://stockfishchess.org) 16 chess engine, packaged as a framework
that an app links and drives through a small C interface. It is the engine
used in the Chess King apps for iOS.

## License

Stockfish is free software distributed under the GNU General Public License
version 3 or later. The license text is in [Copying.txt](Copying.txt), the
authors of Stockfish are listed in [AUTHORS](AUTHORS).

- Stockfish (`Stockfish/`): Copyright (C) 2004-2023 The Stockfish developers (see AUTHORS).
- `Stockfish/incbin/`: incbin by Dale Weiler, public domain ([UNLICENCE](Stockfish/incbin/UNLICENCE)).
- `nn-5af11540bbfe.nnue`: the default neural network of Stockfish 16, see [Neural network](#neural-network).

The framework carries `Copying.txt` and `AUTHORS` inside its bundle, and its
`Info.plist` has the copyright notice (`NSHumanReadableCopyright`).

## Source of shipped builds

Every framework build that ships in an app is tagged; the tag is the complete
source of that build.

| Tag | Commit | Engine | Built |
|---|---|---|---|
| `sf_16-ck.1` | `58e4ab0` | Stockfish 16 | 2025-10 |

`sf_16-ck.1` was built before the license files and notices were added; they
do not change the engine code. That build also contains the public headers
`Stockfish.h` and `stockfishEngine.hpp` of the same commit: the project at that
commit does not mark them public, later commits do.

Builds from 18 February 2022 to May 2024 were based on Stockfish 11. Their
sources are in the history of this repository, commits `9df9aaf` to `be024fd`.

## Changes from Stockfish 16

The base is the official tag [`sf_16`](https://github.com/official-stockfish/Stockfish/tree/sf_16)
(commit `68e1e9b`, 2023-06-24). Commit `849e716` of this repository holds its
`src/` folder unmodified, so `git diff 849e716 -- Stockfish` shows every change.
Files not listed below are identical to `sf_16`.

Modified on 2025-10-09 and 2025-10-10:

- `main.cpp`: no `main()`. `stockfish_initialize()` runs the Stockfish startup
  code unless the engine is already running; command-line arguments are not used.
  `stockfish_cmd()` passes one command to `UCI::cmd()`; after `quit` the engine
  needs to be initialized again. Engine output goes to a message queue:
  `engine_message()` adds a message, `getSearchMessage()` takes the oldest one,
  `clearAllMessages()` empties the queue.
- `uci.cpp`, `uci.h`: new `UCI::cmd()` handles one command per call on a
  position kept between calls, instead of the `UCI::loop()` read loop (still in
  the source, not called). The reply to `uci` goes to the message queue;
  `bench` reports its totals to the queue and prints `bench END`.
- `search.cpp`: search output (`info`, `bestmove`, perft results) goes to the
  message queue instead of standard output.

Removed: `Makefile` (the framework is built with the Xcode project).

Added:

- `Stockfish/stockfishEngine.cpp`, `Stockfish/stockfishEngine.hpp`: the C interface.
- `Stockfish/Stockfish.h`: the umbrella header of the framework.
- `Stockfish.xcodeproj`, `scripts/build-xcframework.sh`: the build.
- `PrivacyInfo.xcprivacy`: Apple privacy manifest, copied into the framework.
- `nn-5af11540bbfe.nnue`: the neural network.

The Stockfish 11 based builds had the same kind of changes. Commit `ffd0aab`
holds Stockfish 11 (tag `sf_11`) unmodified, so `git diff ffd0aab be024fd -- Stockfish`
shows them.

## C interface

```c
void engine_initialize(void);              // starts the engine and sends "uci"
void engine_cmd(const char *cmd);          // one UCI command, e.g. "go depth 20"
const char *engine_getSearchMessage(void); // next output line or NULL
void engine_clearAllMessages(void);
```

- There is one engine per process.
- `go` starts the search on Stockfish's threads and returns at once; other
  commands run synchronously.
- `isready` and the debugging commands (`d`, `eval`, `compiler`) still write to
  standard output, not to the queue.
- The string returned by `engine_getSearchMessage()` is valid until the next call.

## Neural network

`nn-5af11540bbfe.nnue` is the default network of Stockfish 16
(`EvalFileDefaultName` in `Stockfish/evaluate.h`), from
<https://tests.stockfishchess.org/api/nn/nn-5af11540bbfe.nnue>. It is embedded
into the binary at build time with incbin. A network's file name starts with the
first 12 hex digits of its SHA-256:

```
shasum -a 256 nn-5af11540bbfe.nnue
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

Platforms: iOS 13 and later (arm64), iOS Simulator (arm64, x86_64), Mac Catalyst
(arm64, x86_64).
