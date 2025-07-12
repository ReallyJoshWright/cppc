#!/bin/bash

INSTALL_DIR="$HOME/.config/.cppc"
ZSHRC="$HOME/.zshrc"
LINE="export PATH=\"\$PATH:$INSTALL_DIR\""

make

echo "Creating install directory: $INSTALL_DIR"
mkdir -p "$INSTALL_DIR"

echo "Adding cppc binary and builder.h to $INSTALL_DIR"
cp cppc "$INSTALL_DIR"
cp builder.h "$INSTALL_DIR"

if ! grep -Fxq "$LINE" "$ZSHRC"; then
  echo "$LINE" >> "$ZSHRC"
  echo "Added $INSTALL_DIR to PATH in $ZSHRC"
  echo "Restart terminal or run: source ~/.zshrc"
else
  echo "$INSTALL_DIR already exists in PATH"
fi

echo "Installation Complete!"
