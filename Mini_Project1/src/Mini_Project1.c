/*
 ============================================================================
 Name        : Mini_Project1.c
 Author      : Mohamed Ehab
 Description : Vehicle Control System Project
 Date		 : 5/5/2023
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

//The definition is to decide either to work with the engine temperature sensor or not.
#define WITH_ENGINE_TEMP_CONTROLLER 0

//All the needed variables are declared global instead of passing most of them every time I call a function.
//Initialization of variables is random.
char UserFirstDecision;
char UserSecondDecision;
char TrafficLightInput;
int VehicleSpeed = 100;
float RoomTemperatureInput;
float RoomTemp = 20;
char AC = 0;
float EngineTemperatureInput;
float EngineTemp = 20;
char EngineTemperatureController = 0;
char EngineState = 0;

// 6 functions are declared for each condition in the system.
void UserFirstInput();
void UserSecondInput();
void TrafficLightColor();
void RoomTemperature();
void EngineTemperature();
void Display();

int main(void) {
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	//only the user first input function has called in the main, then every function will call the needed one.
	UserFirstInput();

	return EXIT_SUCCESS;
}

/*the user first input function is taking the first input from the user and keep taking the input
 * if it was wrong. then depends on this input it takes an action either to quit the system, repeat to ask for the first input
 * or to go to the user second input function.*/
void UserFirstInput()
{
	do{
		printf("\n");
		printf("a. Turn on the vehicle engine.\n");
		printf("b. Turn off the vehicle engine.\n");
		printf("c. Quit the system.\n");
		scanf(" %c", &UserFirstDecision);
	}while(UserFirstDecision != 'c' && UserFirstDecision != 'a');

	if(UserFirstDecision == 'c')
	{
		printf("You Quit The System.");
		return;
	}

	else if(UserFirstDecision == 'a')
	{
		UserSecondInput();
		EngineState = 1;
	}
	else if(UserFirstDecision == 'b')
	{
		EngineState = 0;
	}
}

/*the user second input function is called either by the user first input function or the display function.
 * this function is asking the user for the second input and keep asking if the input was wrong.
 * depends on the input it calls the needed function either the traffic light color function, room temperature function,
 * or engine temperature function and then call the display function which also call back this function.
 * this function also is preprocessed to check if the engine temperature sensor is on or off.*/
void UserSecondInput()
{
	do{
		printf("\n");
		printf("a. Turn off the engine.\n");
		printf("b. Set the traffic light color.\n");
		printf("c. Set the room temperature (Temperature Sensor).\n");
//preprocessing for the engine temperature sensor.
#if WITH_ENGINE_TEMP_CONTROLLER == 1
		printf("d. Set the engine temperature (Engine Temperature Sensor).\n");
#endif
		scanf(" %c", &UserSecondDecision);
	}while(UserSecondDecision != 'a' && UserSecondDecision != 'b' && UserSecondDecision != 'c' && !((UserSecondDecision != 'd') ^ WITH_ENGINE_TEMP_CONTROLLER));

	if(UserSecondDecision == 'a')
	{
		EngineState = 0;
		UserFirstInput();
	}
	else if(UserSecondDecision == 'b')
	{
		EngineState = 1;
		TrafficLightColor();
		Display();
	}
	else if(UserSecondDecision == 'c')
	{
		EngineState = 1;
		RoomTemperature();
		Display();
	}
	else if(UserSecondDecision == 'd')
	{
		EngineState = 1;
		EngineTemperature();
		Display();
	}
}

/*the traffic light color function is asking for the required color and keep asking if the input was wrong.
 * then it set the vehicle speed depends on the input,
 * then it checks some cases in the vehicle speed, room temp, and engine temp variables.*/
void TrafficLightColor()
{
	do{
		printf("Enter the required color (G,O,R): ");
		scanf(" %c", &TrafficLightInput);
	}while(TrafficLightInput != 'G' && TrafficLightInput != 'O' && TrafficLightInput != 'R');

	if(TrafficLightInput == 'G')
	{
		VehicleSpeed = 100;
	}
	else if(TrafficLightInput == 'O')
	{
		VehicleSpeed = 30;
	}
	else if(TrafficLightInput == 'R')
	{
		VehicleSpeed = 0;
	}

	if(VehicleSpeed == 30)
	{
		AC = 1;
		EngineTemperatureController = 1;
		RoomTemp*=((5.0/4.0) + 1.0);
		EngineTemp*=((5.0/4.0) + 1.0);
	}

	if(RoomTemp<10 || RoomTemp>30)
	{
		RoomTemp = 20;
		AC = 1;
	}
	else
	{
		AC = 0;
	}

	if(EngineTemp <100 || EngineTemp > 150)
	{
		EngineTemp = 125;
		EngineTemperatureController = 1;

	}
	else
	{
		EngineTemperatureController = 0;
	}

}

/*the room temperature function is asking for the required room temperature number
 * then it checks for some cases with the room temperature*/
void RoomTemperature()
{
	printf("Enter the required room temperature: ");
	scanf(" %f", &RoomTemperatureInput);

	if(RoomTemperatureInput<10 || RoomTemperatureInput>30)
	{
		RoomTemp = 20;
		AC = 1;
	}
	else
	{
		RoomTemp = RoomTemperatureInput;
		AC = 0;
	}
}

/*the engine temperature function is asking for the required engine temperature number
 * then it checks for some cases with the engine temperature*/
void EngineTemperature()
{
	printf("Enter the required engine temperature: ");
	scanf(" %f", &EngineTemperatureInput);

	if(EngineTemperatureInput <100 || EngineTemperatureInput > 150)
	{
		EngineTemp = 125;
		EngineTemperatureController = 1;

	}
	else
	{
		EngineTemp = EngineTemperatureInput;
		EngineTemperatureController = 0;
	}
}

/*the display function is printing the needed data then it calls the user second input function
 * the display function is preprocessed to check if the engine temperature sensor is on or off
 * to decide if it will display it's data or not*/
void Display()
{
	printf("\n");
	if(EngineState == 1)
	{
		printf("Engine State: ON\n");
	}
	else if(EngineState == 0)
	{
		printf("Engine State: OFF\n");
	}

	if(AC == 1)
	{
		printf("AC: ON\n");
	}
	else if(AC == 0)
	{
		printf("AC: OFF\n");
	}

	printf("Vehicle Speed: %d Km/Hr\n", VehicleSpeed);
	printf("Room Temperature: %.2f C\n", RoomTemp);

#if WITH_ENGINE_TEMP_CONTROLLER == 1
	if(EngineTemperatureController == 1)
	{
		printf("Engine temperature controller: ON\n");
	}
	else if(EngineTemperatureController == 0)
	{
		printf("Engine temperature controller: OFF\n");
	}
	printf("Engine temperature: %.2f C\n", EngineTemp);
#endif

	printf("\n");
	UserSecondInput();
}
