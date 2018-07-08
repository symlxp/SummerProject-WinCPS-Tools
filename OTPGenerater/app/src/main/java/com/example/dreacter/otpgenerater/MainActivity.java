package com.example.dreacter.otpgenerater;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.util.Date;

public class MainActivity extends AppCompatActivity {

    public Button okButton = null;
    public EditText userCode = null;
    public TextView PINCode = null;

    public String userSecret = "34567898765432456789876543456aa7a786a";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        okButton = findViewById(R.id.okButton);
        userCode = findViewById(R.id.userCode);
        PINCode = findViewById(R.id.PINCode);

        okButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String userInputCode = userCode.getText().toString();
                Date date = new Date();
                long currentTime = date.getTime();
                currentTime /= 30000;
                String firstSHA1 = SHA1.encode(userSecret + String.valueOf(currentTime) + userInputCode);
                String secondSHA1 = SHA1.encode(userSecret+firstSHA1);
                PINCode.setText(secondSHA1.toCharArray(),0,8);
            }
        });

    }


}
