#pragma once
#include <cstdint>
#include <algorithm>
#include <thread>
#include <chrono>
#include <queue>
#include <vector>


enum class MotorState { ERROR, DISENGAGED, IDLE, RUNNING };

class Motor
{
	MotorState state = MotorState::DISENGAGED;
	double speed = 1.0;
	static const double min_speed;
	static const double max_speed;

	void runMotor();
public:
	Motor() {};
	void setSpeed(double sp) { speed = std::min(max_speed, std::max(min_speed, sp)); }
	double getSpeed() { return speed; }
	MotorState getState() { return state; }
	void start();
	void stop();
	void goUp();
	void goDown();
};


enum class DoorState { ERROR, DISENGAGED, OPEN, CLOSE };

class LiftDoor
{
	DoorState state = DoorState::DISENGAGED;
	double closingSpeed;
	static const double maxClosingSpeed;
	static const double minClosingSpeed;
public:
	void setClosingSpeed(double sp) { closingSpeed = std::min(maxClosingSpeed, std::max(minClosingSpeed, sp)); }
	double getClosingSpeed() { return closingSpeed; }
	void open();
	void close();
	DoorState getState() { return state; }
	void start();
	void stop();
};


enum class LBState { ACTIVE, IDLE };

class LiftButton {
protected:
	LBState state = LBState::IDLE;
public:
	virtual void press() = 0;
	virtual LBState getState() { return state; }
};


class FloorButton : public LiftButton {
	uint8_t floor;
public:
	virtual void press();
};


class CallButton : public LiftButton {
public:
	virtual void press();
};


class Controller {
	std::queue<uint8_t> goUpQueue;
	std::queue<uint8_t> goDownQueue;
	std::vector<uint8_t> route;
	bool maintenance = false;
	LiftDoor* liftDoor;
	Motor* motor;
public:
	Controller(Motor*, LiftDoor*);
	void toggleMaintenance() { maintenance = !maintenance; }
	~Controller();
};


class Panel {
	uint8_t floorsNum;
	FloorButton* floorButtons;
	CallButton callButton;
	Controller* controller = nullptr;
public:
	Panel(uint8_t);
	FloorButton* getFloorButton(uint8_t);
	CallButton* getCallButton() { return &callButton; };
	~Panel();

	friend class Lift;
	friend class Controller;
};


class Lift
{
	uint8_t currentFloor = 0;
public:
	Panel panel;

	Lift(uint8_t);
	uint8_t getCurrentFloor() { return currentFloor; }
	//~Lift();
};


class CallLiftButton : public LiftButton {
	uint8_t floor;
public:
	virtual void press() {};
	void callUp(Lift);
	void callDown(Lift);
};

