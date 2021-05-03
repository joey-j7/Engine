cd ../Libraries/skia


if not exist ../../Builds/skia/Windows/Win32/Release/ (
REM Windows x86 Release
%cd%/bin/gn gen ../../Builds/skia/Windows/Win32/Release --args=^
"is_debug=false^
 target_cpu=\"x86\"^
 extra_cflags=[\"/MD\"]^
 skia_use_system_libjpeg_turbo=false^
 skia_use_system_libpng=false^
 skia_use_system_libwebp=false^
 skia_use_system_zlib=false^
 skia_use_system_icu=false^
 skia_use_system_harfbuzz=false^
 is_official_build=true^
 skia_use_gl=true^
 skia_use_vulkan=true"
)

cd ../../

ninja -C %cd%/Builds/skia/Windows/Win32/Release

cd Scripts