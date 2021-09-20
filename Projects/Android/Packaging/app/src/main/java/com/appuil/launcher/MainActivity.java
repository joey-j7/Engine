
package com.appuil.Launcher;

import android.Manifest;
import android.content.pm.PackageManager;

import androidx.annotation.NonNull;

import android.app.NativeActivity;
import androidx.core.app.ActivityCompat;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

public class MainActivity extends NativeActivity
        implements ActivityCompat.OnRequestPermissionsResultCallback
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
    }
	
	private static final int PERMISSION_REQUEST_CODE_CAMERA = 1;
    public void RequestCamera() {
        String[] accessPermissions = new String[] {
            Manifest.permission.CAMERA,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE
        };
        boolean needRequire  = false;
        for(String access : accessPermissions) {
           int curPermission = ActivityCompat.checkSelfPermission(this, access);
           if(curPermission != PackageManager.PERMISSION_GRANTED) {
               needRequire = true;
               break;
           }
        }
        if (needRequire) {
            ActivityCompat.requestPermissions(
                    this,
                    accessPermissions,
                    PERMISSION_REQUEST_CODE_CAMERA);
            return;
        }
        NativeLibrary.notifyCameraPermission(true);
    }
    
    // public void onPreviewFrame(byte[] data, int width, int height) {
    //    NativeLibrary.drawCameraFrame(data, width, height, getOrientation());
    // }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        /*
         * if any permission failed, the sample could not play
         */
        if (PERMISSION_REQUEST_CODE_CAMERA != requestCode) {
            super.onRequestPermissionsResult(requestCode,
                                             permissions,
                                             grantResults);
            return;
        }
    
        // assertEquals(grantResults.length, 2);
        NativeLibrary.notifyCameraPermission(grantResults[0] == PackageManager.PERMISSION_GRANTED &&
                               grantResults[1] == PackageManager.PERMISSION_GRANTED);
    }
}
