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
// github: https://github.com/gusgonnet/
// hackster: https://www.hackster.io/gusgonnet
//
// Free for personal use.
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/

// Used in this project:
// 1 wemos relay - https://wiki.wemos.cc/products:d1_mini_shields:relay_shield
// 1 particle core/photon

#define APP_NAME "oneRelay"
#define VERSION "0.01"

/*******************************************************************************
 * changes in version 0.01:
       * Initial version
*******************************************************************************/

//enable the user code (our program below) to run in parallel with cloud connectivity code
// source: https://docs.particle.io/reference/firmware/photon/#system-thread
SYSTEM_THREAD(ENABLED);

const String timeOn = "07:00AM";
const String timeOff = "08:00PM";

String status = "off";

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
    status = "on";
  }
  if (timeNow == timeOff)
  {
    digitalWrite(A0, LOW);
    status = "off";
  }
}

int turnOn(String dummy)
{
  digitalWrite(A0, HIGH);
  status = "on";
  return 0;
}

int turnOff(String dummy)
{
  digitalWrite(A0, LOW);
  status = "off";
  return 0;
}
