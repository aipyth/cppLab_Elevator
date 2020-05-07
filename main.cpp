#include "Lift.h"
#include <iostream>
#include <string>
#include <map>


std::string gotoCommand = "Go to ";
std::string callDownCommand = "Call down on ";
std::string callUpCommand = "Call up on ";
std::string callCommand = "CallButton";
std::string askFloorCommand = "Current floor";
std::string exitCommand = "Exit";

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

std::map<std::string, int>* generate_commands_map(int n) {
	std::map<std::string, int>* cmap = new std::map<std::string, int>;

	//std::cout << "n = " << n << std::endl;
	for (int i = 0; i < n; i++) {

		cmap->insert({ (gotoCommand + std::to_string(i)), i });
		cmap->insert({ (callDownCommand + std::to_string(i)), i });
		cmap->insert({ (callUpCommand + std::to_string(i)), i });
	}
	cmap->insert({ callCommand, -1 });

	return cmap;
}


int main(void) {
	unsigned int floors_num;
	std::cout << "Enter floors num: ";
	std::cin >> floors_num;
	Lift* lf = new Lift(floors_num);
	CallLiftButton** callLiftButtons = getCallLiftButtons(floors_num);

	std::map<std::string, int>* cmap = generate_commands_map(floors_num);

	std::cout << "You can press floor button in lift with command \33[0;37;46m" << gotoCommand << "#floor#\33[0;37;40m" << std::endl;
	std::cout << "You can press call lift up button with command \33[0;37;46m" << callUpCommand << "#floor#\33[0;37;40m" << std::endl;
	std::cout << "You can press call lift down button with command \33[0;37;46m" << callDownCommand << "#floor#\33[0;37;40m" << std::endl;
	std::cout << "You can call dispatcher with command \33[0;37;46m" << callCommand << "\33[0;37;40m" << std::endl;
	std::cout << "You can exit with command \33[0;37;46m" << exitCommand << "\33[0;37;40m" << std::endl;

	std::string input;
	getline(std::cin, input, '\n');
	do {
		getline(std::cin, input, '\n');
		if (input.find(gotoCommand) == 0) {
			//std::cout << "goto\n";
			if (cmap->find(input) == cmap->end()) {
				std::cout << "\33[0;37;41mNo such floor.\33[0;37;40m" << std::endl;
				continue;
			}
			lf->panel.getFloorButton(cmap->at(input))->press();
		}
		else if (input.find(callDownCommand) == 0) {
			//std::cout << "callDown\n";
			if (cmap->find(input) == cmap->end()) {
				std::cout << "\33[0;37;41mNo such floor.\33[0;37;40m" << std::endl;
				continue;
			}
			callLiftButtons[cmap->at(input)]->callDown(lf);
		}
		else if (input.find(callUpCommand) == 0) {
			//std::cout << "callUp\n";
			if (cmap->find(input) == cmap->end()) {
				std::cout << "\33[0;37;41mNo such floor.\33[0;37;40m" << std::endl;
				continue;
			}
			callLiftButtons[cmap->at(input)]->callUp(lf);
		}
		else if (input == callCommand) {
			//std::cout << "call\n";
			if (cmap->find(input) == cmap->end()) {
				std::cout << "\33[0;37;41mInvalid input.\33[0;37;40m" << std::endl;
				continue;
			}
			lf->panel.getCallButton()->press();
		}
		else if (input == askFloorCommand) {
			std::cout << "\33[0;37;44mCurrent floor: " << (unsigned int)lf->getCurrentFloor() << "\33[0;37;40m" << std::endl;
		}
		else if (input == exitCommand) {
			std::cout << "\33[0;37;43mExiting...\33[0;37;40m\n";
			deleteCallLiftButtons(callLiftButtons, floors_num);
			exit(0);
		}
		else {
			std::cout << "\33[0;37;41mInvalid input.\33[0;37;40m" << std::endl;
		}
	} while (true);



	/*std::cout << "press 3 callLiftButton" << std::endl;
	callLiftButtons[3]->callDown(lf);

	std::cout << "Current floor: " << static_cast<unsigned int>(lf->getCurrentFloor()) << std::endl;

	std::cout << "#press 5 floorbutton#" << std::endl;
	lf->panel.getFloorButton(5)->press();


	deleteCallLiftButtons(callLiftButtons, floors_num);*/

	return 0;
}