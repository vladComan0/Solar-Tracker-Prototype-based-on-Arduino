#include <Servo.h>
#include <Wire.h> // this is mandatory in order to work with the LiquidCrystal_I2C library
#include <LiquidCrystal_I2C.h> // the library for the I2C LCD, I chose to use this because it only takes 2 analog pins from arduino and because I had some problems while trying to solder the normal LCDs
// pin assignments for the analog reads
const int pinR1 = A0, pinR2 = A1, pinR3 = A2, pinR4 = A3;
// Creating the lcd_i2c object
LiquidCrystal_I2C lcd(0x20, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
// variables for storing the actual voltages (mapped)
float vinR1 = 0, vinR2, vinR3, vinR4, vldr1 = 0, vldr2 = 0, vldr3 = 0, vldr4 = 0;
// variables for storing the analogRead values (0, 1023)
float vin1 = 0, vin2 = 0, vin3 = 0, vin4 = 0;

// pin assignment for the servo signals
const int servoPinX = 6, servoPinY = 5;
// variables for storing the angles
int angleX = 0, angleY = 0;
// variables for setting the limits of the servos
int servoXmax = 160, servoXmin = -20;
int servoYmax = 90, servoYmin = 0;
// tolerance level variable
int tolerance = 10;
// creating the Servo objects for the horizontal and vertical servo motors
Servo servoX, servoY;
void setup() {
  Serial.begin(9600); // Begin the communication at 9600 baud
  lcd.begin(16, 2); //Begin the transmission to the LCD display
  lcd.clear(); // make sure the LCD is cleared
  lcd.print("Initialising..."); // alert the user that the tracker is about to start
  pinMode(pinR1, INPUT);  // Setup the pins accordingly
  pinMode(pinR2, INPUT);
  pinMode(pinR3, INPUT);
  pinMode(pinR4, INPUT);
  servoX.attach(servoPinX); //attach to the Servo object servoX the signal pin from the horizontal servo-motor
  servoY.attach(servoPinY); //attach to the Servo object servoY the signal pin from the vertical servo-motor
  servoX.write(60); // the initial value of the horizontal servo motor (doesn't have to be this one)
  servoY.write(45); // the initial value of the vertical servo motor (doesn't have to be this one)
  Serial.println(servoX.read()); //print the value in case Proteus doesn't animate correctly the servo-motors
  Serial.println(servoY.read());
  delay(2000); // Some intentional delay for initialization (not properly needed but to expect the tracker to move)
  lcd.clear(); // clear the lcd before entering the loop
}

void loop() {
  //As a remark, there is no need to convert the values into voltage, because the ratios remain the same,
  //and we only need the average. But in order to display them on the LCD display, I also converted them into voltages.
  vin1 = analogRead(pinR1); //we read the value of the voltage from the resistor R1 (0,1023)
  vinR1 = (vin1 / 1023) * 5; //the voltage across the resistor R1
  vldr1 = 5 - vinR1; // the voltage across the ldr-X1
  lcd.setCursor(1, 0); // set the cursor to the begining
  lcd.print("V1=");
  lcd.print(vldr1); //We print the value of the voltage on the ldr 1
  lcd.print("V");
  vin2 = analogRead(pinR2); //we read the value of the voltage from the resistor R2 (0,1023)
  vinR2 = (vin2 / 1023) * 5; //the voltage across the resistor
  vldr2 = 5 - vinR2; // the voltage across the ldr
  lcd.setCursor(9, 0); // set the cursor to the proper column
  lcd.print("V2=");
  lcd.print(vldr2); //We print the value of the voltage on the ldr 2
  lcd.print("V");
  vin3 = analogRead(pinR3); //we read the value of the voltage from the resistor R3 (0,1023)
  vinR3 = (vin3 / 1023) * 5; //the voltage across the resistor
  vldr3 = 5 - vinR3; // the voltage across the ldr
  lcd.setCursor(1, 1); // set the cursor to the begining on the second row
  lcd.print("V3=");
  lcd.print(vldr3); //We print the value of the voltage on the ldr 3
  lcd.print("V");
  vin4 = analogRead(pinR4); //we read the value of the voltage from the resistor R4 (0,1023)
  vinR4 = (vin4 / 1023) * 5; //the voltage across the resistor
  vldr4 = 5 - vinR4; // the voltage across the ldr
  lcd.setCursor(9, 1); // set the cursor to the proper column on the second row
  lcd.print("V4=");
  lcd.print(vldr4); //We print the value of the voltage on the ldr 4
  lcd.print("V");


  //The servo motors movement part
  //As a method, I will obtain 4 averages: top, bottom, left, right

  float avgtopf = (vldr1 + vldr2) / 2; // ldr1 represents the top left one, while ldr2 represents the top right one
  int avgtop = (int)(avgtopf * 100); // leave two digits after the comma and move to integer because it is better to work with integer in comparisons than to work with floating point
  float avgbottomf = (vldr3 + vldr4) / 2; // ldr3 represents the bottom left one, while ldr4 represents the bottom right one
  int avgbottom = (int)(avgbottomf * 100); // leave two digits after the comma and move to integer because it is better to work with integer in comparisons than to work with floating point
  float avgleftf = (vldr1 + vldr3) / 2; // ldr1 represents the top left one, while ldr3 represents the bottom left one
  int avgleft = (int)(avgleftf * 100); // leave two digits after the comma and move to integer because it is better to work with integer in comparisons than to work with floating point
  float avgrightf = (vldr2 + vldr4) / 2; // ldr2 represents the top right one, while ldr4 represents the bottom right one
  int avgright = (int)(avgrightf * 100); // leave two digits after the comma and move to integer because it is better to work with integer in comparisons than to work with floating point

  //    The horizontal movement
  if (abs(avgleft - avgright) > tolerance) { // if it is in the tolerance region than it is ok, the tracker should stabilize
    if (avgleft > avgright) { // this means that the light intensity is greater on the right side
      //here is a compensation try in order for Proteus to simulate the adaptation to light intensity but it doesn't work properly because it reads the correct values again every loop
      //        if (vldr1 > vldr2){
      //          vldr1 -= (vldr1-vldr2);
      //        }
      //        else if (vldr3 > vldr4){
      //          vldr3 -= (vldr3-vldr4);
      //        }
      angleX += 2; // so we tell the horizontal servo motor to slowly move to the optimal position
      if (angleX > servoXmax) { // make sure it is in the desired range
        angleX = servoXmax; // if greater, than it takes the maximum allowed value
      }
    }
    else if (avgright > avgleft) { // this means that the light intensity is greater on the left side
      //here is a compensation try in order for Proteus to simulate the adaptation to light intensity but it doesn't work properly because it reads the correct values again every loop
      //        if (vldr2 > vldr1){
      //          vldr2 -= (vldr2-vldr1);
      //        }
      //        else if (vldr4 > vldr3){
      //          vldr4 -= (vldr4-vldr3);
      //        }
      angleX -= 2; // so we tell the horizontal servo motor to slowly move to the optimal position
      if (angleX < servoXmin) { // make sure it is in the desired range
        angleX = servoXmin; // if smaller, than it takes the minimum allowed value
      }
    }
  }
  servoX.write(angleX);  // Write the current angle on the signal pin of the horizontal servo-motor

  // The vertical movement
  if (abs(avgtop - avgbottom) > tolerance) { // if it is in the tolerance region than it is ok, the tracker should stabilize
    if (avgtop > avgbottom) { // this means that the light intensity is greater on the bottom side
      //here is a compensation try in order for Proteus to simulate the adaptation to light intensity but it doesn't work properly because it reads the correct values again every loop
      //    if (vldr1 > vldr3){
      //      vldr1 -= (vldr1-vldr2);
      //    }
      //    else if (vldr2 > vldr4){
      //      vldr2 -= (vldr2-vldr4);
      //    }
      angleY -= 2; // so we tell the vertical servo motor to slowly move to the optimal position
      if (angleY < servoYmin) { // make sure it is in the desired range
        angleY = servoYmin; // if smaller, than it takes the minimum allowed value
      }
    }
    else if (avgbottom > avgtop) { // this means that the light intensity is greater on the top side
      ////here is a compensation try in order for Proteus to simulate the adaptation to light intensity but it doesn't work properly because it reads the correct values again every loop
      //      if (vldr3 > vldr1){
      //        vldr3 -= (vldr3-vldr1);
      //      }
      //      else if (vldr4 > vldr2){
      //        vldr4=vldr2;
      //      }
      angleY += 2; // so we tell the vertical servo motor to slowly move to the optimal position
      if (angleY > servoYmax) { // make sure it is in the desired range
        angleY = servoYmax; // if greater, than it takes the maximum allowed value
      }
    }
  }
  servoY.write(angleY);  // Write the current angle on the signal pin of the vertical servo-motor

  Serial.print("Avg top: ");
  Serial.print(avgtop); // print the top average for maintenance
  Serial.println();
  Serial.print("Avg bottom: ");
  Serial.print(avgbottom); // print the bottom average for maintenance
  Serial.println();
  Serial.print("Avg left: ");
  Serial.print(avgleft); // print the left average for maintenance
  Serial.println();
  Serial.print("Avg right: ");
  Serial.print(avgright); // print the right average for maintenance
  Serial.println();
  Serial.print("VLDR1: ");

  // print the voltages across the LDRs also for maintenance
  Serial.print(vldr1);
  Serial.println();
  Serial.print("VLDR2: ");
  Serial.print(vldr2);
  Serial.println();
  Serial.print("VLDR3: ");
  Serial.print(vldr3);
  Serial.println();
  Serial.print("VLDR4: ");
  Serial.print(vldr4);
  Serial.println();
  // also print the current angles

  Serial.print(angleX);
  Serial.println();
  Serial.print(angleY);
  Serial.println();
  //delay(10); for maintenance just apply the desired delay
}
