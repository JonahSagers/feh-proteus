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

DigitalEncoder left_shaft(FEHIO::P0_2);
DigitalEncoder right_shaft(FEHIO::P0_3);

int left_count = 0;
int right_count = 0;

bool left_latch;
bool right_latch;

int main()
{
    left_shaft.ResetCounts();
    right_shaft.ResetCounts();
    left_motor.SetPercent(25);
    right_motor.SetPercent(25);
    while(left_shaft.Counts() < 566.84){}
    left_motor.Stop();
    right_motor.Stop();

    left_shaft.ResetCounts();
    right_shaft.ResetCounts();
    left_motor.SetPercent(-25);
    right_motor.SetPercent(25);
    while(left_shaft.Counts() < 222.6){}
    left_motor.Stop();
    right_motor.Stop();

    left_shaft.ResetCounts();
    right_shaft.ResetCounts();
    left_motor.SetPercent(25);
    right_motor.SetPercent(25);
    while(left_shaft.Counts() < 404.89){}
    left_motor.Stop();
    right_motor.Stop();

    left_shaft.ResetCounts();
    right_shaft.ResetCounts();
    left_motor.SetPercent(25);
    right_motor.SetPercent(-25);
    while(left_shaft.Counts() < 222.6){}
    left_motor.Stop();
    right_motor.Stop();

    left_shaft.ResetCounts();
    right_shaft.ResetCounts();
    left_motor.SetPercent(25);
    right_motor.SetPercent(25);
    while(left_shaft.Counts() < 161.95){}
    left_motor.Stop();
    right_motor.Stop();
}