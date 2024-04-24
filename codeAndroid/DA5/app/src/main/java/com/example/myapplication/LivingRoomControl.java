package com.example.myapplication;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class LivingRoomControl extends AppCompatActivity {
    ImageView toUser, toControl1, toControl2;
    int flag = 0;

    private ValueEventListener fanValueEventListener;

    FirebaseDatabase firebaseDatabase = FirebaseDatabase.getInstance();
    DatabaseReference FireSetting = firebaseDatabase.getReference().child("CSDL").child("SETTING_AUTO");
    DatabaseReference FireControl = firebaseDatabase.getReference().child("CSDL").child("CONTROL");
    DatabaseReference FireValue = firebaseDatabase.getReference().child("CSDL").child("VALUES");

    DatabaseReference fanStateReference = firebaseDatabase.getReference().child("CSDL").child("CONTROL").child("STATE_FAN_TRAN");
    DatabaseReference autoStateReference = firebaseDatabase.getReference().child("CSDL").child("CONTROL").child("AUTO_FAN");
    DatabaseReference appStateReference = firebaseDatabase.getReference().child("CSDL").child("CONTROL").child("FAN_APP");

    private Switch autoSwitch, controlSwitch;
    private ImageView fanImageView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.livingroom);

        getTempFromFire();
        goToUser();
        goToControl();
        pushDataMAXMINtofire();
        controlLight();

        autoSwitch = findViewById(R.id.autoSwitch);
        controlSwitch = findViewById(R.id.controlSwitch);
        fanImageView = findViewById(R.id.fan);

        // Lắng nghe sự kiện thay đổi trạng thái quạt từ Firebase
        fanStateReference.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot dataSnapshot) {
                int fanState = dataSnapshot.getValue(Integer.class);
                updateFanStateUI(fanState);
            }

            @Override
            public void onCancelled(@NonNull DatabaseError databaseError) {
                // Xử lý khi có lỗi kết nối Firebase
            }
        });

        // Lắng nghe sự kiện thay đổi trạng thái chế độ auto từ Firebase
        autoStateReference.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot dataSnapshot) {
                int autoState = dataSnapshot.getValue(Integer.class);
                autoSwitch.setChecked(autoState == 1);
            }

            @Override
            public void onCancelled(@NonNull DatabaseError databaseError) {
                // Xử lý khi có lỗi kết nối Firebase
            }
        });

        // Sự kiện khi nhấn nút switch mới
        controlSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (autoSwitch.isChecked()) {
                    // Nếu đang ở chế độ auto, cập nhật trạng thái trên Firebase
                    appStateReference.setValue(isChecked ? 1 : 0);
                }
            }
        });

        // Sự kiện khi nhấn switch chế độ auto
        autoSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {

                // Ẩn/hiện toggleButton tùy thuộc vào chế độ auto
                controlSwitch.setEnabled(!isChecked);

                // Nếu tắt chế độ auto, chỉ cập nhật trạng thái trên Firebase khi switch mới được nhấn
                if (isChecked) {
                    controlSwitch.setOnCheckedChangeListener(null);
                    controlSwitch.setChecked(false);
                    controlSwitch.setOnCheckedChangeListener(controlSwitchListener);
                    appStateReference.setValue(0);
                } else {
                    controlSwitch.setOnCheckedChangeListener(controlSwitchListener);
                }

                // Cập nhật trạng thái chế độ auto trên Firebase
                autoStateReference.setValue(isChecked ? 1 : 0);
            }
        });
    }

    // Sự kiện khi nhấn switch mới
    private CompoundButton.OnCheckedChangeListener controlSwitchListener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            appStateReference.setValue(isChecked ? 1 : 0);
        }
    };

    // Cập nhật giao diện dựa trên trạng thái quạt
    private void updateFanStateUI(int fanState) {
        if (fanState == 1) {
            fanImageView.setImageResource(R.drawable.fan_on);
        } else {
            fanImageView.setImageResource(R.drawable.fan);
        }
    }

    //chuyển trang sang thông tin user
    void goToUser() {
        toUser = findViewById(R.id.ad);
        toUser.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent mh3 = new Intent(LivingRoomControl.this, User.class);
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
                Intent mh = new Intent(LivingRoomControl.this, Control.class);
                startActivity(mh);
            }
        });
        toControl2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent mh = new Intent(LivingRoomControl.this, Control.class);
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
                        if(Integer.parseInt(val_max) <= 45 && Integer.parseInt(val_max) > 0){
                            FireSetting.child("FAN_ON").setValue(Integer.parseInt(val_max)); //đẩy thông tin nhiệt độ max lên fb
                            flag = 1;
                        }
                        else flag = 2;
                    }

                    if(val_min.length() != 0){
                        if(Integer.parseInt(val_min) <= 45 && Integer.parseInt(val_min) > 0){
                            FireSetting.child("FAN_OFF").setValue(Integer.parseInt(val_min)); //đẩy thông tin nhiệt độ min lên fb
                            flag = 1;
                        }
                        else flag = 2;
                    }
                }

                if(flag == 1)
                    Toast.makeText(LivingRoomControl.this,"LƯU THÀNH CÔNG", Toast.LENGTH_SHORT).show();
                else if(flag == 2)
                    Toast.makeText(LivingRoomControl.this,"SAI ĐỊNH DẠNG", Toast.LENGTH_SHORT).show();
                else if (flag == 3)
                    Toast.makeText(LivingRoomControl.this,"HÃY NHẬP THÔNG TIN", Toast.LENGTH_SHORT).show();

            }
        });

    }

    //lấy thông tin nhiệt độ từ fb
    void getTempFromFire(){
        TextView textView1 = findViewById(R.id.upperText);
        TextView textView2 = findViewById(R.id.lowerText);
        TextView textView3 = findViewById(R.id.temperatureText);
        ProgressBar progressBar = findViewById(R.id.temperatureProgressBar);

        FireSetting.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                int tempMax = snapshot.child("FAN_ON").getValue(Integer.class);
                int tempMin = snapshot.child("FAN_OFF").getValue(Integer.class);

                textView1.setText(String.valueOf(tempMax));
                textView2.setText(String.valueOf(tempMin));
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {

            }
        });

        FireValue.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                int tempCurrent = snapshot.child("TEMP").getValue(Integer.class);

                textView3.setText(String.valueOf(tempCurrent));
                progressBar.setProgress(tempCurrent);
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {

            }
        });
    }

    //điều khiển đèn
    void controlLight(){
        ImageView imageView = findViewById(R.id.light);

        //đọc từ firebase, nếu giá trị den = 1 thì đổi hình thành light_on và ngược lại
        FireControl.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                int stateLight = snapshot.child("STATE_DEN").getValue(Integer.class);
                if(stateLight == 1)
                    imageView.setBackgroundResource(R.drawable.light_on);
                else
                    imageView.setBackgroundResource(R.drawable.light_off);
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {

            }
        });
    }

}
