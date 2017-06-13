
#include "Vals.h"

Vals::Vals() {
	stop = false;
}

signed char Vals::getForward() {
	return forward;
}

void Vals::setForward(signed char forward) {
	this->forward = forward;
}

signed char Vals::getTurn() {
	return turn;
}

void Vals::setTurn(signed char turn) {
	this->turn = turn;
}

int Vals::getMotor_ang_l() {
	return motor_ang_l;
}

void Vals::setMotor_ang_l(int motor_ang_l) {
	this->motor_ang_l = motor_ang_l;
}

int Vals::getMotor_ang_r() {
	return motor_ang_r;
}

void Vals::setMotor_ang_r(int motor_ang_r) {
	this->motor_ang_r = motor_ang_r;
}

int Vals::getGyro() {
	return gyro;
}

void Vals::setGyro(int gyro) {
	this->gyro = gyro;
}

int Vals::getVolt() {
	return volt;
}

void Vals::setVolt(int volt) {
	this->volt = volt;
}

int Vals::getAlert() {
	return alert;
}

void Vals::setAlert(int alert) {
	this->alert = alert;
}

int Vals::getReflect() {
	return reflect;
}

void Vals::setReflect(int reflect) {
	this->reflect = reflect;
}

bool Vals::getStop() {
	return stop;
}

void Vals::setStop(bool stop) {
	this->stop = stop;
}