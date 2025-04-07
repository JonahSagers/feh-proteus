#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHIO.h>
#include <FEHLCD.h>

// declare motor on motor port 0, set maximum voltage to 9.0 V
FEHMotor left_motor(FEHMotor::Motor0,9.0);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
AnalogEncoder left_opto(FEHIO::P2_0);
AnalogEncoder center_opto(FEHIO::P2_1);
AnalogEncoder right_opto(FEHIO::P2_2);

float left_online = 2.3;
float left_offline = 0.85;
float center_online = 2.742;
float center_offline = 2.208;
float right_online = 2.595;
float right_offline = 1.52;

bool left;
bool center;
bool right;

int main()
{
    while(true){
        if(left_opto.Value() > left_online - 0.3 && left_opto.Value() < left_online + 0.3){
            left = true;
        } else {
            left = false;
        }
        if(center_opto.Value() > center_online - 0.3 && center_opto.Value() < center_online + 0.3){
            center = true;
        } else {
            center = false;
        }
        if(right_opto.Value() > right_online - 0.3 && right_opto.Value() < right_online + 0.3){
            right = true;
        } else {
            right = false;
        }

        if(left && !right) {
            left_motor.SetPercent(20);
            right_motor.SetPercent(0);
        } else if(!left && right) {
            left_motor.SetPercent(0);
            right_motor.SetPercent(20);
        } else {
            left_motor.SetPercent(20);
            right_motor.SetPercent(20);
        }
    }
}