python %cd%/Libraries/skia/tools/git-sync-deps

REM rd /s /q "%cd%/Builds/skia/Android/ARM/Debug/obj"
REM rd /s /q "%cd%/Builds/skia/Android/ARM/Release/obj"
REM rd /s /q "%cd%/Builds/skia/Android/ARM64/Debug/obj"
REM rd /s /q "%cd%/Builds/skia/Android/ARM64/Release/obj"
REM rd /s /q "%cd%/Builds/skia/Android/Win32/Debug/obj"
REM rd /s /q "%cd%/Builds/skia/Android/Win32/Release/obj"
REM rd /s /q "%cd%/Builds/skia/Android/x64/Debug/obj"
REM rd /s /q "%cd%/Builds/skia/Android/x64/Release/obj"

cd Libraries/skia

REM Android ARM Debug
%cd%/bin/gn gen ../../Builds/skia/Android/ARM/Debug --args=^
"ndk=\"%ANDROID_NDK%\"^
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
 skia_use_gl=true^
 skia_use_vulkan=true"

REM Android ARM Release
%cd%/bin/gn gen ../../Builds/skia/Android/ARM/Release --args=^
"ndk=\"%ANDROID_NDK%\"^
 is_debug=false^
 is_official_build=true^
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
 skia_use_gl=true^
 skia_use_vulkan=true"

REM Android ARM64 Debug
%cd%/bin/gn gen ../../Builds/skia/Android/ARM64/Debug --args=^
"ndk=\"%ANDROID_NDK%\"^
 target_os=\"android\"^
 target_cpu=\"arm64\"^
 skia_use_system_libjpeg_turbo=false^
 skia_use_system_libpng=false^
 skia_use_system_libwebp=false^
 skia_use_system_zlib=false^
 skia_use_system_icu=false^
 skia_use_system_harfbuzz=false^
 skia_use_system_expat=false^
 skia_use_system_freetype2=false^
 skia_use_gl=true^
 skia_use_vulkan=true"

REM Android ARM64 Release
%cd%/bin/gn gen ../../Builds/skia/Android/ARM64/Release --args=^
"ndk=\"%ANDROID_NDK%\"^
 is_debug=false^
 is_official_build=true^
 target_os=\"android\"^
 target_cpu=\"arm64\"^
 skia_use_system_libjpeg_turbo=false^
 skia_use_system_libpng=false^
 skia_use_system_libwebp=false^
 skia_use_system_zlib=false^
 skia_use_system_icu=false^
 skia_use_system_harfbuzz=false^
 skia_use_system_expat=false^
 skia_use_system_freetype2=false^
 skia_use_gl=true^
 skia_use_vulkan=true"
 
REM Windows x86 Debug
%cd%/bin/gn gen ../../Builds/skia/Windows/Win32/Debug --args=^
"target_cpu=\"x86\"^
 extra_cflags=[\"/MDd\"]^
 skia_use_system_libjpeg_turbo=false^
 skia_use_system_libpng=false^
 skia_use_system_libwebp=false^
 skia_use_system_zlib=false^
 skia_use_system_icu=false^
 skia_use_system_harfbuzz=false^
 skia_use_gl=true^
 skia_use_vulkan=true"
 
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

REM Windows x64 Debug
%cd%/bin/gn gen ../../Builds/skia/Windows/x64/Debug --args=^
"target_cpu=\"x64\"^
 extra_cflags=[\"/MDd\"]^
 skia_use_system_libjpeg_turbo=false^
 skia_use_system_libpng=false^
 skia_use_system_libwebp=false^
 skia_use_system_zlib=false^
 skia_use_system_icu=false^
 skia_use_system_harfbuzz=false^
 skia_use_gl=true^
 skia_use_vulkan=true"
 
REM Windows x64 Release
%cd%/bin/gn gen ../../Builds/skia/Windows/x64/Release --args=^
"is_debug=false^
 target_cpu=\"x64\"^
 extra_cflags=[\"/MD\"]^
 is_official_build=true^
 skia_use_system_libjpeg_turbo=false^
 skia_use_system_libpng=false^
 skia_use_system_libwebp=false^
 skia_use_system_zlib=false^
 skia_use_system_icu=false^
 skia_use_system_harfbuzz=false^
 skia_use_expat=false^
 skia_use_gl=true^
 skia_use_vulkan=true"
 
REM python %cd%/bin/gn/gn_meta_sln.py

cd ../../

REM Build all
ninja -C %cd%/Builds/skia/Android/ARM/Debug
ninja -C %cd%/Builds/skia/Android/ARM/Release
ninja -C %cd%/Builds/skia/Android/ARM64/Debug
ninja -C %cd%/Builds/skia/Android/ARM64/Release
ninja -C %cd%/Builds/skia/Windows/Win32/Debug
ninja -C %cd%/Builds/skia/Windows/Win32/Release
ninja -C %cd%/Builds/skia/Windows/x64/Debug
ninja -C %cd%/Builds/skia/Windows/x64/Release
