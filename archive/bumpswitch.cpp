#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHIO.h>
// declare motor on motor port 0, set maximum voltage to 9.0 V
FEHMotor left_motor(FEHMotor::Motor0,9.0);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
DigitalInputPin left_front_bump(FEHIO::P1_0); 
DigitalInputPin right_front_bump(FEHIO::P3_6);
DigitalInputPin left_back_bump(FEHIO::P0_2); 
DigitalInputPin right_back_bump(FEHIO::P3_7);

void MoveForward()
{
    bool left = true, right = true;
    left_motor.SetPercent(-50);
    right_motor.SetPercent(-50);
    while(left || right){
        if(!left_front_bump.Value()){
            left_motor.Stop();
            left = false;
        }
        if(!right_front_bump.Value()){
            right_motor.Stop();
            right = false;
        }
    }
}

void TurnBackwards(int leftPower, int rightPower)
{
    bool left = true, right = false;
    left_motor.SetPercent(leftPower);
    right_motor.SetPercent(rightPower);
    while(left || right){
        if(!left_back_bump.Value()){
            left_motor.Stop();
            left = false;
        } else {
            left_motor.SetPercent(leftPower);
        }
        if(!right_back_bump.Value()){
            right_motor.Stop();
            right = false;
        } else {
            right_motor.SetPercent(rightPower);
        }
    }
}

int main()
{
    MoveForward();
    Sleep(0.1);
    TurnBackwards(5, 50);
    Sleep(0.1);
    MoveForward();
    Sleep(0.1);
    TurnBackwards(50, 5);
    Sleep(0.1);
    MoveForward();
}