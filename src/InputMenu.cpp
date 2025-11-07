#include <Device.h>
#include <Input.h>
#include <InputSettings.h>
#include <InputMenu.h>


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


void ReportInputAction::execute(Stream &instream, Stream &outstream,
				unsigned long timeout, bool echo,
				bool detailed) {
  Data.reset();
  Settings.configure(&Data);
  if (Setupai != 0)
    Setupai(Data, Settings, Controls, NControls, outstream);
  if (!Data.check(0, outstream)) {
    outstream.println();
    return;
  }
  Data.begin();
  Data.start();
  Data.report(outstream);
  Data.stop();
  Data.reset();
}


void PrintInputAction::execute(Stream &instream, Stream &outstream,
			       unsigned long timeout, bool echo,
			       bool detailed) {
  Data.reset();
  Settings.configure(&Data);
  if (Setupai != 0)
    Setupai(Data, Settings, Controls, NControls, outstream);
  if (!Data.check(0, outstream)) {
    outstream.println();
    return;
  }
  int tmax = 100;
  outstream.print("Record some data ...");
  Data.begin();
  Data.start();
  delay(tmax + 10);
  Data.stop();
  outstream.println();
  size_t nframes = Data.index()/Data.nchannels();
  if (Data.frames(0.001*tmax) < nframes)
    nframes = Data.frames(0.001*tmax);
  outstream.printf("Sampling rate: %dHz", Data.rate());
  outstream.println();
  outstream.printf("Resolution: %ubits", Data.dataResolution());
  outstream.println();
  char gs[16];
  Data.gainStr(gs, 16);
  outstream.print("Gain: ");
  outstream.println(gs);
  Data.printData(0, nframes, outstream);
  outstream.println();
}


void StartInputAction::execute(Stream &instream, Stream &outstream,
			       unsigned long timeout, bool echo,
			       bool detailed) {
  Data.reset();
  Settings.configure(&Data);
  if (Setupai != 0)
    Setupai(Data, Settings, Controls, NControls, outstream);
  if (!Data.check(0, outstream)) {
    outstream.println();
    return;
  }
  outstream.println("Start recording ...");
  Data.begin();
  Data.start();
  outstream.println();
}


void GetInputAction::execute(Stream &instream, Stream &outstream,
			     unsigned long timeout, bool echo,
			     bool detailed) {
  int tmax = 100;
  outstream.println("Get data ...");
  if (!Data.running()) {
    outstream.println("    ERROR: recording not running!");
    outstream.println();
    return;
  }
  size_t nframes = Data.frames(0.001*tmax);
  size_t start = Data.currentSample(nframes);
  outstream.printf("Sampling rate: %dHz", Data.rate());
  outstream.println();
  outstream.printf("Resolution: %ubits", Data.dataResolution());
  outstream.println();
  char gs[16];
  Data.gainStr(gs, 16);
  outstream.print("Gain: ");
  outstream.println(gs);
  Data.printData(start, nframes, outstream);
  outstream.println();
}


void StopInputAction::execute(Stream &instream, Stream &outstream,
			      unsigned long timeout, bool echo,
			      bool detailed) {
  outstream.println("Stop recording ...");
  Data.stop();
  outstream.println();
}


InputMenu::InputMenu(Menu &menu, Input &data, InputSettings &settings,
		     Device** controls, size_t ncontrols, SetupAI setupai) :
  Menu(menu, "Analog input", Action::StreamInput),
  ReportAct(*this, "Report input configuration", data, settings,
	    controls, ncontrols, setupai),
  PrintAct(*this, "Record some data", data, settings,
	   controls, ncontrols, setupai),
  StartAct(*this, "Start recording", data, settings,
	   controls, ncontrols, setupai),
  GetAct(*this, "Get data from running recording", data, settings,
	   controls, ncontrols, setupai),
  StopAct(*this, "Stop recording", data, settings,
	   controls, ncontrols, setupai) {
}

