/*
    Name:       Car.ino
    Created:	01.11.2018 08.54.10
    Author:     LAPTOP-LJ3QMHSA\thaun
*/

const int MOTOR_RIGHT_FRONT = 2;
const int MOTOR_RIGHT_BACK = 3;
const int MOTOR_LEFT_FRONT = 4;
const int MOTOR_LEFT_BACK = 5;
const int MOTOR_RIGHT_FRONT_REVERSE = 6;
const int MOTOR_RIGHT_BACK_REVERSE = 7;
const int MOTOR_LEFT_FRONT_REVERSE = 8;
const int MOTOR_LEFT_BACK_REVERSE = 9;

const int DISTANCE_FRONT_SENSOR_1 = 10;
const int DISTANCE_FRONT_SENSOR_2 = 11;
const int DISTANCE_BACK_SENSOR = 12;

const int TOGGLE_MOTOR_BUTTON = A0;

boolean fourWheeler = true;
boolean frontWheel = false;

boolean driving = false;
boolean releasedButton = true;

int motorState = 0;

void setup()
{
	Serial.begin(9600);
	
	pinMode(MOTOR_RIGHT_FRONT, OUTPUT);
	pinMode(MOTOR_RIGHT_BACK, OUTPUT);
	pinMode(MOTOR_LEFT_FRONT, OUTPUT);
	pinMode(MOTOR_LEFT_BACK, OUTPUT);
	pinMode(MOTOR_RIGHT_FRONT_REVERSE, OUTPUT);
	pinMode(MOTOR_RIGHT_BACK_REVERSE, OUTPUT);
	pinMode(MOTOR_LEFT_FRONT_REVERSE, OUTPUT);
	pinMode(MOTOR_LEFT_BACK_REVERSE, OUTPUT);
	
	pinMode(TOGGLE_MOTOR_BUTTON, INPUT);
}

void loop()
{
 	boolean buttonReleased = digitalRead(TOGGLE_MOTOR_BUTTON);
 	if(!buttonReleased && releasedButton) {
 		driving = !driving;
		releasedButton = false;
 	}else if(buttonReleased){
 		releasedButton = true;
 	}
	// SENSOR
	
	long distance1 = getSensorDistance(DISTANCE_FRONT_SENSOR_1);
	delay(20);
	long distance2 = getSensorDistance(DISTANCE_FRONT_SENSOR_2);
	delay(20);
	long distance3 = getSensorDistance(DISTANCE_BACK_SENSOR);
	delay(20);
	
	String json = "{\n    \"sensor1\": " + String(distance1) + ", \n    \"sensor2\": " + String(distance2) + ", \n    \"sensor3\": " + String(distance3) + "\n    \"driving\": " + String(driving) + "\n}";
	Serial.println(json);
	if(driving){
		if (distance3 < 60)
		{
			driveForwards();
		}else if(distance1 < 60 && distance2 > 60){
			rotateLeft();
		}else if(distance2 < 60 && distance1 > 60){
			rotateRight();
		}else if(distance1 < 60 && distance2 < 60){
			driveBackwards();
		}else if(distance1 > 100 && distance2 > 100){
			driveForwards();
		}else{
			driveForwards();
		}
	}else{
		stopAllMotors();
	}
}

#pragma region Motor

void rotateLeft(){
	stopAllMotors();
	if(fourWheeler || (frontWheel && !fourWheeler)){
		startMotor(MOTOR_RIGHT_FRONT_REVERSE);
		startMotor(MOTOR_LEFT_FRONT);
	} if(fourWheeler || (!frontWheel && !fourWheeler)){
		startMotor(MOTOR_RIGHT_BACK_REVERSE);
		startMotor(MOTOR_LEFT_BACK);
	}
}

void rotateRight(){
	stopAllMotors();
	if(fourWheeler || (frontWheel && !fourWheeler)){
		startMotor(MOTOR_RIGHT_FRONT);
		startMotor(MOTOR_LEFT_FRONT_REVERSE);
	} if(fourWheeler || (!frontWheel && !fourWheeler)){
		startMotor(MOTOR_RIGHT_BACK);
		startMotor(MOTOR_LEFT_BACK_REVERSE);
	}
}

void driveForwards(){
	stopAllMotors();
	if(fourWheeler || (frontWheel && !fourWheeler)){
		startMotor(MOTOR_RIGHT_FRONT);
		startMotor(MOTOR_LEFT_FRONT);
	} if(fourWheeler || (!frontWheel && !fourWheeler)){
		startMotor(MOTOR_RIGHT_BACK);
		startMotor(MOTOR_LEFT_BACK);
	}
}

void driveBackwards(){
	stopAllMotors();
	if(fourWheeler || (frontWheel && !fourWheeler)){
		startMotor(MOTOR_RIGHT_FRONT_REVERSE);
		startMotor(MOTOR_LEFT_FRONT_REVERSE);
	} if(fourWheeler || (!frontWheel && !fourWheeler)){
		startMotor(MOTOR_RIGHT_BACK_REVERSE);
		startMotor(MOTOR_LEFT_BACK_REVERSE);
	}
}

void stopAllMotors(){
	stopMotor(MOTOR_RIGHT_FRONT);
	stopMotor(MOTOR_RIGHT_BACK);
	stopMotor(MOTOR_LEFT_FRONT);
	stopMotor(MOTOR_LEFT_BACK);
	stopMotor(MOTOR_RIGHT_FRONT_REVERSE);
	stopMotor(MOTOR_RIGHT_BACK_REVERSE);
	stopMotor(MOTOR_LEFT_FRONT_REVERSE);
	stopMotor(MOTOR_LEFT_BACK_REVERSE);
}

void stopMotor(int motor){
	digitalWrite(motor, HIGH);
}

void startMotor(int motor){
	digitalWrite(motor, LOW);
}

#pragma endregion Sensor

#pragma region Sensor
long microsecondsToInches(long microseconds) {
	// According to Parallax's datasheet for the PING))), there are 73.746
	// microseconds per inch (i.e. sound travels at 1130 feet per second).
	// This gives the distance travelled by the ping, outbound and return,
	// so we divide by 2 to get the distance of the obstacle.
	// See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
	return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
	// The speed of sound is 340 m/s or 29 microseconds per centimeter.
	// The ping travels out and back, so to find the distance of the object we
	// take half of the distance travelled.
	return microseconds / 29 / 2;
}

int getSensorDistance(int sensorPin){
	long duration, inches, cm;

	// The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
	// Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
	pinMode(sensorPin, OUTPUT);
	digitalWrite(sensorPin, LOW);
	delayMicroseconds(2);
	digitalWrite(sensorPin, HIGH);
	delayMicroseconds(5);
	digitalWrite(sensorPin, LOW);

	// The same pin is used to read the signal from the PING))): a HIGH pulse
	// whose duration is the time (in microseconds) from the sending of the ping
	// to the reception of its echo off of an object.
	pinMode(sensorPin, INPUT);
	
	long sensor1Duration = pulseIn(sensorPin, HIGH);

	// convert the time into a distance
	inches = microsecondsToInches(duration);
	cm = microsecondsToCentimeters(duration);
	
	inches = microsecondsToInches(sensor1Duration);
	cm = microsecondsToCentimeters(sensor1Duration);
	
	return cm;
}
#pragma endregion Sensor