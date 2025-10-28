#include <RTClock.h>
#include <RTClockMenu.h>


RTCAction::RTCAction(Menu &menu, const char *name, RTClock &rtclock,
		     unsigned int roles) :
  Action(menu, name, roles),
  RTC(rtclock) {
}


void PrintRTCAction::execute(Stream &stream, unsigned long timeout,
			     bool echo, bool detailed) {
  stream.print("Current time: ");
  RTC.print(stream);
  stream.println();
}


ReportRTCAction::ReportRTCAction(Menu &menu, const char *name,
				 RTClock &rtclock) :
  RTCAction(menu, name, rtclock, StreamInput | Report) {
}


void ReportRTCAction::report(Stream &stream, unsigned int roles,
			     size_t indent, size_t w, bool descend) const {
  if (disabled(roles))
    return;
  if (descend)
    RTC.report(stream, indent, indentation());
  else
    Action::report(stream, roles, indent, w, descend);
}


void SetRTCAction::execute(Stream &stream, unsigned long timeout,
			   bool echo, bool detailed) {
  RTC.set(stream);
  stream.println();
}


RTClockMenu::RTClockMenu(Menu &menu, RTClock &rtclock) :
  Menu(menu, "Date & time", Action::StreamInput),
  PrintAct(*this, "Print date & time", rtclock),
  ReportAct(*this, "Report date & time infos", rtclock),
  SetAct(*this, "Set date & time", rtclock) {
}

