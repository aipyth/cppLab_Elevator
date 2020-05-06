#include "Lift.h"

const double motor_speed = 7;
const double lift_door_closing_speed = 4;
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
		std::cout << ":Motor: going up" << std::endl;
		std::thread{ &Motor::runMotor, this }.detach();
		state = MotorState::RUNNING;
	}
}

void Motor::goDown() {
	if (state == MotorState::IDLE) {
		std::cout << ":Motor: going down" << std::endl;
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
		std::cout << ":LiftDoor: opening" << std::endl;
		int sleep_milliseconds = static_cast<int>((maxClosingSpeed - closingSpeed + minClosingSpeed) * 1000);
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_milliseconds));
		state = DoorState::OPEN;
	}
}

void LiftDoor::close() {
	if (state == DoorState::OPEN) {
		std::cout << ":LiftDoor: closing" << std::endl;
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


void CallLiftButton::callUp(Lift* lift) {
	//state = LBState::ACTIVE;
	lift->controller->pushGoUpQ(floor);
}

void CallLiftButton::callDown(Lift* lift) {
	//state = LBState::ACTIVE;
	lift->controller->pushGoDownQ(floor);
}

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
	Controller* cntllr = new Controller(m, lftdoor, &panel);
	controller = cntllr;
	currentFloor = controller->getCurrentFloor();
}

Lift::~Lift() {
	delete controller;
}


Controller::Controller(Motor* m, LiftDoor* lftdoor, Panel* pnl) {
	motor = m;
	liftDoor = lftdoor;
	panel = pnl;

	motor->start();
	motor->setSpeed(motor_speed);
	liftDoor->start();
	liftDoor->setClosingSpeed(lift_door_closing_speed);
	processThread = std::thread(&Controller::process, this);
}

Controller::~Controller()
{
	delete motor;
	delete liftDoor;
}

void Controller::process() {
	static Direction movingDirection = Direction::STAND;

	std::vector<uint8_t> on_route;
	std::vector<uint8_t> not_on_route;

	// ALL IN WHILE CYCLE
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(update_delay));
		lock.lock();
	// check liftButtons in panel
		for (unsigned int i = 0;
			i < panel->floorsNum;
			i++) {
			if (panel->floorButtons[i].getState() == LBState::ACTIVE) {
				if (std::find(route.begin(), route.end(), i) == route.end()) {
					route.push_back(i);
				}
			}
		}
	// look up for new requests to go up or down in queues
		while (!goDownQueue.empty()) {
			route.push_back(goDownQueue.front());
			goDownQueue.pop();
		}
		while (!goUpQueue.empty()) {
			route.push_back(goUpQueue.front());
			goUpQueue.pop();
		}
	// build a new route vector
		for (auto& i : route) {
			if (movingDirection == Direction::STAND) {
				if (i > currentFloor) {
					movingDirection = Direction::UP;
				}
				else if (i < currentFloor) {
					movingDirection = Direction::DOWN;
				}
				on_route.push_back(i);
			}
			else if ((movingDirection == Direction::UP) && (i > currentFloor)) {
				on_route.push_back(i);
			}
			else if ((movingDirection == Direction::DOWN) && (i < currentFloor)) {
				on_route.push_back(i);
			}
			else {
				not_on_route.push_back(i);
			}
		}

		route.erase(route.begin(), route.end());

		// sort on_route and not_on_route
		if (movingDirection == Direction::DOWN) {
			std::sort(on_route.begin(), on_route.end(), std::greater<uint8_t>());
			std::sort(not_on_route.begin(), not_on_route.end());
		}
		else if (movingDirection == Direction::UP) {
			std::sort(on_route.begin(), on_route.end());
			std::sort(not_on_route.begin(), not_on_route.end(), std::greater<uint8_t>());
		}

		route.insert(route.begin(), on_route.begin(), on_route.end());
		route.insert(route.end(), not_on_route.begin(), not_on_route.end());

		on_route.erase(on_route.begin(), on_route.end());
		not_on_route.erase(not_on_route.begin(), not_on_route.end());

		/*std::cout << "route:";
		for (auto& i : route) {
			std::cout << (unsigned int)i << ' ';
		}
		std::cout << std::endl;

		std::cout << std::endl;*/
		lock.unlock();
	// give commands to motor
		if (!route.empty()) {
			if (std::find(route.begin(), route.end(), currentFloor) != route.end()) {
				// lift was called on the current floor

				// wait till the motor stops
				while (motor->getState() == MotorState::RUNNING) {}

				route.erase(std::remove(route.begin(), route.end(), currentFloor), route.end());
				//movingDirection = Direction::STAND;
				panel->getFloorButton(currentFloor)->state = LBState::IDLE;
				liftDoor->open();
				std::this_thread::sleep_for(std::chrono::seconds(2));
				liftDoor->close();
				continue;
			}

			if (motor->getState() != MotorState::RUNNING) {
				if (route.front() > currentFloor) {
					currentFloor++;
					motor->goUp();
				}
				else if (route.front() < currentFloor) {
					currentFloor--;
					motor->goDown();
				}
			}
		}
		else {
			movingDirection = Direction::STAND;
		}
	}
}
