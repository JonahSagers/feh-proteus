#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHIO.h>
#include <FEHLCD.h>
#include <FEHRCS.h>
#include <stdlib.h>
#include <math.h>
#include <FEHServo.h>

# define PI 3.141592

// declare motor on motor port 0, set maximum voltage to 9.0 V
//FEHMotor backMotor(FEHMotor::Motor0,9.0);

FEHMotor backMotor(FEHMotor::Motor2,9.0);
FEHMotor rightMotor(FEHMotor::Motor1,9.0);
FEHMotor leftMotor(FEHMotor::Motor3,9.0);

FEHMotor forklift(FEHMotor::Motor0, 9.0); 

//DigitalEncoder backShaft(FEHIO::P3_7);
DigitalEncoder backShaft(FEHIO::P2_5);
DigitalEncoder rightShaft(FEHIO::P2_6);
DigitalEncoder leftShaft(FEHIO::P2_7);

AnalogInputPin cdsCell(FEHIO::P0_0);
AnalogInputPin leftOpto(FEHIO::P1_0);
AnalogInputPin rightOpto(FEHIO::P1_7);

DigitalInputPin forkLimitUp(FEHIO::P3_1);   //active low
DigitalInputPin forkLimitDown(FEHIO::P0_3); //active high

float forkliftPos;

void TurnRight(int reverse)
{
    leftShaft.ResetCounts();
    rightShaft.ResetCounts();
    leftMotor.SetPercent(-25 * reverse);
    rightMotor.SetPercent(25 * reverse);
    while(leftShaft.Counts() < 222.6 && rightShaft.Counts() < 222.6){}
    leftMotor.Stop();
    rightMotor.Stop();
}

float ToRadian(float degree){
    return degree * (PI/180);
}

void Drive(float distance, float speedBack, float speedRight, float speedLeft, float timeout){

    backShaft.ResetCounts();
    rightShaft.ResetCounts();
    leftShaft.ResetCounts();

    float backDist, rightDist, leftDist, avgTraveled, backPercent, rightPercent, leftPercent, numMoving, speedMult;
    if(speedBack != 0){
        backDist = (48.89 * distance)/(100/abs(speedBack));
    } else {
        backDist = 0;
        backMotor.Stop();
    }
    if(speedRight != 0){
        rightDist = (48.89 * distance)/(100/abs(speedRight));
    } else {
        rightDist = 0;
        rightMotor.Stop();
    }
    if(speedLeft != 0){
        leftDist = (48.89 * distance)/(100/abs(speedLeft));
    } else {
        leftDist = 0;
        leftMotor.Stop();
    }

    //speedBack *= 1.02;
    // speedRight *= 1;
    speedLeft *= 0.98;
    if(backDist != 0){
        backMotor.SetPercent(speedBack);
    }
    rightMotor.SetPercent(speedRight);
    leftMotor.SetPercent(speedLeft);
    ResetTime();
    //encoder counts per inch: 48.89
    while((backShaft.Counts() < backDist || (
            rightShaft.Counts() < rightDist || 
            leftShaft.Counts() < leftDist)) && TimeNow() < timeout ){
        numMoving = 0;
        if(speedMult < 1){
            speedMult += 0.02;
        }
        if(avgTraveled > 0.93){
            speedMult = 1 - (avgTraveled - 0.93) * 8;
        }
        if(backDist != 0){
            backPercent = backShaft.Counts()/backDist;
            numMoving += 1;
        } else {
            backPercent = 0;
        }
        if(rightDist != 0){
            rightPercent = (rightShaft.Counts() * 0.95)/rightDist;
            numMoving += 1;
        } else {
            rightPercent = 0;
        }
        if(leftDist != 0){
            leftPercent = leftShaft.Counts()/leftDist;
            numMoving += 1;
        } else {
            leftPercent = 0;
        }
        avgTraveled = (backPercent + leftPercent + rightPercent)/numMoving;

        backMotor.SetPercent(speedBack * (1 + (avgTraveled - backPercent) * distance) * speedMult);
        rightMotor.SetPercent(speedRight * (1 + (avgTraveled - rightPercent) * distance) * speedMult);
        leftMotor.SetPercent(speedLeft * (1 + (avgTraveled - leftPercent) * distance) * speedMult);
        // if(TimeNow() > 0.5){
        //     LCD.Clear();
        //     LCD.Write("Back Motor: ");
        //     LCD.WriteLine(backPercent);
        //     LCD.Write("Left Motor: ");
        //     LCD.WriteLine(leftPercent);
        //     LCD.Write("Right Motor: ");
        //     LCD.WriteLine(rightPercent);
        //     ResetTime();
        // }
        Sleep(2);
    }
    backMotor.Stop();
    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(50);
    //this shouldn't be required, but it fixes issues
    backMotor.Stop();
    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(50);
}

