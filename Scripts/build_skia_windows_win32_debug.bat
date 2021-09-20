cd ../Libraries/skia

if not exist ../../Builds/skia/Windows/Win32/Debug/ (
REM Windows x86 Debug
%cd%/bin/gn gen ../../Builds/skia/Windows/Win32/Debug --args=^
"target_cpu=\"x86\"^
 is_debug=true^
 extra_cflags=[\"/MTd\"]^
 win_vc=\"C:\\Program Files\\Microsoft Visual Studio\\2022\\Preview\\VC\"^
 clang_win=\"C:\\Program Files\\Microsoft Visual Studio\\2022\\Preview\\VC\\Tools\\Llvm\\x64\"^
 skia_use_system_libjpeg_turbo=false^
 skia_use_system_libpng=false^
 skia_use_system_libwebp=false^
 skia_use_system_zlib=false^
 skia_use_system_icu=false^
 skia_use_system_harfbuzz=false^
 skia_enable_svg=true^
 skia_use_gl=true^
 skia_use_vulkan=true"
)

cd ../../

ninja -C %cd%/Builds/skia/Windows/Win32/Debug

cd Scripts