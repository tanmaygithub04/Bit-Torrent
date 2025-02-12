#!/bin/bash

# Create the src directory if it doesn't exist
mkdir -p src

# Create the header files in src directory
touch src/torrentfile.h
touch src/tracker.h
touch src/peer.h
touch src/piece_download.h
touch src/file_download.h
touch src/utils.h

# Create the source files in src directory
touch src/torrentfile.cpp
touch src/tracker.cpp
touch src/peer.cpp
touch src/piece_download.cpp
touch src/file_download.cpp
touch src/utils.cpp

# Move main.cpp into the src directory (optional but recommended for organization)
if [ -f main.cpp ]; then
  mv main.cpp src/
fi

echo "Directory structure created in 'src' directory."
echo "Header files (.h) and source files (.cpp) created in 'src'."
echo "Moved main.cpp to 'src' directory (if it existed in root)."
echo "Please move the content of your namespaces into the respective files in 'src'."
echo "Make sure to update your build.sh to compile from the 'src' directory if necessary."