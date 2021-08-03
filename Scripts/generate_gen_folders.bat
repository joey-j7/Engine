cd ../Libraries/skia

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
 skia_use_gl=true^
 skia_use_vulkan=true"

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
)

%cd%/bin/gn gen ../../Builds/skia/Android/ARM64/Debug --args=^
"ndk=\"%ANDROID_NDK%\"^
 is_debug=true^
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
)

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
)

%cd%/bin/gn gen ../../Builds/skia/Windows/Win32/Debug --args=^
"target_cpu=\"x86\"^
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
)

cd ../../Scripts