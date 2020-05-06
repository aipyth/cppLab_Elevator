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

CallLiftButton** getCallLiftButtons(uint8_t floors) {
	CallLiftButton** clbs = new CallLiftButton*[floors];
	for (int i = 0; i < floors; i++) {
		clbs[i] = new CallLiftButton(i);
	}
	return clbs;
}

void deleteCallLiftButtons(CallLiftButton** arr, uint8_t floors) {
	for (int i = 0; i < floors; i++) {
		delete arr[i];
	}
	delete[] arr;
}

int main(void) {
	uint8_t floors_num = 12;
	Lift* lf = new Lift(floors_num);
	CallLiftButton** callLiftButtons = getCallLiftButtons(floors_num);


	std::cout << "press 3 callLiftButton" << std::endl;
	callLiftButtons[3]->callDown(lf);

	std::cout << "Current floor: " << static_cast<unsigned int>(lf->getCurrentFloor()) << std::endl;

	std::cout << "#press 5 floorbutton#" << std::endl;
	lf->panel.getFloorButton(5)->press();


	int i = 0;
	while (i < 40) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "Current floor: " << static_cast<unsigned int>(lf->getCurrentFloor());
	}

	deleteCallLiftButtons(callLiftButtons, floors_num);

	return 0;
}