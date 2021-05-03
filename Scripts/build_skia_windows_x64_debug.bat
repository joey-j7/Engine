cd ../Libraries/skia

if not exist ../../Builds/skia/Windows/x64/Debug/ (
REM Windows x64 Debug
%cd%/bin/gn gen ../../Builds/skia/Windows/x64/Debug --args=^
"target_cpu=\"x64\"^
 is_debug=true^
 extra_cflags=[\"/MDd\"]^
 skia_use_system_libjpeg_turbo=false^
 skia_use_system_libpng=false^
 skia_use_system_libwebp=false^
 skia_use_system_zlib=false^
 skia_use_system_icu=false^
 skia_use_system_harfbuzz=false^
 skia_use_gl=true^
 skia_use_vulkan=true"
)

cd ../../

ninja -C %cd%/Builds/skia/Windows/x64/Debug

cd Scripts