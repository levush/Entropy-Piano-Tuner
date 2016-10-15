/*****************************************************************************
 * Copyright 2016 Haye Hinrichsen, Christoph Wick
 *
 * This file is part of Entropy Piano Tuner.
 *
 * Entropy Piano Tuner is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Entropy Piano Tuner is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Entropy Piano Tuner. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

package org.uniwue.tp3;

import android.content.Intent;
import java.io.File;
import java.io.InputStream;
import java.io.FileOutputStream;
import android.os.Bundle;
import android.net.Uri;
import android.util.Log;
import android.view.WindowManager;
import android.database.Cursor;
import android.provider.MediaStore;
import android.app.ActivityManager;
import android.app.ActivityManager.MemoryInfo;

import org.qtproject.qt5.android.bindings.QtActivity;

/**
 * Custom activity
 */
public class TunerApplication extends QtActivity {
    private String mFileToOpen = "";
    private boolean  mFileToOpenCached = false;

    static private TunerApplication mApplication;

    public TunerApplication() {
        super();
        mApplication = this;
    }

    static TunerApplication getInstance() {
        return mApplication;
    }

    void mainWindowInitialized() {
        parseIntent(getIntent());
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // keep screen on
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }


    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onNewIntent(Intent intent) {
        parseIntent(intent);
    }

    public String getFileToOpen() {
        return mFileToOpen;
    }
    public boolean getFileToOpenCached() {return mFileToOpenCached;}

    public void shareFile(String title, String path)
    {
        Intent share = new Intent(Intent.ACTION_SEND);
        // xml data (prefer application/xml over text/xml
        share.setType("application/xml");
        share.setAction(Intent.ACTION_SEND);
        File file = new File(path);
        share.putExtra(Intent.EXTRA_STREAM, Uri.fromFile(file));

        this.startActivity(Intent.createChooser(share, title));
    }

    public long getPhysicalMemoryInB()
    {
        MemoryInfo mi = new MemoryInfo();
        ActivityManager activityManager = (ActivityManager) getSystemService(ACTIVITY_SERVICE);
        activityManager.getMemoryInfo(mi);
        return mi.availMem;
    }



    public static native void java_openFile(String s, boolean cached);

    private void parseIntent(Intent intent) {
        if (intent != null) {
            String action = intent.getAction();
            if (!Intent.ACTION_VIEW.equals(action)) {
                return;
            }

            Uri     uri = intent.getData();
            String  scheme = uri.getScheme();

            if (scheme.equals("file")) {
                mFileToOpen = intent.getData().getPath();
            } else if (scheme.equals("content")) {
                // open file from application e.g. mail app
                Cursor  cursor = getContentResolver().query(
                        uri,
                        new String[]{MediaStore.MediaColumns.DISPLAY_NAME},
                        null,
                        null,
                        null);

                cursor.moveToFirst();
                mFileToOpen = cursor.getString(0);

                // check if the file exists (downloaded) elsewise copy to tmp dir
                File f = new File(mFileToOpen);
                if(!f.exists()) {
                    int nameIndex = cursor.getColumnIndex(MediaStore.MediaColumns.DISPLAY_NAME);
                    if (nameIndex >= 0) {
                        mFileToOpen = cursor.getString(nameIndex);
                    } else {
                        return;
                    }
                }
                try {
                    InputStream attachment = getContentResolver().openInputStream(intent.getData());
                    if (attachment == null) {
                        mFileToOpen = "";
                        android.util.Log.e("EntropyTuner", "onStart - cannot access mail attachment");
                    } else {
                        mFileToOpen = this.getCacheDir() + "/" + mFileToOpen;
                        mFileToOpenCached = true;
                        FileOutputStream tmp = new FileOutputStream(mFileToOpen);
                        byte[] buffer = new byte[1024];
                        while (attachment.read(buffer) > 0) {
                            tmp.write(buffer);
                        }

                        tmp.close();
                        attachment.close();
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }

                cursor.close();
            }

            try {
                java_openFile(mFileToOpen, mFileToOpenCached);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
}
