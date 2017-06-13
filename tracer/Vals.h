class Vals
{
private:
	signed char forward;
    signed char turn;
    int motor_ang_l;
    int motor_ang_r;
    int gyro;
    int volt;
    int alert;
    int reflect;
	bool stop;

public:
	Vals();

	signed char getForward();
	void setForward(signed char forward);

	signed char getTurn();
	void setTurn(signed char turn);

	int getMotor_ang_l();
	void setMotor_ang_l(int motor_ang_l);

	int getMotor_ang_r();
	void setMotor_ang_r(int motor_ang_r);

	int getGyro();
	void setGyro(int gyro);

	int getVolt();
	void setVolt(int volt);

	int getAlert();
	void setAlert(int alert);

	int getReflect();
	void setReflect(int reflect);

	bool getStop();
	void setStop(bool stop);
};