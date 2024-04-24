package com.example.myapplication;

import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.auth.AuthResult;
import com.google.firebase.auth.FirebaseAuth;

public class Signup_tab extends AppCompatActivity{
    private EditText Email, Password;
    private Button ButtonSignup;
    private FirebaseAuth auth;
    TextView tLi;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.signup_tab);

        goToLogIn();
        SignUp();
    }

    //chuyển trang từ đăng ký sang đăng nhập
    void goToLogIn(){
        tLi = findViewById(R.id.toLogin);
        tLi.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view) {
                Intent mh2 = new Intent(Signup_tab.this, Login_tab.class);
                startActivity(mh2);
            }
        });
    }
    //đăng ký
    void SignUp(){
        auth = FirebaseAuth.getInstance();
        ButtonSignup = (Button) findViewById(R.id.buttonSignup);
        Email = (EditText) findViewById(R.id.email);
        Password = (EditText) findViewById(R.id.pass);

        ButtonSignup.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                String email = Email.getText().toString().trim();
                String password = Password.getText().toString().trim();

                //kiểm tra email
                if (TextUtils.isEmpty(email)) {
                    Toast.makeText(getApplicationContext(), "Hãy nhập email!", Toast.LENGTH_SHORT).show();
                    return;
                }

                //kiểm tra mật khẩu
                if (TextUtils.isEmpty(password)) {
                    Toast.makeText(getApplicationContext(), "Hãy nhập mật khẩu!", Toast.LENGTH_SHORT).show();
                    return;
                }

                if (password.length() < 6) {
                    Toast.makeText(getApplicationContext(), "Mật khẩu quá ngắn, cần có ít nhất 6 ký tự!",
                            Toast.LENGTH_SHORT).show();
                    return;
                }

                //create user
                auth.createUserWithEmailAndPassword(email, password)
                        .addOnCompleteListener(Signup_tab.this, new OnCompleteListener<AuthResult>() {

                            @Override
                            public void onComplete(@NonNull Task<AuthResult> task) {

                                if (!task.isSuccessful()) {
                                    Toast.makeText(Signup_tab.this, "Email sai hoặc đã được đăng ký!",
                                            Toast.LENGTH_SHORT).show();
                                } else {
                                    Toast.makeText(Signup_tab.this, "Đăng ký thành công!",
                                            Toast.LENGTH_SHORT).show();
                                    startActivity(new Intent(Signup_tab.this, Login_tab.class));
                                    finishAffinity();
                                }
                            }
                        });

            }
        });
    }
}
