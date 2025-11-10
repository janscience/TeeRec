/*
  RTClockMenu - Actions and menu for reading and setting the real-time clock.
  Created by Jan Benda, February 9th, 2025.
*/

#ifndef RTClockMenu_h
#define RTClockMenu_h


#include <MicroConfig.h>


class RTClock;


class RTCAction : public Action {

 public:

  /* Initialize and add to menu. */
  RTCAction(Menu &menu, const char *name, RTClock &rtclock,
	    unsigned int roles=StreamInput);

 protected:

  RTClock &RTC;
};


class PrintRTCAction : public RTCAction {

 public:

  using RTCAction::RTCAction;

  /* Print the current time. */
  virtual void execute(Stream &stream=Serial);
};


class ReportRTCAction : public RTCAction {

 public:


  /* Initialize and add to menu. */
  ReportRTCAction(Menu &menu, const char *name, RTClock &rtclock);

  /* Report the time and the status of the real-time clock. */
  virtual void write(Stream &stream=Serial, unsigned int roles=AllRoles,
		     size_t indent=0, size_t width=0, bool descend=true) const;
};


class SetRTCAction : public RTCAction {

 public:

  using RTCAction::RTCAction;

  /* Set the real-time clock. */
  virtual void execute(Stream &stream=Serial);
};


class RTClockMenu : public Menu {

public:

  RTClockMenu(Menu &menu, RTClock &rtclock);

  PrintRTCAction PrintAct;
  ReportRTCAction ReportAct;
  SetRTCAction SetAct;
  
};


#endif
