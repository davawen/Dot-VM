#! /usr/bin/env fish

argparse -n build --ignore-unknown 'r/run' -- $argv
or return

cd build
cmake ..
make
or return

if test -n "$_flag_r"
	cd ..
	./build/dot-vm $argv
end
