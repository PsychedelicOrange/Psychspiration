@echo off
del ".\vcpkg\packages\gli_x64-windows-static\include\gli\texture.hpp"
del ".\vcpkg\packages\gli_x64-windows-static\include\gli\core\texture.inl"
del ".\build\vcpkg_installed\x64-windows-static\include\gli\texture.hpp"
del ".\build\vcpkg_installed\x64-windows-static\include\gli\core\texture.inl"
copy ".\external_modified_files\texture.hpp" ".\vcpkg\packages\gli_x64-windows-static\include\gli\"
copy ".\external_modified_files\texture.inl" ".\vcpkg\packages\gli_x64-windows-static\include\gli\core\"
copy ".\external_modified_files\texture.hpp" ".\build\vcpkg_installed\x64-windows-static\include\gli\"
copy ".\external_modified_files\texture.inl" ".\build\vcpkg_installed\x64-windows-static\include\gli\core\"
echo done