void FindLine(int direction, float low, float high)
{
    backMotor.SetPercent(10 * direction);
    leftMotor.SetPercent(10 * direction);
    rightMotor.SetPercent(10 * direction);
    if(direction > 0){
        while(leftOpto.Value() < low || leftOpto.Value() > high ){};
    } else {
        while(rightOpto.Value() < low || rightOpto.Value() > high ){};
    }
    backMotor.Stop();
    leftMotor.Stop();
    rightMotor.Stop();
}

void Lift(float distance)
{
    forklift.SetPercent(100 * (distance/abs(distance)));
    float msec = 0;
    while((msec / 1000 < abs(distance)) && (forkLimitDown.Value() < 1 || distance > 0) && (forkLimitUp.Value() > 0 || distance < 0)){
        Sleep(10);
        msec += 10;
    }
    // LCD.Clear();
    // LCD.WriteLine(msec / 1000);
    // LCD.Write("Top: ");
    // LCD.WriteLine(forkLimitUp.Value());
    // LCD.Write("Bot: ");
    // LCD.WriteLine(forkLimitDown.Value());
    forklift.Stop();
    Sleep(50);
}

void LiftBottom()
{
    forklift.SetPercent(-60);
    while(forkLimitDown.Value() > 0) {}
    forklift.SetPercent(0);
}

int main()
{
    Sleep(2);
    //backwards
    //Drive(3, 0, 40, -40, 10);

    //forwards
    //Drive(3, 0, -40, 40, 10);
    //back, right, left
    
    Drive(3, 0, 20, -20, 1.5);
    Drive(3, 0, -20, 20, 2);
    Drive(4.8, -40, -40, -40, 10);
    Drive(13.2, 0, -40, 40, 1);
    Drive(7, 40, -40, 0, 10);
    for(int i = 0; i < 3; i++){
        Lift(-0.8);
        Drive(3, 0, 40, -40, 2);
        Lift(0.8);
        Drive(3.2, 0, -40, 40, 2);
    }
    //should be -0.6 if doing bonus
    Lift(-0.48);
    Drive(3, 0, 40, -40, 2);
    //Lift(-0.4);
    // Drive(3.5, 0, -40, 40, 2);
        // for(int i = 0; i < 3; i++){
        //     Lift(0.8);
        //     Drive(3, 0, 40, -40, 2);
        //     Lift(-0.8);
        //     Drive(3.5, 0, -40, 40, 2);
        // }

    // Drive(3, 0, 40, -40, 2);
    //Lift(0.45);
    Drive(9, 40, 40, 40, 3);
    Drive(30, 0, -40, 40, 5);
    Drive(8.5, -40, -40, -40, 3);
    Drive(19, 0, -20, 20, 4);
    Lift(0.3);
    Drive(38.5, 0, 43, -37, 5);
    Drive(9.5, 40, 40, 40, 3);
    Drive(30, 0, -60, 60, 5);
    Lift(1.25);
    Drive(40, 0, -39, 41, 4);
    Lift(-0.3);

    Drive(26, 35, 55, -35, 10);
    Lift(-0.7);
    Drive(9.5, 40, 40, 40, 3);
    Drive(8, 0, -40, 40, 10);
    Drive(10, -84, 80 * cos(ToRadian(60)) + 4, 80 * cos(ToRadian(60)) + 4, 3);

    Drive(15, 18, -42, -42, 10);
    Drive(4, 40, -40 * cos(ToRadian(60)), -40 * cos(ToRadian(60)), 2);
    Drive(15, 0, 40, -40, 4);
    
    Drive(3, 0, -40, 40, 3);
    Drive(17, -40, -40, -40, 3);
    Drive(15, -40, 40 * cos(ToRadian(60)), 40 * cos(ToRadian(60)), 2);
    Lift(0.2);
    Drive(12, 0, -40, 40, 10);
    Lift(-0.6);
    Drive(5, 0, 40, -40, 10);
    Lift(-0.4);
    Drive(5, 0, -40, 40, 10);
    Lift(0.6);

    Drive(45, 0, 40, -40, 10);
    Drive(4, 40, 40, 40, 3);
    Drive(70, 0, 40, -40, 10);
}

// Lift(0.6);
// Drive(3, -40 * cos(ToRadian(30)), 40, 40, 10);
// Lift(-0.6);
// Drive(3, 40 * cos(ToRadian(30)), -40, -40, 10);