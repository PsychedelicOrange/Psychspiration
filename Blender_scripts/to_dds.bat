@echo off
cd %1
compressonatorcli -fd BC7 -EncodeWith HPC -miplevels 9 .\%2 .\%3
