package com.example.myapplication;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import androidx.appcompat.app.AppCompatActivity;

public class Intro extends AppCompatActivity{
    Button cont;

    @Override
    protected void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
        setContentView(R.layout.intro);

        //chuyển trang sang đăng nhập
        cont = findViewById(R.id.cont);
        cont.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view) {
                Intent mh2 = new Intent(Intro.this, Login_tab.class);
                startActivity(mh2);
            }
        });

    }
}
