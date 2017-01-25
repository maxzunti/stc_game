/*
XBOX360 Controller code from Lawrence McCauley's tutorial:
https://lcmccauley.wordpress.com/2014/01/05/gamepad-input-tutorial/
*/

#ifndef INPUT_H
#define INPUT_H
 
#include <Windows.h>
#include <Xinput.h>
 
// XInput Button values
static const WORD XINPUT_Buttons[] = {
      XINPUT_GAMEPAD_A,
      XINPUT_GAMEPAD_B,
      XINPUT_GAMEPAD_X,
      XINPUT_GAMEPAD_Y,
      XINPUT_GAMEPAD_DPAD_UP,
      XINPUT_GAMEPAD_DPAD_DOWN,
      XINPUT_GAMEPAD_DPAD_LEFT,
      XINPUT_GAMEPAD_DPAD_RIGHT,
      XINPUT_GAMEPAD_LEFT_SHOULDER,
      XINPUT_GAMEPAD_RIGHT_SHOULDER,
      XINPUT_GAMEPAD_LEFT_THUMB,
      XINPUT_GAMEPAD_RIGHT_THUMB,
      XINPUT_GAMEPAD_START,
      XINPUT_GAMEPAD_BACK
};
 
// XInput Button IDs
struct XButtonIDs
{
      // Function prototypes
      //---------------------//
 
      XButtonIDs(); // Default constructor
 
      // Member variables
      //---------------------//
 
      int A,B,X,Y; // 'Action' buttons
 
      // Directional Pad (D-Pad)
      int DPad_Up, DPad_Down, DPad_Left, DPad_Right;
 
      // Shoulder ('Bumper') buttons
      int L_Shoulder, R_Shoulder;
 
      // Thumbstick buttons
      int L_Thumbstick, R_Thumbstick;
 
      int Start; // 'START' button
      int Back;  // 'BACK' button
};
 
class Input
{
public:
     // Constructors
     Input();
     Input(int controllerIndex);
	 
	 // Member functions
	 void Update();
	 void RefreshState(); // Update button states for next frame
	 
	 XINPUT_STATE GetState();
	 int GetControllerIndex();
	 bool IsConnected();
	 
	 bool LStick_InDeadzone();
     bool RStick_InDeadzone();
     float LeftStick_X();  // Return X axis of left stick
     float LeftStick_Y();  // Return Y axis of left stick
     float RightStick_X(); // Return X axis of right stick
     float RightStick_Y(); // Return Y axis of right stick
	 
	 float LeftTrigger();  // Return value of left trigger
     float RightTrigger(); // Return value of right trigger
	 
	 // Button functions
     // - 'Pressed' - Return true if pressed, false if not
     // - 'Down'    - Same as 'Pressed', but ONLY on current frame
     bool GetButtonPressed(int a_iButton);
     bool GetButtonDown(int a_iButton);
	 
	 // Vibrate the gamepad (0.0f cancel, 1.0f max speed)
     void Rumble(float a_fLeftMotor = 0.0f, float a_fRightMotor = 0.0f);
 
private:
     
	 // Members
     XINPUT_STATE m_ControllerState;
     int m_ControllerIndex;
	 
	 static const int ButtonCount = 14;    // Total gamepad buttons
     bool bPrev_ButtonStates[ButtonCount]; // Previous frame button states
     bool bButtonStates[ButtonCount];      // Current frame button states
 
     // Buttons pressed on current frame
     bool bGamepad_ButtonsDown[ButtonCount];
};
 
#endif

// Externally define the 'XButtonIDs' struct as 'XButtons'
extern XButtonIDs XButtons;