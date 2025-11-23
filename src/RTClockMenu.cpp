#include <RTClock.h>
#include <RTClockMenu.h>


RTCAction::RTCAction(Menu &menu, const char *name, RTClock &rtclock) :
  Action(menu, name),
  RTC(rtclock) {
}


void PrintRTCAction::execute(Stream &stream) {
  stream.print("Current time: ");
  RTC.print(stream);
  stream.println();
}


ReportRTCAction::ReportRTCAction(Menu &menu, const char *name,
				 RTClock &rtclock) :
  RTCAction(menu, name, rtclock) {
}


void ReportRTCAction::write(Stream &stream, unsigned int roles,
			    size_t indent, size_t width) const {
  RTC.write(stream, indent, indentation());
}


void SetRTCAction::execute(Stream &stream) {
  RTC.set(stream);
  stream.println();
}


RTClockMenu::RTClockMenu(Menu &menu, RTClock &rtclock) :
  Menu(menu, "Date & time", StreamInput),
  PrintAct(*this, "Print date & time", rtclock),
  ReportAct(*this, "Report date & time infos", rtclock),
  SetAct(*this, "Set date & time", rtclock) {
}

