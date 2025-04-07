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

DigitalInputPin forkLimit(FEHIO::P3_0);

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
            speedMult += 0.01;
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
    forklift.SetPercent(80 * (distance/abs(distance)));
    Sleep(abs(distance));
    forklift.Stop();
    Sleep(250);
}

void LiftTop()
{
    forklift.SetPercent(80);
    while(forkLimit.Value() > 0) {
        LCD.Clear();
        LCD.WriteLine(forkLimit.Value());
    }
    forklift.SetPercent(0);
}

int main()
{
    // ResetTime();
    // Sleep(1);
    // //while(cdsCell.Value() > 0.7){}
    // Drive(3, 0, 20, -20, 1);
    Drive(33.5, 0, -40, 40, 10);
    // Drive(4, -40, -40, -40, 10);
    // //FindLine(-1, 2.5, 3);
    // Drive(17, 0, -20, 20, 10);
    Lift(0.5);
    // Drive(38, 0, 41, -39, 10);
    Lift(-0.5);
    // Drive(10, 40, 40, 40, 10);
    // Drive(36, 0, -58, 62, 10);
    LiftTop();
    // Drive(15, 0, -40, 40, 2);
    // Lift(-0.3);
    // Drive(18, 0, 40, -40, 10);
    // Drive(4, -40, -40, -40, 10);
    // Drive(50, 0, -40, 40, 10);
    // Lift(-2);
}