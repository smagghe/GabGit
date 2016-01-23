//#######################################################################################################
//#################################### Plugin 099: Input Relay #########################################
//#######################################################################################################

#define PLUGIN_099
#define PLUGIN_ID_099         99
#define PLUGIN_NAME_099       "Relay"
#define PLUGIN_VALUENAME1_099 "Relay"

boolean Plugin_099(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;
  static byte switchstate[TASKS_MAX];
  static byte outputstate[TASKS_MAX];

  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_099;
        Device[deviceCount].Type = DEVICE_TYPE_SINGLE;
        Device[deviceCount].VType = SENSOR_TYPE_RELAY;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = true;
        Device[deviceCount].FormulaOption = false;
        Device[deviceCount].ValueCount = 1;
	        Device[deviceCount].SendDataOption = false;

        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_099);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_099));
        break;
      }



    case PLUGIN_INIT:
      {
        pinMode(Settings.TaskDevicePin1[event->TaskIndex], INPUT);

        if (Settings.TaskDevicePin1Inversed[event->TaskIndex]) {
          switchstate[event->TaskIndex] = !digitalRead(Settings.TaskDevicePin1[event->TaskIndex]);
        }
        else
        {
          switchstate[event->TaskIndex] = digitalRead(Settings.TaskDevicePin1[event->TaskIndex]);
        }
        success = true;
        break;
      }

    case PLUGIN_TEN_PER_SECOND:
      {
        byte state = digitalRead(Settings.TaskDevicePin1[event->TaskIndex]);
        byte currentOutputState = state;
        if (Settings.TaskDevicePin1Inversed[event->TaskIndex]) {

          if (state != switchstate[event->TaskIndex])
          {
            switchstate[event->TaskIndex] = !state;
            currentOutputState = outputstate[event->TaskIndex];
          }
          else
          {
            switchstate[event->TaskIndex] = state;
            currentOutputState = outputstate[event->TaskIndex];
          }

          // send if output needs to be changed
          if (currentOutputState != outputstate[event->TaskIndex])
          {
            byte sendState = outputstate[event->TaskIndex];
            if (Settings.TaskDevicePin1Inversed[event->TaskIndex])
              sendState = !outputstate[event->TaskIndex];
            UserVar[event->BaseVarIndex] = sendState;
            event->sensorType = SENSOR_TYPE_RELAY;
            String log = F("SW   : State ");
            log += sendState;
            addLog(LOG_LEVEL_INFO, log);
            sendData(event);
          }
        }
        success = true;
        break;
      }

    case PLUGIN_WRITE:
      {
        String tmpString  = string;
        int8_t DeviceGPIO = 0;
        int8_t DeviceID = 0;
        int argIndex = tmpString.indexOf(',');
        if (argIndex)
          tmpString = tmpString.substring(0, argIndex);
        if (tmpString.equalsIgnoreCase("RELAY"))
        {
          success = true;
          DeviceID = getDeviceID(event->Par1);
          if (DeviceID > 0) {
            DeviceGPIO = Settings.TaskDevicePin1[DeviceID];
            //recherche du sensor ayant le'idx spécifié afin d'acitver le bon GPIO
            //Settings.TaskDevicePin1[event->Par1]

            pinMode(DeviceGPIO, OUTPUT);
            if (Settings.TaskDevicePin1Inversed[DeviceID]) {
              digitalWrite(DeviceGPIO, !event->Par2);
            } else {
              digitalWrite(DeviceGPIO, event->Par2);
            }
            if (printToWeb)
            {
              printWebString += F("Command : ");
              printWebString += event->Par1;
              printWebString += F(",");
              printWebString += event->Par2;
              printWebString += F("<BR>");
              printWebString += F("Device : ");
              printWebString += DeviceID;

              printWebString += F("<BR>");
              printWebString += F("Inverted : ");
              printWebString += Settings.TaskDevicePin1Inversed[DeviceID];

              printWebString += F("<BR>");
              printWebString += F("GPIO ");
              printWebString += DeviceGPIO;
              printWebString += F(" Set to ");
              printWebString += event->Par2;
              if (Settings.TaskDevicePin1Inversed[DeviceID]) {
                printWebString += F(", Real State : ");
                printWebString +=  !event->Par2;
              } else {
                printWebString +=  event->Par2;
              }
              printWebString += F("<BR>");
            }
          } else {
            if (printToWeb)
            {
              printWebString += F("Command : ");
              printWebString += event->Par1;
              printWebString += F(",");
              printWebString += event->Par2;
              printWebString += F("<BR>");
              printWebString += F("Device : ");
              printWebString += DeviceID;
              printWebString += F(" : device unkwon !<BR>");
            }
          }
        }


        break;
      }
  }
  return success;
}
/********************************************************************************************\
* Find device Number corresponding to task IDX setting
\*********************************************************************************************/
byte getDeviceID(unsigned int IDX)
{
  /*
  byte TaskDeviceNumber[TASKS_MAX];
  unsigned int  TaskDeviceID[TASKS_MAX];
  int8_t        TaskDevicePin1[TASKS_MAX];
   */
  byte DeviceID = 0;
  for (byte x = 0; x <= deviceCount ; x++)
    if (Settings.TaskDeviceID[x] == IDX)
      DeviceID = x;
  return DeviceID;
}
