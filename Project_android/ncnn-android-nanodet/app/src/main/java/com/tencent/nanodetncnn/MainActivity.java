// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2021 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

package com.tencent.nanodetncnn;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.Spinner;

import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;

public class MainActivity extends Activity implements SurfaceHolder.Callback
{
    // 初始化的一些变量
    public static final int REQUEST_CAMERA = 100;

    private NanoDetNcnn nanodetncnn = new NanoDetNcnn(); // 调用ncnn的java接口类
    private int facing = 0; // 用来记录前摄还是后摄
    private SurfaceView cameraView; // 预览界面

    // 初始化函数
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main); // 加载布局

        // 保持屏幕唤醒不锁屏
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // 绑定预览的控件，并设置格式为RGBA8888
        cameraView = (SurfaceView) findViewById(R.id.cameraview);
        cameraView.getHolder().setFormat(PixelFormat.RGBA_8888);
        cameraView.getHolder().addCallback(this);

        // 绑定切换镜头的按键，并设置监听函数
        Button buttonSwitchCamera = (Button) findViewById(R.id.buttonSwitchCamera);
        buttonSwitchCamera.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View arg0) {
                int new_facing = 1 - facing; // 切换摄像头
                nanodetncnn.closeCamera(); // 关闭已经打开的摄像头
                nanodetncnn.openCamera(new_facing); // 用切换后的摄像头重新打开
                facing = new_facing;
            }
        });

        // 所有初始化完成后，重新加载模型
        reload();
    }

    private void reload()
    {
        boolean ret_init = nanodetncnn.loadModel(getAssets());
        if (!ret_init)
        {
            Log.e("MainActivity", "nanodetncnn loadModel failed");
        }
    }

    // 这是SurfaceView需要实现的三个方法
    // surface尺寸发生改变的时候调用，如横竖屏切换
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        nanodetncnn.setOutputWindow(holder.getSurface());
    }
    // surface创建的时候调用，一般在该方法中启动绘图的线程
    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
    }
    // surface被销毁的时候调用，如退出游戏画面，一般在该方法中停止绘图线程
    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
    }

    // onPause方法，app被覆盖后就把ncnn的摄像头关了就完事了
    @Override
    public void onPause()
    {
        super.onPause();
        nanodetncnn.closeCamera();
    }
    // onResume方法，app被覆盖恢复后把摄像头重新开起来
    @Override
    public void onResume()
    {
        super.onResume();
        if (ContextCompat.checkSelfPermission(getApplicationContext(), Manifest.permission.CAMERA) == PackageManager.PERMISSION_DENIED)
        {
            ActivityCompat.requestPermissions(this, new String[] {Manifest.permission.CAMERA}, REQUEST_CAMERA);
        }
        nanodetncnn.openCamera(facing);
    }


}
