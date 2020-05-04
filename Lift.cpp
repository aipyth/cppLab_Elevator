#include "Lift.h"

const double Motor::max_speed = 8;
const double Motor::min_speed = 0.2;
const double LiftDoor::maxClosingSpeed = 4;
const double LiftDoor::minClosingSpeed = 1;

void Motor::start() {	
	if (state == MotorState::DISENGAGED) {
		state = MotorState::IDLE;
	}
}

void Motor::stop() {
	state = MotorState::DISENGAGED;
}

void Motor::goUp() {
	if (state == MotorState::IDLE) {
		std::thread{ &Motor::runMotor, this }.detach();
		state = MotorState::RUNNING;
	}
}

void Motor::goDown() {
	if (state == MotorState::IDLE) {
		std::thread{ &Motor::runMotor, this }.detach();
		state = MotorState::RUNNING;
	}
}

void Motor::runMotor() {
	int sleep_milliseconds = static_cast<int>((max_speed - speed + min_speed) * 1000);
	std::this_thread::sleep_for(std::chrono::milliseconds(sleep_milliseconds));
	state = MotorState::IDLE;
}

void LiftDoor::open() {
	if (state == DoorState::CLOSE) {
		int sleep_milliseconds = static_cast<int>((maxClosingSpeed - closingSpeed + minClosingSpeed) * 1000);
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_milliseconds));
		state = DoorState::OPEN;
	}
}

void LiftDoor::close() {
	if (state == DoorState::OPEN) {
		int sleep_milliseconds = static_cast<int>((maxClosingSpeed - closingSpeed + minClosingSpeed) * 1000);
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_milliseconds));
		state = DoorState::CLOSE;
	}
}

void LiftDoor::start() {
	if (state == DoorState::DISENGAGED) {
		state = DoorState::CLOSE;
	}
}

void LiftDoor::stop() {
	state = DoorState::DISENGAGED;
}

void FloorButton::press()
{
	state = LBState::ACTIVE;
}

void CallButton::press() {
	if (state == LBState::ACTIVE) {
		state = LBState::IDLE;
	}
	else if (state == LBState::IDLE) {
		state = LBState::ACTIVE;
	}
}


void CallLiftButton::callUp(Lift lift) {}

void CallLiftButton::callDown(Lift lift) {}

Panel::Panel(uint8_t floors_num) {
	floorsNum = floors_num;
	floorButtons = new FloorButton[floorsNum];
}

FloorButton* Panel::getFloorButton(uint8_t fn) {
	if (fn <= floorsNum) {
		return &floorButtons[fn];
	}
	return nullptr;
}

Panel::~Panel() {
	delete[] floorButtons;
}

Lift::Lift(uint8_t floors_num) : panel(floors_num) {
	Motor* m = new Motor;
	LiftDoor* lftdoor = new LiftDoor;
	Controller* cntllr = new Controller(m, lftdoor);


	panel.controller = cntllr;
}


Controller::Controller(Motor* m, LiftDoor* lftdoor)
{
	motor = m;
	liftDoor = lftdoor;
}

Controller::~Controller()
{
}
