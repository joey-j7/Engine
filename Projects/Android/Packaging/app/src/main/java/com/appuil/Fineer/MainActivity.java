package com.appuil.Fineer;

import android.Manifest;
import android.content.pm.PackageManager;

import androidx.annotation.NonNull;

import android.app.NativeActivity;
import androidx.core.app.ActivityCompat;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import java.util.Arrays;

import com.appuil.Launcher.NativeLibrary;

public class MainActivity extends NativeActivity
    implements ActivityCompat.OnRequestPermissionsResultCallback
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle SavedInstanceState)
    {
        super.onCreate(SavedInstanceState);
    }
	
    public void RequestPermissions(String[] Permissions) {
        boolean NeedsRequest = false;

        // Check if one of the permissions is not granted
        for (String Permission : Permissions)
        {
           int PermissionState = ActivityCompat.checkSelfPermission(this, Permission);
           
           if(PermissionState != PackageManager.PERMISSION_GRANTED)
           {
               NeedsRequest = true;
               break;
           }
        }

        // In case one is not granted, file a request
        if (NeedsRequest)
        {
            ActivityCompat.requestPermissions(
                this,
                Permissions,
                1 // "Custom" request code
            );

            return;
        }

        // Everything has already been granted, pass result
        int[] Granted = new int[Permissions.length];
        Arrays.fill(Granted, 1);

        NativeLibrary.PermissionNotify(
            Permissions,
            Granted
        );
    }
    
    @Override
    public void onRequestPermissionsResult(
        int RequestCode,
        @NonNull String[] Permissions,
        @NonNull int[] GrantResults
    ) {
        super.onRequestPermissionsResult(
            RequestCode,
            Permissions,
            GrantResults
        );
    
        NativeLibrary.PermissionNotify(
            Permissions,
            GrantResults
        );
    }
}
