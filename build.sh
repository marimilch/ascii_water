#!bin/bash
echo "Download necessary packages..."
clib install tingping/ustring
echo "Compile..."
cc ascii_water.c -o ascii_water
echo "Aaaaaand done! <3"