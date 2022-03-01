#!/bin/sh

echo "Zipping sd card image..."
gzip -kfv $BINARIES_DIR/sdcard.img
