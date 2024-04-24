package com.example.myapplication;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.FirebaseUser;
import com.google.firebase.auth.UserProfileChangeRequest;

public class User extends AppCompatActivity{
    private ImageView toControl2, toControl;
    private Button btnLogOut, btnUpdate;
    private TextView tvEmail, adText;
    private EditText edtName;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.user);

        goToControl();
        ShowUserInformation();
        LogOut();
    }
    //chuyển trang về điều khiển
    void goToControl(){
        toControl = findViewById(R.id.home);
        toControl2 = findViewById(R.id.back_to_ctrl);
        toControl.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent mh = new Intent(User.this, Control.class);
                startActivity(mh);
            }
        });
        toControl2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent mh2 = new Intent(User.this, Control.class);
                startActivity(mh2);
            }
        });
    }

    //get thông tin email từ Fb về để hiển thị
    void ShowUserInformation(){
        tvEmail = findViewById(R.id.tvEmail);
        FirebaseUser user = FirebaseAuth.getInstance().getCurrentUser();
        if (user == null){
            return;
        }
        String email = user.getEmail();
        tvEmail.setText(email);

        UpDateUserInformation();
        edtName.setText(user.getDisplayName());
        adText.setText(user.getDisplayName());
    }
    //cập nhật tên người dùng
    void UpDateUserInformation(){
        btnUpdate = findViewById(R.id.btn_update);
        edtName = findViewById(R.id.editName);
        adText = findViewById(R.id.adText);
        btnUpdate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FirebaseUser user = FirebaseAuth.getInstance().getCurrentUser();
                if (user == null){
                    return;
                }
                String strName = edtName.getText().toString().trim();
                UserProfileChangeRequest profileUpdates = new UserProfileChangeRequest.Builder()
                        .setDisplayName(strName).build();

                String strName2 = adText.getText().toString().trim();
                UserProfileChangeRequest profileUpdates2 = new UserProfileChangeRequest.Builder()
                        .setDisplayName(strName2).build();

                user.updateProfile(profileUpdates).addOnCompleteListener(new OnCompleteListener<Void>() {
                    @Override
                    public void onComplete(@NonNull Task<Void> task) {
                        if (task.isSuccessful()){
                        }else {
                        }
                    }
                });

                user.updateProfile(profileUpdates).addOnCompleteListener(new OnCompleteListener<Void>() {
                    @Override
                    public void onComplete(@NonNull Task<Void> task) {
                        if (task.isSuccessful()){
                            Toast.makeText(User.this, "Cập nhật thành công!",
                                    Toast.LENGTH_LONG).show();
                        }else {
                            Toast.makeText(User.this, "Cập nhật thất bại!",
                                    Toast.LENGTH_LONG).show();
                        }
                    }
                });
            }
        });
    }

    //nhấn đăng xuất
    void LogOut(){
        btnLogOut = findViewById(R.id.log_out);
        btnLogOut.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FirebaseAuth.getInstance().signOut();
                Toast.makeText(User.this, "Đăng xuất", Toast.LENGTH_LONG).show();
                Intent mh2 = new Intent(User.this, Login_tab.class);
                startActivity(mh2);
            }
        });
    }
}
