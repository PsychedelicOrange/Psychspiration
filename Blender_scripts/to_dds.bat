@echo off
cd %1
compressonatorcli -fd BC7 -GenGPUMipMaps -EncodeWith GPU -miplevels 1 .\%2 .\%3
