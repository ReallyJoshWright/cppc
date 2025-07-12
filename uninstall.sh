#!/bin/bash

INSTALL_DIR="$HOME/.config/.cppc"
ZSHRC="$HOME/.zshrc"
LINE="export PATH=\"\$PATH:$INSTALL_DIR\""

echo "Removing install directory: $INSTALL_DIR"
rm -rf "$INSTALL_DIR"

if grep -Fxq "$LINE" "$ZSHRC"; then
  sed -i.bak "\|$LINE|d" "$ZSHRC"
  echo "Removed PATH entry from $ZSHRC"
  echo "Backup saved as $ZSHRC.bak"
fi

echo "Uninstall Complete!"
