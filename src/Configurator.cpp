#include <SDWriter.h>
#include <Configurator.h>


ReportAction::ReportAction(const Configurator *config) :
  Action("Print configuration", AllRoles),
  Config(config) {
  disableSupported(SetValue);  // Parameters should set this, and AllRoles should not include this.
}


void ReportAction::execute() {
  Config->Config->report(); // TODO: this should call a Configurator::report() function that then takes care on its own what to report.
  Serial.println();
}


SaveAction::SaveAction(const Configurator *config) :
  // TODO: need to add SDCard
  Action("Save configuration", AllRoles),
  Config(config) {
  disableSupported(SetValue);
}


void SaveAction::execute() {
  Serial.printf("Save configuration to '%s' ...", Config->configFile());
  //Config->save(SDC);
  Serial.println();
}


Configurable *Configurator::Config = NULL;


Configurator::Configurator() :
  Configurator("") {
}


Configurator::Configurator(const char *name) :
  Configurable(name),
  MActions(0),
  ConfigureAct("Configure"),
  ReportAct(this),
  SaveAct(this) {
  Config = this;
  strncpy(ConfigFile, "teerec.cfg", MaxFile);
  ConfigFile[MaxFile-1] = '\0';
  ConfigureAct.disableSupported(SetValue);
}


void Configurator::setConfigFile(const char *fname) {
  strncpy(ConfigFile, fname, MaxFile);
  ConfigFile[MaxFile-1] = '\0';
}


void Configurator::addConfigure() {
  for (size_t k=MActions; k<NActions; k++)
    ConfigureAct.add(Actions[k]);
  NActions = MActions;
  add(&ConfigureAct);
  MActions++;
  Config = &ConfigureAct;
}


void Configurator::addReport() {
  if (NActions >= MaxActions) {
    Serial.printf("ERROR! Number of maximum Actions exceeded in %s!\n",
		  name());
    return;
  }
  for (size_t k=NActions; k>MActions; k--)
    Actions[k] = Actions[k - 1];
  NActions++;
  Actions[MActions++] = &ReportAct;
}


void Configurator::addSave() {
  if (NActions >= MaxActions) {
    Serial.printf("ERROR! Number of maximum Actions exceeded in %s!\n",
		  name());
    return;
  }
  for (size_t k=NActions; k>MActions; k--)
    Actions[k] = Actions[k - 1];
  NActions++;
  Actions[MActions++] = &SaveAct;
}


bool Configurator::save(SDCard &sd) const {
  return Configurable::save(sd, ConfigFile);
}


void Configurator::configure(Stream &stream, unsigned long timeout) {
  Configurable::configure(stream, timeout);
}


void Configurator::configure(SDCard &sd) {
  Configurable::configure(sd, ConfigFile);
}

