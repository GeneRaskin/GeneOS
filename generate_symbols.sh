#!/bin/bash
i686-elf-nm $1 | grep " T " | awk '{ print $1" "$3 }' > $1.sym