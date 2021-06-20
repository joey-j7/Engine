cd ../
python %cd%/Libraries/skia/tools/git-sync-deps

REM rd /s /q "%cd%/Builds/skia/Android/ARM/Debug/obj"
REM rd /s /q "%cd%/Builds/skia/Android/ARM/Release/obj"
REM rd /s /q "%cd%/Builds/skia/Android/ARM64/Debug/obj"
REM rd /s /q "%cd%/Builds/skia/Android/ARM64/Release/obj"
REM rd /s /q "%cd%/Builds/skia/Android/Win32/Debug/obj"
REM rd /s /q "%cd%/Builds/skia/Android/Win32/Release/obj"
REM rd /s /q "%cd%/Builds/skia/Android/x64/Debug/obj"
REM rd /s /q "%cd%/Builds/skia/Android/x64/Release/obj"

cd Scripts/
build_skia_android_arm_debug.bat
build_skia_android_arm_release.bat
build_skia_android_arm64_debug.bat
build_skia_android_arm64_release.bat
build_skia_windows_win32_debug.bat
build_skia_windows_win32_release.bat
build_skia_windows_x64_debug.bat
build_skia_windows_x64_release.bat
 
REM Create singular .sln
REM python %cd%/bin/gn/gn_meta_sln.py