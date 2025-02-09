#include <Device.h>
#include <Input.h>
#include <InputSettings.h>
#include <InputMenu.h>


InputAction::InputAction(const char *name, Input &data,
			 InputSettings &settings,
			 Device** controls, size_t ncontrols,
			 SetupAI setupai) :
  InputAction(*root()->Config, name, data, settings,
	      controls, ncontrols, setupai) {
}


InputAction::InputAction(Menu &menu, const char *name,
			 Input &data, InputSettings &settings,
			 Device** controls, size_t ncontrols,
			 SetupAI setupai) :
  Action(menu, name, StreamInput),
  Data(data),
  Settings(settings),
  Controls(controls),
  NControls(ncontrols),
  Setupai(setupai) {
}


void ReportInputAction::execute(Stream &stream, unsigned long timeout,
				bool echo, bool detailed) {
  Data.reset();
  Settings.configure(&Data);
  if (Setupai != 0)
    Setupai(Data, Settings, Controls, NControls, stream);
  if (!Data.check(0, stream)) {
    stream.println();
    return;
  }
  Data.begin();
  Data.start();
  Data.report(stream);
  Data.stop();
  Data.reset();
}


void PrintInputAction::execute(Stream &stream, unsigned long timeout,
			       bool echo, bool detailed) {
  Data.reset();
  Settings.configure(&Data);
  if (Setupai != 0)
    Setupai(Data, Settings, Controls, NControls, stream);
  if (!Data.check(0, stream)) {
    stream.println();
    return;
  }
  int tmax = 100;
  stream.print("Record some data ...");
  Data.begin();
  Data.start();
  delay(tmax + 10);
  Data.stop();
  stream.println();
  size_t nframes = Data.index()/Data.nchannels();
  if (Data.frames(0.001*tmax) < nframes)
    nframes = Data.frames(0.001*tmax);
  stream.printf("Sampling rate: %dHz", Data.rate());
  stream.println();
  stream.printf("Resolution: %ubits", Data.dataResolution());
  stream.println();
  char gs[32];
  Data.gainStr(gs);
  stream.print("Gain: ");
  stream.println(gs);
  Data.printData(0, nframes, stream);
  stream.println();
}


InputMenu::InputMenu(Input &data, InputSettings &settings,
		     Device** controls, size_t ncontrols, SetupAI setupai) :
  Menu("Analog input", Action::StreamInput),
  ReportAct(*this, "Report input configuration", data, settings,
	    controls, ncontrols, setupai),
  PrintAct(*this, "Record some data", data, settings,
	   controls, ncontrols, setupai) {
}

