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
    
    public static native void Init();
    public static native void PermissionNotify(String[] Permissions, int[] GrantResults);
}