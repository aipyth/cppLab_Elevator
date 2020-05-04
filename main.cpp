#include "Lift.h"
#include <iostream>
#include <string>

std::string translate_motor_state(MotorState mst) {
	switch (mst) {
	case MotorState::RUNNING:
		return "RUNNING";
	case MotorState::DISENGAGED:
		return "DISENGAGED";
	case MotorState::IDLE:
		return "IDLE";
	case MotorState::ERROR:
		return "ERROR";
	}
}

std::string translate_liftdoor_state(DoorState dst) {
	switch (dst) {
	case DoorState::OPEN:
		return "OPEN";
	case DoorState::CLOSE:
		return "CLOSE";
	case DoorState::DISENGAGED:
		return "DISENGAGED";
	case DoorState::ERROR:
		return "ERROR";
	}
}

std::string translate_liftbutton_state(LBState bst) {
	switch (bst) {
	case LBState::ACTIVE:
		return "ACTIVE";
	case LBState::IDLE:
		return "IDLE";
	}
}

int main(void) {
	Lift* lf = new Lift(5);

	CallButton* cbtn = lf->panel.getCallButton();

	std::cout << translate_liftbutton_state(cbtn->getState()) << std::endl;

	return 0;
}