  #include <Seeed_Arduino_SSCMA.h>
  SSCMA AI;

  #define MOTOR_LEFT_ENABLE_PWM  D10
  #define MOTOR_LEFT_PHASE_DIR D9

  #define MOTOR_RIGHT_ENABLE_PWM  D8
  #define MOTOR_RIGHT_PHASE_DIR  D7

  void setup()
  {
    // Configure (set as output) and initialize (set as stopped) motor pins

    pinMode(MOTOR_LEFT_ENABLE_PWM, OUTPUT);
    analogWrite(MOTOR_LEFT_ENABLE_PWM, 0);

    pinMode(MOTOR_LEFT_PHASE_DIR, OUTPUT);
    digitalWrite(MOTOR_LEFT_PHASE_DIR, LOW);

    pinMode(MOTOR_RIGHT_ENABLE_PWM, OUTPUT);
    analogWrite(MOTOR_RIGHT_ENABLE_PWM, 0);

    pinMode(MOTOR_RIGHT_PHASE_DIR, OUTPUT);
    digitalWrite(MOTOR_RIGHT_PHASE_DIR, LOW);
    
    AI.begin();
      
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Face Tracking - Grove AI V2 / XIAO ");
    
  }

  void loop()
  {

    if (!AI.invoke(1, 0, true)){
          Serial.println("\nInvoke Success");
          Serial.print("Latency [ms]: prepocess=");
          Serial.print(AI.perf().prepocess);
          Serial.print(", inference=");
          Serial.print(AI.perf().inference);
          Serial.print(", postpocess=");
          Serial.println(AI.perf().postprocess);
          
      // AI.boxes() store the dimensions of the boxes that surround each face detected by the model
      if(AI.boxes().size() > 0)
          { 
              // we only track the first face detected for simplicity. #Potential TODO: Track the face with the highest confidence score
              int i =0;
              // the score is the confidence score of face detection 
              Serial.print("score=");
              Serial.print(AI.boxes()[i].score);
              // x and y represent the x and y coordinates of the center of the box surrounding the face. 
              Serial.print(", x=");
              int x = AI.boxes()[i].x;
              Serial.print(x);
              Serial.print(", y=");
              int y = AI.boxes()[i].y;
              Serial.print(y);
              // w is the width of the box surrounding the face
              Serial.print(", w=");
              Serial.print(AI.boxes()[i].w);
              // h is the height of the box surrounding the face
              Serial.print(", h=");
              Serial.println(AI.boxes()[i].h);
              
              
              delay(1000);
              // save image before moving the robot
              AI.save_jpeg();

              // the dimensions of the entire photo is 240 x 240
              // less than 100 means the face is on the left side of the image.
              if(x < 100){
                Serial.print("Moving Left \n");
                if(x <50){
                  left(30);
                  // moves for longer time when the face is in the far left side 
                  delay(1000);          
                  setMotorSpeed(0);
                }
                else{
                  left(30);
                  // moves for shorter time when the face is slightly left side 
                  delay(500);          
                  setMotorSpeed(0);
                }
              }
              
              // the dimensions of the entire photo is 240 x 240
              // more than 140 means the face is on the right side of the image.
              else if (x > 140){
                Serial.print("Moving Right \n");
                  if(x > 200){
                    right(30);
                    // moves for longer time when the face is in the far right side 
                    delay(1000);          
                    setMotorSpeed(0);
                  }
                  else{
                    right(30);
                    // moves for shorter time when the face is slightly right side 
                    delay(500);          
                    setMotorSpeed(0);
                  }
              }


              // saves image after the left/right movement
              AI.save_jpeg();  

              // the dimensions of the entire photo is 240 x 240
              // In the Y-dimension, the upper part of the photo is 0, and the lower part is 240
              // less than 100 means the detected face is on the upper side of the image.
              if(y < 100){
                Serial.print("Moving Backward \n");
                if(y <50){
                  setMotorSpeed(-30);
                  delay(1000);          
                  setMotorSpeed(0);
                }
                else{
                  setMotorSpeed(-30);
                  delay(500);          
                  setMotorSpeed(0);
                }
              }
              
              // more than 140  means the detected face is on the bottom side of the image.
              else if (y > 140){
                Serial.print("Moving Forward \n");
                  if(y > 200){
                    setMotorSpeed(30);
                    delay(1000);          
                    setMotorSpeed(0);
                  }
                  else{
                    setMotorSpeed(30);
                    delay(500);          
                    setMotorSpeed(0);
                  }
              }
              // saves image after finishing all movements
              AI.save_jpeg(); 
              Serial.print("Done Moving\n");
              delay(500);
          }

      }
      else{
        Serial.print("failure to invoke");
      }
  }





  void setMotorSpeed(int raw_speed)
  {

    // this is different from the original code https://github.com/anthonyjclark/cs181r/tree/main/_arduino/MotorControlMinimal
    // the left motor is reversed, so I reversed its HIGH/LOW here.
    if (raw_speed >= 0)
    {
      digitalWrite(MOTOR_LEFT_PHASE_DIR, LOW);
      digitalWrite(MOTOR_RIGHT_PHASE_DIR, HIGH);
    }
    else
    {
      digitalWrite(MOTOR_LEFT_PHASE_DIR, HIGH);
      digitalWrite(MOTOR_RIGHT_PHASE_DIR, LOW);
    }

    int speed = constrain(abs(raw_speed), 0, 100);
    int PWM = map(speed, 0, 100, 0, 255);

    analogWrite(MOTOR_LEFT_ENABLE_PWM, PWM);
    analogWrite(MOTOR_RIGHT_ENABLE_PWM, PWM);
  }



  void right(int raw_speed)
  {
    // moves the left motor only and stops the right motor

    // the left motor was reversed, so I reversed the speed
    raw_speed = raw_speed * -1;

    if (raw_speed >= 0)
    {
      digitalWrite(MOTOR_LEFT_PHASE_DIR, HIGH);
      digitalWrite(MOTOR_RIGHT_PHASE_DIR, LOW);
    }
    else
    {
      digitalWrite(MOTOR_LEFT_PHASE_DIR, LOW);
      digitalWrite(MOTOR_RIGHT_PHASE_DIR, LOW);
    }

    int speed = constrain(abs(raw_speed), 0, 100);
    int PWM = map(speed, 0, 100, 0, 255);
    int PWM_2 = map(0, 0, 100, 0, 255);


    analogWrite(MOTOR_LEFT_ENABLE_PWM, PWM);
    analogWrite(MOTOR_RIGHT_ENABLE_PWM, PWM_2);
  }


  void left(int raw_speed)
  {
    // Moves the right motor only and stops the left motor 
  if (raw_speed >= 0)
    {
      digitalWrite(MOTOR_LEFT_PHASE_DIR, LOW);
      digitalWrite(MOTOR_RIGHT_PHASE_DIR, HIGH);
    }
    else
    {
      digitalWrite(MOTOR_LEFT_PHASE_DIR, LOW);
      digitalWrite(MOTOR_RIGHT_PHASE_DIR, LOW);
    }

    int speed = constrain(abs(raw_speed), 0, 100);
    int PWM = map(speed, 0, 100, 0, 255);
    int PWM_2 = map(0, 0, 100, 0, 255);

    analogWrite(MOTOR_LEFT_ENABLE_PWM, PWM_2);
    analogWrite(MOTOR_RIGHT_ENABLE_PWM, PWM);
  }




