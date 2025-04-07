#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHIO.h>
#include <FEHLCD.h>
#include <FEHRCS.h>
#include <stdlib.h>
#include <math.h>

# define PI 3.141592

// declare motor on motor port 0, set maximum voltage to 9.0 V
//FEHMotor backMotor(FEHMotor::Motor0,9.0);

FEHMotor backMotor(FEHMotor::Motor0,9.0);
FEHMotor rightMotor(FEHMotor::Motor1,9.0);
FEHMotor leftMotor(FEHMotor::Motor2,9.0);

//DigitalEncoder backShaft(FEHIO::P3_7);
DigitalEncoder backShaft(FEHIO::P2_5);
DigitalEncoder rightShaft(FEHIO::P2_6);
DigitalEncoder leftShaft(FEHIO::P2_7);

AnalogInputPin cdsCell(FEHIO::P0_7);

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

    float backDist, rightDist, leftDist;
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
        
        // if(backShaft.Counts() >= backDist){
        //     backMotor.Stop();
        // }
        // if(rightShaft.Counts() >= rightDist){
        //     rightMotor.Stop();
        // }
        // if(leftShaft.Counts() >= leftDist){
        //     leftMotor.Stop();
        // }
    }
    backMotor.Stop();
    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(50);
}

int main()
{
    // RCS.InitializeTouchMenu("0800A5DYF");

    // int lever = RCS.GetLever();

    // LCD.WriteLine(lever);

    //close: cos(30)
    //far: cos(60)
    //drive motor order
    //back, right, left
    //Drive(100, 15, 15, 15);
    while(cdsCell.Value() > 0.7){}
    Drive(2, 0, 50 * cos(ToRadian(30)), -46 * cos(ToRadian(30)), 9999);
    Drive(2, 0, -50 * cos(ToRadian(30)), 46 * cos(ToRadian(30)), 9999);
    Drive(12, 20, 20, 20, 9999);
    Drive(68, 0, -50 * cos(ToRadian(30)), 46 * cos(ToRadian(30)), 9999);
    Drive(42, 28, -30 * cos(ToRadian(60)), -30 * cos(ToRadian(60)), 9999);
    Sleep(1000);
    LCD.WriteLine(cdsCell.Value());
    if(cdsCell.Value() < 0.3){
        Drive(4, 0, -30 * cos(ToRadian(30)), 27 * cos(ToRadian(30)), 9999);
        LCD.Clear(RED);
    } else {
        Drive(4, 0, 30 * cos(ToRadian(30)), -27 * cos(ToRadian(30)), 9999);
        LCD.Clear(BLUE);
    }
    Drive(20, 20, 20, 20, 9999);
    Drive(20, 0, 30 * cos(ToRadian(30)), -27 * cos(ToRadian(30)), 3000);
    Drive(60, 0, -30 * cos(ToRadian(30)), 27 * cos(ToRadian(30)), 9999);
    Drive(85, -38, 40 * cos(ToRadian(60)), 40 * cos(ToRadian(60)), 9999);
    Drive(20, 20, 20, 20, 9999);
    Drive(42, 28, -30 * cos(ToRadian(60)), -30 * cos(ToRadian(60)), 9999);
}