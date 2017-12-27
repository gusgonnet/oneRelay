// Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
// This is a human-readable summary of (and not a substitute for) the license.
// Disclaimer
//
// You are free to:
// Share — copy and redistribute the material in any medium or format
// Adapt — remix, transform, and build upon the material
// The licensor cannot revoke these freedoms as long as you follow the license terms.
//
// Under the following terms:
// Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
// NonCommercial — You may not use the material for commercial purposes.
// ShareAlike — If you remix, transform, or build upon the material, you must distribute your contributions under the same license as the original.
// No additional restrictions — You may not apply legal terms or technological measures that legally restrict others from doing anything the license permits.
//
// Notices:
// You do not have to comply with the license for elements of the material in the public domain or where your use is permitted by an applicable exception or limitation.
// No warranties are given. The license may not give you all of the permissions necessary for your intended use. For example, other rights such as publicity, privacy, or moral rights may limit how you use the material.
//
// github: https://github.com/gusgonnet/oneRelay
// hackster: https://www.hackster.io/gusgonnet/christmas-tree-928462
//
// Free for personal use.
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/

// Used in this project:
// 1 wemos relay - https://wiki.wemos.cc/products:d1_mini_shields:relay_shield
// 1 particle core/photon

#define APP_NAME "oneRelay"
#define VERSION "0.02"

/*******************************************************************************
 * changes in version 0.01:
       * Initial version
 * changes in version 0.02:
       * saving status in eeprom to be more robust in resets
*******************************************************************************/

//enable the user code (our program below) to run in parallel with cloud connectivity code
// source: https://docs.particle.io/reference/firmware/photon/#system-thread
SYSTEM_THREAD(ENABLED);

const String timeOn = "07:00AM";
const String timeOff = "08:00PM";

#define STATUS_ON "on"
#define STATUS_OFF "off"

String status = STATUS_OFF;

/*******************************************************************************
 structure for writing thresholds in eeprom
 https://docs.particle.io/reference/firmware/photon/#eeprom
*******************************************************************************/
// randomly chosen value here. The only thing that matters is that it's not 255
// since 255 is the default value for uninitialized eeprom
// value 112 will be used in version 0.2
#define EEPROM_VERSION 112
#define EEPROM_ADDRESS 0

struct EepromMemoryStructure
{
  uint8_t version = EEPROM_VERSION;
  uint8_t status;
};
EepromMemoryStructure eepromMemory;

/*******************************************************************************
 * Function Name  : setup
 * Description    : this function runs once at system boot
 *******************************************************************************/
void setup()
{
  // publish startup message with firmware version
  Particle.publish(APP_NAME, VERSION, PRIVATE);

  // declare cloud functions
  // https://docs.particle.io/reference/firmware/photon/#particle-function-
  // Up to 15 cloud functions may be registered and each function name is limited to a maximum of 12 characters.
  Particle.function("turnOff", turnOff);
  Particle.function("turnOn", turnOn);

  // declare cloud variables
  // https://docs.particle.io/reference/firmware/photon/#particle-variable-
  // Up to 20 cloud variables may be registered and each variable name is limited to a maximum of 12 characters.
  Particle.variable("status", status);

  //set time zone and format
  Time.zone(-5);
  Time.setFormat(TIME_FORMAT_ISO8601_FULL);

  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);

  readFromEeprom();
  if (status == STATUS_ON)
  {
    digitalWrite(A0, HIGH);
  }
}

/*******************************************************************************
 * Function Name  : loop
 * Description    : this function runs continuously while the project is running
 *******************************************************************************/
void loop()
{

  // get time. Example: 10:38PM
  String timeNow = Time.format(Time.now(), "%I:%M%p");

  if (timeNow == timeOn)
  {
    digitalWrite(A0, HIGH);
    saveSettingsInEeprom();
    status = STATUS_ON;
  }
  if (timeNow == timeOff)
  {
    digitalWrite(A0, LOW);
    saveSettingsInEeprom();
    status = STATUS_OFF;
  }
}

int turnOn(String dummy)
{
  digitalWrite(A0, HIGH);
  status = STATUS_ON;
  saveSettingsInEeprom();
  return 0;
}

int turnOff(String dummy)
{
  digitalWrite(A0, LOW);
  status = STATUS_OFF;
  saveSettingsInEeprom();
  return 0;
}

/*******************************************************************************/
/*******************************************************************************/
/*******************          EEPROM FUNCTIONS         *************************/
/********  https://docs.particle.io/reference/firmware/photon/#eeprom         **/
/********                                                                     **/
/********  wear and tear discussion:                                          **/
/********  https://community.particle.io/t/eeprom-flash-wear-and-tear/23738/5 **/
/**                                                                           **/
/*******************************************************************************/
/*******************************************************************************/

/*******************************************************************************
 * Function Name  : readFromEeprom
 * Description    : retrieves the settings from the EEPROM memory
 * Return         : none
 *******************************************************************************/
void readFromEeprom()
{

  EepromMemoryStructure myObj;
  EEPROM.get(EEPROM_ADDRESS, myObj);

  //verify this eeprom was written before
  // if version is 255 it means the eeprom was never written in the first place, hence the
  // data just read with the previous EEPROM.get() is invalid and we will ignore it
  if (myObj.version == EEPROM_VERSION)
  {
    status = convertIntToStatus(myObj.status);
    Particle.publish(APP_NAME, "Read settings from EEPROM", PRIVATE);
  }
}

/*******************************************************************************
 * Function Name  : saveSettingsInEeprom
 * Description    : save info to eeprom
                    Remember that each eeprom writing cycle is a precious and finite resource
 * Return         : none
 *******************************************************************************/
void saveSettingsInEeprom()
{

  //store thresholds in the struct type that will be saved in the eeprom
  eepromMemory.version = EEPROM_VERSION;
  eepromMemory.status = convertStatusToInt(status);

  //then save
  EEPROM.put(EEPROM_ADDRESS, eepromMemory);

  Particle.publish(APP_NAME, "Stored settings on EEPROM", PRIVATE);
}

/*******************************************************************************
 * Function Name  : convertIntToStatus
 * Description    : converts the status int (saved in the eeprom) into the String variable (in RAM)
 * Return         : String
 *******************************************************************************/
String convertIntToStatus(uint8_t integ)
{
  if (integ == 1)
  {
    return STATUS_ON;
  }

  //in all other cases
  return STATUS_OFF;
}

/*******************************************************************************
 * Function Name  : convertCalibratedToInt
 * Description    : converts the String calibrated (in RAM) into the int calibrated (to be saved in the eeprom)
 * Return         : String
 *******************************************************************************/
uint8_t convertStatusToInt(String stat)
{
  if (stat == STATUS_ON)
  {
    return 1;
  }

  //in all other cases
  return 0;
}
