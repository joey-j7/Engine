package com.appuil.Launcher;

public class NativeLibrary
{
    static {
        try {
    	    System.loadLibrary("Launcher");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Launcher library failed to load.\n" + e);
            System.exit(1);
        }
    }
    
    public static native void init();
    public static native void notifyCameraPermission(boolean granted);
    // public static native void drawCameraFrame(byte[] data, int width, int height, int rotation);
}