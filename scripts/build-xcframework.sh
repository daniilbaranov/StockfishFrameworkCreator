#!/bin/bash
# Builds Stockfish.xcframework (iOS, iOS Simulator, Mac Catalyst) from this checkout.
#
#   scripts/build-xcframework.sh [output-dir] [xcodebuild settings...]
#
# output-dir defaults to build/. Extra arguments go to every xcodebuild archive call,
# e.g. CODE_SIGNING_ALLOWED=NO or DEVELOPMENT_TEAM=<your team>.
# The output folder also gets BUILD-INFO.txt with the commit the framework was built from.

set -euo pipefail

cd "$(dirname "$0")/.."
OUT="${1:-build}"
shift || true
mkdir -p "$OUT"
OUT="$(cd "$OUT" && pwd)"

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

if [ -n "$(git status --porcelain)" ]; then
    echo "warning: the working tree has uncommitted changes" >&2
fi

archive() {
    xcodebuild archive -project Stockfish.xcodeproj -scheme Stockfish \
        -archivePath "$WORK/$1.xcarchive" -derivedDataPath "$WORK/DerivedData" \
        "${@:2}" SKIP_INSTALL=NO
}

archive iphoneos -sdk iphoneos ${@+"$@"}
archive iphonesimulator -sdk iphonesimulator ${@+"$@"}
archive catalyst -destination 'platform=macOS,variant=Mac Catalyst' ${@+"$@"}

rm -rf "$OUT/Stockfish.xcframework"
xcodebuild -create-xcframework \
    -framework "$WORK/iphoneos.xcarchive/Products/Library/Frameworks/Stockfish.framework" \
    -framework "$WORK/iphonesimulator.xcarchive/Products/Library/Frameworks/Stockfish.framework" \
    -framework "$WORK/catalyst.xcarchive/Products/Library/Frameworks/Stockfish.framework" \
    -output "$OUT/Stockfish.xcframework"

{
    echo "commit: $(git rev-parse HEAD)"
    echo "describe: $(git describe --tags --always --dirty)"
    echo "built: $(date -u +%Y-%m-%dT%H:%M:%SZ)"
    xcodebuild -version
} > "$OUT/BUILD-INFO.txt"

echo "$OUT/Stockfish.xcframework"
