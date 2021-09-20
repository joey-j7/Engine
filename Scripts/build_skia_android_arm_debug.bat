cd ../Libraries/skia

if not exist ../../Builds/skia/Android/ARM/Debug/ (
REM Android ARM Debug
%cd%/bin/gn gen ../../Builds/skia/Android/ARM/Debug --args=^
"ndk=\"%ANDROID_NDK%\"^
 is_debug=true^
 target_os=\"android\"^
 target_cpu=\"arm\"^
 skia_use_system_libjpeg_turbo=false^
 skia_use_system_libpng=false^
 skia_use_system_libwebp=false^
 skia_use_system_zlib=false^
 skia_use_system_icu=false^
 skia_use_system_harfbuzz=false^
 skia_use_system_expat=false^
 skia_use_system_freetype2=false^
 skia_enable_svg=true^
 skia_use_gl=true^
 skia_use_vulkan=true"
)
 
cd ../../

ninja -C %cd%/Builds/skia/Android/ARM/Debug

cd Scripts