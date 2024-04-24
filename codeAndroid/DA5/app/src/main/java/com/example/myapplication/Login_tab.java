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

public class Login_tab extends AppCompatActivity{
    TextView tSu, forgotPass;
    Button ButtonLogin;
    EditText Email, Password;
    FirebaseAuth auth;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.login_tab);

        auth = FirebaseAuth.getInstance();
        //nếu đăng nhập rồi thì vào thẳng trang điều khiển
        if (auth.getCurrentUser() != null) {
            Toast.makeText(Login_tab.this, "Tự động đăng nhập" , Toast.LENGTH_SHORT).show();
            startActivity(new Intent(Login_tab.this, Control.class));
            finish();
        }

        goToSignUp();
        LogIn();
        ForgotPassword();
    }

    //chuyển trang sang đăng ký
    void goToSignUp(){
        tSu = findViewById(R.id.toSignup);
        tSu.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view) {
                Intent mh2 = new Intent(Login_tab.this, Signup_tab.class);
                startActivity(mh2);
            }
        });
    }

    //ấn nút đăng nhập
    void LogIn(){
        ButtonLogin = (Button) findViewById(R.id.buttonLogin);
        Email = (EditText) findViewById(R.id.email);
        Password = (EditText) findViewById(R.id.pass);
        ButtonLogin.setOnClickListener(new View.OnClickListener() {
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

                //đăng nhập bằng email và mật khẩu
                auth.signInWithEmailAndPassword(email, password)
                        .addOnCompleteListener(Login_tab.this, new OnCompleteListener<AuthResult>() {
                            @Override
                            public void onComplete(@NonNull Task<AuthResult> task) {
                                if (task.isSuccessful()) {
                                    Toast.makeText(Login_tab.this, "Đăng nhập thành công!",
                                            Toast.LENGTH_LONG).show();
                                    Intent intent = new Intent(Login_tab.this, Control.class);
                                    startActivity(intent);
                                    finishAffinity();
                                    // there was an error
                                } else {
                                    Toast.makeText(Login_tab.this, "Tài khoản hoặc mật khẩu sai!",
                                            Toast.LENGTH_LONG).show();
                                }
                            }
                        });

            }
        });
    }
    //forgot pass
    void ForgotPassword(){
        forgotPass = findViewById(R.id.forget_pass);
        forgotPass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view) {
                String emailAddress = "quynhanhdo07052002@gmail.com";

                auth.sendPasswordResetEmail(emailAddress)
                        .addOnCompleteListener(new OnCompleteListener<Void>() {
                            @Override
                            public void onComplete(@NonNull Task<Void> task) {
                                if (task.isSuccessful()) {
                                    Toast.makeText(Login_tab.this, "Đã gửi mật khẩu tới Email",
                                            Toast.LENGTH_SHORT).show();
                                }
                                else {
                                    Toast.makeText(Login_tab.this, "Gửi email thất bại!",
                                            Toast.LENGTH_SHORT).show();
                                }
                            }
                        });
            }
        });
    }
}
