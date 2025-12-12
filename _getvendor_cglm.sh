
#!/usr/bin/env bash
set -euo pipefail

# Hardcoded settings
REPO="https://github.com/recp/cglm.git"
TARGET_DIR="vendor/cglm"
DEPTH=1

echo "Repository: $REPO"
echo "Target directory: $TARGET_DIR"
echo "Clone depth: $DEPTH"

# Ensure parent exists
mkdir -p "$(dirname "$TARGET_DIR")"

if [ -d "$TARGET_DIR" ]; then
  if [ -d "$TARGET_DIR/.git" ]; then
    echo "Target exists and is a git repository. Attempting shallow update..."
    (
      cd "$TARGET_DIR"
      # Try to fetch shallowly; fall back to full fetch if needed
      git fetch --depth="$DEPTH" origin || git fetch origin
      # Reset to remote HEAD (safe for vendor clones)
      git reset --hard origin/HEAD
    )
    echo "Repository updated in $TARGET_DIR"
  else
    echo "Target exists but is not a git repository: $TARGET_DIR"
    echo "Remove or move it and re-run the script."
    exit 1
  fi
else
  echo "Cloning $REPO into $TARGET_DIR ..."
  git clone --depth="$DEPTH" "$REPO" "$TARGET_DIR"
  echo "Clone complete."
fi

echo "Done."
