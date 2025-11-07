#include <RTClock.h>
#include <RTClockMenu.h>


RTCAction::RTCAction(Menu &menu, const char *name, RTClock &rtclock,
		     unsigned int roles) :
  Action(menu, name, roles),
  RTC(rtclock) {
}


void PrintRTCAction::execute(Stream &instream, Stream &outstream,
			     unsigned long timeout, bool echo,
			     bool detailed) {
  outstream.print("Current time: ");
  RTC.print(outstream);
  outstream.println();
}


ReportRTCAction::ReportRTCAction(Menu &menu, const char *name,
				 RTClock &rtclock) :
  RTCAction(menu, name, rtclock) {
}


void ReportRTCAction::write(Stream &stream, unsigned int roles,
			    size_t indent, size_t width, bool descend) const {
  if (disabled(roles))
    return;
  if (descend)
    RTC.write(stream, indent, indentation());
  else
    Action::write(stream, roles, indent, width, descend);
}


void SetRTCAction::execute(Stream &instream, Stream &outstream,
			   unsigned long timeout, bool echo,
			   bool detailed) {
  RTC.set(instream, outstream);
  outstream.println();
}


RTClockMenu::RTClockMenu(Menu &menu, RTClock &rtclock) :
  Menu(menu, "Date & time", Action::StreamInput),
  PrintAct(*this, "Print date & time", rtclock),
  ReportAct(*this, "Report date & time infos", rtclock),
  SetAct(*this, "Set date & time", rtclock) {
}

