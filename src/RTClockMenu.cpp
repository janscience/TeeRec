#include <RTClock.h>
#include <RTClockMenu.h>


RTCAction::RTCAction(const char *name, RTClock &rtclock) :
  RTCAction(*root()->Config, name, rtclock) {
}


RTCAction::RTCAction(Menu &menu, const char *name, RTClock &rtclock) :
  Action(menu, name, StreamInput),
  RTC(rtclock) {
}


void PrintRTCAction::execute(Stream &stream, unsigned long timeout,
			     bool echo, bool detailed) {
  stream.print("Current time: ");
  RTC.print(stream);
  stream.println();
}


void ReportRTCAction::execute(Stream &stream, unsigned long timeout,
			      bool echo, bool detailed) {
  RTC.report(stream);
}


void SetRTCAction::execute(Stream &stream, unsigned long timeout,
			   bool echo, bool detailed) {
  RTC.set(stream);
  stream.println();
}


RTClockMenu::RTClockMenu(RTClock &rtclock) :
  Menu("Date & time", Action::StreamInput),
  PrintAct(*this, "Print date & time", rtclock),
  ReportAct(*this, "Report date & time infos", rtclock),
  SetAct(*this, "Set date & time", rtclock) {
}

