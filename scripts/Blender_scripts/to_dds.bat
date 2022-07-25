@echo off
cd %1
IF EXIST %3 (
echo text_already_compressed
) ELSE (compressonatorcli -fd BC7 -EncodeWith HPC -miplevels 9 .\%2 .\%3)

