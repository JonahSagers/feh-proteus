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

FEHMotor backMotor(FEHMotor::Motor1,9.0);
FEHMotor rightMotor(FEHMotor::Motor0,9.0);
FEHMotor leftMotor(FEHMotor::Motor2,9.0);

FEHMotor forklift(FEHMotor::Motor3, 9.0); 

//DigitalEncoder backShaft(FEHIO::P3_7);
DigitalEncoder backShaft(FEHIO::P2_5);
DigitalEncoder rightShaft(FEHIO::P2_6);
DigitalEncoder leftShaft(FEHIO::P2_7);

AnalogInputPin cdsCell(FEHIO::P0_0);

int left_count = 0;
int right_count = 0;

bool left_latch;
bool right_latch;

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

    float backDist, rightDist, leftDist, avgTraveled, backPercent, rightPercent, leftPercent, numMoving;
    float backError, rightError, leftError, backErrorSum, rightErrorSum, leftErrorSum, backErrorLast = 0, rightErrorLast = 0, leftErrorLast = 0;
    float speedBackCurrent = speedBack, speedRightCurrent = speedRight, speedLeftCurrent = speedLeft;
    float p = 1, i = 1, d = 1;
    backDist = (48.89 * distance)/(100/abs(speedBack));
    rightDist = (48.89 * distance)/(100/abs(speedRight));
    leftDist = (48.89 * distance)/(100/abs(speedLeft));

    //speedBack *= 1.02;
    // speedRight *= 1;
    speedLeft *= 0.98;

    backMotor.SetPercent(speedBack);
    rightMotor.SetPercent(speedRight);
    leftMotor.SetPercent(speedLeft);
    ResetTime();
    //encoder counts per inch: 48.89
    while((backShaft.Counts() < backDist || (
            rightShaft.Counts() < rightDist || 
            leftShaft.Counts() < leftDist)) && TimeNow() < timeout ){
        numMoving = 0;
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
        backErrorLast = backError;
        rightErrorLast = rightError;
        leftErrorLast = leftError;
        avgTraveled = (backPercent + leftPercent + rightPercent)/numMoving;
        backError = (avgTraveled * backDist) - backShaft.Counts();
        rightError = rightShaft.Counts() - (avgTraveled * rightDist);
        leftError = leftShaft.Counts() - (avgTraveled * leftDist);
        backErrorSum += backError;
        rightErrorSum += rightError;
        leftErrorSum += leftError;

        speedBackCurrent += backError * p + backErrorSum * i + (backError - backErrorLast) * d;
        speedRightCurrent += rightError * p + rightErrorSum * i + (rightError - rightErrorLast) * d;
        speedLeftCurrent += leftError * p + leftErrorSum * i + (leftError - leftErrorLast) * d;

        backMotor.SetPercent(speedLeftCurrent);
        rightMotor.SetPercent(speedRightCurrent);
        leftMotor.SetPercent(speedLeftCurrent);
        if(TimeNow() > 0.1){
            LCD.Clear();
            LCD.Write("Back Motor: ");
            LCD.WriteLine(backPercent);
            LCD.Write("Left Motor: ");
            LCD.WriteLine(leftPercent);
            LCD.Write("Right Motor: ");
            LCD.WriteLine(rightPercent);
            ResetTime();
        }
    }
    backMotor.Stop();
    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(1);
    //this shouldn't be required, but it fixes issues
    backMotor.Stop();
    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(49);
}

int main()
{
    //Drive(3, 0, 20, -20, 2);
    Drive(100, 0, -20, 20, 2);

    // //while(cdsCell.Value() > 0.7){}
    
    //Drive(35, 0, -40, 40, 10);
    // Drive(5, -40, -40, -40, 10);


    // Drive(5, -40, 0, 40, 10);
    // Drive(66, 0, -40, 40, 10);
    // Drive(23, 40, 40, 40, 10);
    // Drive(20, -40, 40 * cos(ToRadian(60)), 40 * cos(ToRadian(60)), 9999);
    // Drive(7, 0, -40, 40, 10);
    // Drive(15, -80, 80 * cos(ToRadian(60)), 80 * cos(ToRadian(60)), 9999);
    // // Drive(7, 0, 40, -40, 10);
    // // Drive(2, -40, 40 * cos(ToRadian(60)), 40 * cos(ToRadian(60)), 9999);
    // Drive(2, 0, -40, 40, 10);
    // Drive(30, 80, -80 * cos(ToRadian(60)), -80 * cos(ToRadian(60)), 9999);
}