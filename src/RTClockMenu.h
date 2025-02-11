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
  RTCAction(Menu &menu, const char *name, RTClock &rtclock);

 protected:

  RTClock &RTC;
};


class PrintRTCAction : public RTCAction {

 public:

  using RTCAction::RTCAction;

  /* Print the current time. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);
};


class ReportRTCAction : public RTCAction {

 public:

  using RTCAction::RTCAction;

  /* Report the time and the status of the real-time clock. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);
};


class SetRTCAction : public RTCAction {

 public:

  using RTCAction::RTCAction;

  /* Set the real-time clock. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);
};


class RTClockMenu : public Menu {

public:

  RTClockMenu(Menu &menu, RTClock &rtclock);

protected:

  PrintRTCAction PrintAct;
  ReportRTCAction ReportAct;
  SetRTCAction SetAct;
  
};


#endif
