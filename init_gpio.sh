#/bin/sh

gpio unexportall
gpio export 19 out
gpio export 20 out
gpio export 21 out
gpio export 2 in
gpio export 3 in
gpio export 4 in

