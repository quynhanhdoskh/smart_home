package com.example.myapplication;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class KitchenControl extends AppCompatActivity{
    ImageView toUser, toControl1, toControl2;
    int flag = 0;

    FirebaseDatabase firebaseDatabase = FirebaseDatabase.getInstance();
    DatabaseReference FireSetting = firebaseDatabase.getReference().child("CSDL").child("SETTING_AUTO");
    DatabaseReference FireControl = firebaseDatabase.getReference().child("CSDL").child("CONTROL");
    DatabaseReference FireValue = firebaseDatabase.getReference().child("CSDL").child("VALUES");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.kitchen);

        getGasFromFire();
        goToUser();
        goToControl();
        pushDataMAXMINtofire();
        controlWindFan();
    }

    //chuyển trang sang thông tin user
    void goToUser() {
        toUser = findViewById(R.id.ad);
        toUser.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent mh3 = new Intent(KitchenControl.this, User.class);
                startActivity(mh3);
            }
        });
    }

    void goToControl(){
        toControl1 = findViewById(R.id.back_to_ctrl);
        toControl2 = findViewById(R.id.home);
        toControl1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent mh = new Intent(KitchenControl.this, Control.class);
                startActivity(mh);
            }
        });
        toControl2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent mh = new Intent(KitchenControl.this, Control.class);
                startActivity(mh);
            }
        });
    }

    public void pushDataMAXMINtofire(){
        EditText max = findViewById(R.id.upperLitmit);
        EditText min = findViewById(R.id.lowerLitmit);

        Button update = findViewById(R.id.btn_update);

        update.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String val_max = max.getText().toString();
                String val_min = min.getText().toString();

                if(val_max.length() == 0 && val_min.length() == 0)
                {
                    flag = 3;
                }
                else {
                    if(val_max.length() != 0){
                        if(Integer.parseInt(val_max) <= 15 && Integer.parseInt(val_max) > 0){
                            FireSetting.child("GAS_ON").setValue(Integer.parseInt(val_max)); //đẩy thông tin nồng độ gas max lên fb
                            flag = 1;
                        }
                        else flag = 2;
                    }

                    if(val_min.length() != 0){
                        if(Integer.parseInt(val_min) <= 10 && Integer.parseInt(val_min) > 0){
                            FireSetting.child("GAS_OFF").setValue(Integer.parseInt(val_min)); //đẩy thông tin nồng độ gas min lên fb
                            flag = 1;
                        }
                        else flag = 2;
                    }
                }

                if(flag == 1)
                    Toast.makeText(KitchenControl.this,"LƯU THÀNH CÔNG", Toast.LENGTH_SHORT).show();
                else if(flag == 2)
                    Toast.makeText(KitchenControl.this,"SAI ĐỊNH DẠNG", Toast.LENGTH_SHORT).show();
                else if (flag == 3)
                    Toast.makeText(KitchenControl.this,"HÃY NHẬP THÔNG TIN", Toast.LENGTH_SHORT).show();

            }
        });

    }

    //lấy thông tin nồng độ gas từ fb
    void getGasFromFire(){
        TextView textView1 = findViewById(R.id.upperText);
        TextView textView2 = findViewById(R.id.lowerText);
        TextView textView3 = findViewById(R.id.gasText);
        ProgressBar progressBar = findViewById(R.id.gasProgressBar);

        FireSetting.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                int gasMax = snapshot.child("GAS_ON").getValue(Integer.class);
                int gasMin = snapshot.child("GAS_OFF").getValue(Integer.class);

                textView1.setText(String.valueOf(gasMax));
                textView2.setText(String.valueOf(gasMin));
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {

            }
        });

        FireValue.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                int gasCurrent = snapshot.child("GAS").getValue(Integer.class);
                textView3.setText(String.valueOf(gasCurrent));
                progressBar.setProgress(gasCurrent);
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {

            }
        });
    }

    //điều khiển quạt gió
    void controlWindFan(){
        ImageView imageView = findViewById(R.id.windfan);
        //đọc từ firebase, nếu giá trị quat = 1 thì đổi hình thành fan_on và ngược lại
        FireControl.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                int stateLight = snapshot.child("STATE_FAN_GAS").getValue(Integer.class);
                if(stateLight == 1)
                    imageView.setBackgroundResource(R.drawable.fan_on);

                else
                    imageView.setBackgroundResource(R.drawable.fan);
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {

            }
        });
    }

}
