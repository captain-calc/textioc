#!/bin/bash
#----------------------------------------
# Credits for this useful script go to
# beckadamtheinventor
#----------------------------------------


#----------------------------------------
#Put your program name in place of "DEMO"
name='TEXTIOC.8xv'
#----------------------------------------

mkdir "bin" || echo ""

echo "compiling to $name"
fasmg src/main.asm bin/$name

read -p "Finished. Press any key to exit"
