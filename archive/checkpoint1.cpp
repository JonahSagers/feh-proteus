#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHIO.h>
#include <FEHLCD.h>

// declare motor on motor port 0, set maximum voltage to 9.0 V
FEHMotor front_motor(FEHMotor::Motor0,9.0);
FEHMotor left_motor(FEHMotor::Motor2,9.0);
FEHMotor right_motor(FEHMotor::Motor1,9.0);

DigitalEncoder front_shaft(FEHIO::P3_5);
DigitalEncoder left_shaft(FEHIO::P3_6);
DigitalEncoder right_shaft(FEHIO::P0_0);

int left_count = 0;
int right_count = 0;

bool left_latch;
bool right_latch;

int main()
{
    right_shaft.ResetCounts();
    right_motor.SetPercent(10);
    Sleep(3000);
    while(left_shaft.Counts() < 1){}
    left_motor.Stop();
    right_motor.Stop();
    left_motor.SetPercent(-40);
    right_motor.SetPercent(-38);
    Sleep(3500);
    left_motor.Stop();
    right_motor.Stop();
}