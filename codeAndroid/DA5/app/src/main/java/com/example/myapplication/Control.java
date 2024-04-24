package com.example.myapplication;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class Control extends AppCompatActivity{
    ImageView toLivingRoom, toKitchen, toUser;

    private ImageView doorImageView;
    private Switch alertSwitch;
    private DatabaseReference doorReference;
    private ValueEventListener doorValueEventListener;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.control);

        goToLivingRoom();
        goToKitchen();
        goToUser();

        doorImageView = findViewById(R.id.door);
        alertSwitch = findViewById(R.id.switchButton);

        doorReference = FirebaseDatabase.getInstance().getReference().child("CSDL").child("CONTROL").child("STATE_DOOR");

        // Set initial door state
        updateDoorImage();

        // Set switch listener
        alertSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                // Remove previous ValueEventListener to avoid duplicate listeners
                if (doorValueEventListener != null) {
                    doorReference.removeEventListener(doorValueEventListener);
                }

                // Listen for changes in Firebase "door" child only if the switch is ON
                if (isChecked) {
                    doorValueEventListener = new ValueEventListener() {
                        @Override
                        public void onDataChange(@NonNull DataSnapshot dataSnapshot) {
                            // Get the value of "door" from Firebase
                            int firebaseSignal = dataSnapshot.getValue(Integer.class);

                            // If switch is ON, show toast and update door image
                            showToastAndChangeImage(firebaseSignal);
                        }

                        @Override
                        public void onCancelled(@NonNull DatabaseError databaseError) {
                            // Handle errors
                        }
                    };
                    doorReference.addValueEventListener(doorValueEventListener);
                }
                else{
                    doorValueEventListener = new ValueEventListener() {
                        @Override
                        public void onDataChange(@NonNull DataSnapshot dataSnapshot) {
                            // Get the value of "door" from Firebase
                            // If switch is ON, just update door image
                            updateDoorImage();
                        }

                        @Override
                        public void onCancelled(@NonNull DatabaseError databaseError) {
                            // Handle errors
                        }
                    };
                    doorReference.addValueEventListener(doorValueEventListener);
                }
            }
        });
    }
    private void showToastAndChangeImage(int firebaseSignal) {
        // Show toast based on firebase signal
        if (firebaseSignal == 1) {
            showToast("Cửa mở! Có người đột nhập!!!");
        } else {
        }
        // Update door image
        updateDoorImage();
    }

    private void updateDoorImage() {
        // Update door image based on Firebase signal and switch state
        doorReference.addListenerForSingleValueEvent(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot dataSnapshot) {
                // Get the value of "door" from Firebase
                int firebaseSignal = dataSnapshot.getValue(Integer.class);

                if (alertSwitch.isChecked()) {
                    // If switch is ON, update image and show toast
                    if (firebaseSignal == 1) {
                        doorImageView.setImageResource(R.drawable.doop_open);
                    } else {
                        doorImageView.setImageResource(R.drawable.doop_close);
                    }
                } else {
                    // If switch is OFF, only update image
                    doorImageView.setImageResource(firebaseSignal == 1 ? R.drawable.doop_open : R.drawable.doop_close);
                }
            }

            @Override
            public void onCancelled(@NonNull DatabaseError databaseError) {
                // Handle errors
            }
        });
    }

    private void showToast(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }

    //chuyển trang sang điều khiển các phòng
    void goToLivingRoom(){
        toLivingRoom = findViewById(R.id.livingroom);
        toLivingRoom.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent mh2 = new Intent(Control.this, LivingRoomControl.class);
                startActivity(mh2);
            }
        });
    }

    void goToKitchen(){
        toKitchen = findViewById(R.id.kitchen);
        toKitchen.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent mh2 = new Intent(Control.this, KitchenControl.class);
                startActivity(mh2);
            }
        });
    }

    //chuyển trang sang thông tin user
    void goToUser() {
        toUser = findViewById(R.id.ad);
        toUser.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent mh3 = new Intent(Control.this, User.class);
                startActivity(mh3);
            }
        });
    }
}