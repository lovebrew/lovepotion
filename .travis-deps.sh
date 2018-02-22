#!/bin/sh
set -e
set -x

# Build and install devkitAArch64 + libnx
wget https://www.dropbox.com/s/6vht1m4c9v89lpx/updateAA64.pl?dl=1
perl updateAA64.pl
