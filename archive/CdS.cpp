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

void Drive(float distance, float speedBack, float speedRight, float speedLeft){

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
    
    //encoder counts per inch: 48.89
    while(backShaft.Counts() < backDist || 
            rightShaft.Counts() < rightDist || 
            leftShaft.Counts() < leftDist){
        
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

    while(true){
        LCD.Clear();
        LCD.Write(cdsCell.Value());
        Sleep(100);
    }

    //close: cos(30)
    //far: cos(60)
    //drive motor order
    //back, right, left
    // Drive(68, 0, -50 * cos(ToRadian(30)), 46 * cos(ToRadian(30)));
    // Drive(30, 28, -30 * cos(ToRadian(60)), -30 * cos(ToRadian(60)));
    // Drive(20, 20, 20, 20);
    // Drive(30, 0, 30 * cos(ToRadian(30)), -27 * cos(ToRadian(30)));
    // Drive(60, 0, -30 * cos(ToRadian(30)), 27 * cos(ToRadian(30)));
    // Drive(80, -38, 40 * cos(ToRadian(60)), 40 * cos(ToRadian(60)));
}