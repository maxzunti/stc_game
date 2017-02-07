/*
XBOX360 Controller code from Lawrence McCauley's tutorial:
https://lcmccauley.wordpress.com/2014/01/05/gamepad-input-tutorial/
*/

#include "Input.h"
 
/*
// Define the 'XButtonIDs' struct as 'XButtons'
XButtonIDs XButtons;

// 'XButtonIDs' - Default constructor
XButtonIDs::XButtonIDs()
{
      // These values are used to index the XINPUT_Buttons array,
      // accessing the matching XINPUT button value
 
      A = 0;
      B = 1;
      X = 2;
      Y = 3;
 
      DPad_Up    = 4;
      DPad_Down  = 5;
      DPad_Left  = 6;
      DPad_Right = 7;
 
      L_Shoulder = 8;
      R_Shoulder = 9;
 
      L_Thumbstick = 10;
      R_Thumbstick = 11;
 
      Start = 12;
      Back  = 13;
}*/
 
Input::Input() {}
 
Input::Input(int controllerIndex)
{
    // Set controller index (0 to 3)
    m_ControllerIndex = controllerIndex;
	
	// Iterate through all gamepad buttons
    for (int i = 0; i < ButtonCount; i++)
    {
          bPrev_ButtonStates[i]   = false;
          bButtonStates[i]        = false;
          bGamepad_ButtonsDown[i] = false;
    }
}

XINPUT_STATE Input::GetState()
{
	XINPUT_STATE ControllerState;
	
	ZeroMemory(&ControllerState, sizeof(XINPUT_STATE));
	
	XInputGetState(m_ControllerIndex, &ControllerState);
	
	return ControllerState;
}

int Input::GetControllerIndex()
{
	return m_ControllerIndex;
}

bool Input::IsConnected()
{
	ZeroMemory(&m_ControllerState, sizeof(XINPUT_STATE));
	
	DWORD returnVal = XInputGetState(m_ControllerIndex, &m_ControllerState);
	
	if (returnVal == ERROR_SUCCESS)
		return true;
	else
		return false;
}

void Input::Update()
{
	m_ControllerState = GetState();
	
	// Iterate through all gamepad buttons
    for (int i = 0; i < ButtonCount; i++)
    {
          // Set button state for current frame
          bButtonStates[i] = (m_ControllerState.Gamepad.wButtons &
             XINPUT_Buttons[i]) == XINPUT_Buttons[i];
 
          // Set 'DOWN' state for current frame
          bGamepad_ButtonsDown[i] = !bPrev_ButtonStates[i] &&
            bButtonStates[i];
    }
}

// Update button states for next frame
void Input::RefreshState()
{
       memcpy(bPrev_ButtonStates, bButtonStates,
         sizeof(bPrev_ButtonStates));
}

bool Input::LStick_InDeadzone()
{
       // Obtain the X & Y axes of the stick
       short sX = m_ControllerState.Gamepad.sThumbLX;
       short sY = m_ControllerState.Gamepad.sThumbLY;
 
       // X axis is outside of deadzone
       if (sX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
           sX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
           return false;
 
       // Y axis is outside of deadzone
       if (sY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
           sY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
           return false;
 
       // One (or both axes) axis is inside of deadzone
       return true;
}

bool Input::RStick_InDeadzone()
{
       // Obtain the X & Y axes of the stick
       short sX = m_ControllerState.Gamepad.sThumbRX;
       short sY = m_ControllerState.Gamepad.sThumbRY;
 
       // X axis is outside of deadzone
       if (sX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
           sX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
           return false;
 
       // Y axis is outside of deadzone
       if (sY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
           sY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
           return false;
 
       // One (or both axes) axis is inside of deadzone
       return true;
}

// Return X axis of left stick
float Input::LeftStick_X()
{
        // Obtain X axis of left stick
        short sX = m_ControllerState.Gamepad.sThumbLX;
 
        // Return axis value, converted to a float
        return (static_cast<float>(sX) / 32768.0f);
}
 
// Return Y axis of left stick
float Input::LeftStick_Y()
{
        // Obtain Y axis of left stick
        short sY = m_ControllerState.Gamepad.sThumbLY;
 
        // Return axis value, converted to a float
        return (static_cast<float>(sY) / 32768.0f);
}
 
// Return X axis of right stick
float Input::RightStick_X()
{
        // Obtain X axis of right stick
        short sX = m_ControllerState.Gamepad.sThumbRX;
 
        // Return axis value, converted to a float
        return (static_cast<float>(sX) / 32768.0f);
}
 
// Return Y axis of right stick
float Input::RightStick_Y()
{
        // Obtain the Y axis of the left stick
        short sY = m_ControllerState.Gamepad.sThumbRY;
 
        // Return axis value, converted to a float
        return (static_cast<float>(sY) / 32768.0f);
}

// Return value of left trigger
float Input::LeftTrigger()
{
        // Obtain value of left trigger
        BYTE Trigger = m_ControllerState.Gamepad.bLeftTrigger;
 
        if (Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
              return Trigger / 255.0f;
 
        return 0.0f; // Trigger was not pressed
}
 
// Return value of right trigger
float Input::RightTrigger()
{
        // Obtain value of right trigger
        BYTE Trigger = m_ControllerState.Gamepad.bRightTrigger;
 
        if (Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
              return Trigger / 255.0f;
 
        return 0.0f; // Trigger was not pressed
}

// Vibrate the gamepad (values of 0.0f to 1.0f only)
void Input::Rumble(float a_fLeftMotor, float a_fRightMotor)
{
       // Vibration state
       XINPUT_VIBRATION VibrationState;
 
       // Zero memory
       ZeroMemory(&VibrationState, sizeof(XINPUT_VIBRATION));
 
       // Calculate vibration values
       int iLeftMotor  = int(a_fLeftMotor * 65535.0f);
       int iRightMotor = int(a_fRightMotor * 65535.0f);
 
       // Set vibration values
       VibrationState.wLeftMotorSpeed  = iLeftMotor;
       VibrationState.wRightMotorSpeed = iRightMotor;
 
       // Set the vibration state
       XInputSetState(m_ControllerIndex, &VibrationState);
}

// Return true if button is pressed, false if not
bool Input::GetButtonPressed(int a_iButton)
{
      if (m_ControllerState.Gamepad.wButtons & XINPUT_Buttons[a_iButton])
      {
            return true; // The button is pressed
      }
 
      return false; // The button is not pressed
}

// Frame-specific version of 'GetButtonPressed' function
bool Input::GetButtonDown(int a_iButton)
{
       return bGamepad_ButtonsDown[a_iButton];
}