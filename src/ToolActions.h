/*
  ToolActions - Actions for handling the configuration file and the real-time clock.
  Created by Jan Benda, August 13th, 2024.
*/

#ifndef ToolActions_h
#define ToolActions_h


#include <Action.h>
#include <Configurable.h>


class SDCard;
class RTClock;


class ConfigureAction : public Configurable {

 public:

  /* Initialize and add to default menu. */
  ConfigureAction(const char *name);
  
  /* Initialize and add to configuration menu. */
  ConfigureAction(const char *name, Configurable &menu);  
};


class ReportConfigAction : public Action {

 public:

  /* Initialize and add to default menu. */
  ReportConfigAction(const char *name);

  /* Initialize and add to configuration menu. */
  ReportConfigAction(const char *name, Configurable &menu);

  /* Report the configuration settings. */
  virtual void execute();  
};


class SDCardAction : public Action {

 public:

  /* Initialize and add to default menu. */
  SDCardAction(const char *name, SDCard &sd);

  /* Initialize and add to configuration menu. */
  SDCardAction(const char *name, SDCard &sd, Configurable &menu);

 protected:

  SDCard &SDC; 
};


class SaveConfigAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Save the configuration settings to configuration file. */
  virtual void execute();
};


class LoadConfigAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Load the configuration settings from configuration file. */
  virtual void execute();
};


class RemoveConfigAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Remove the configuration file from SD card. */
  virtual void execute();
};


class RTCAction : public Action {

 public:

  /* Initialize and add to default menu. */
  RTCAction(const char *name, RTClock &rtclock);

  /* Initialize and add to configuration menu. */
  RTCAction(const char *name, RTClock &rtclock, Configurable &menu);

 protected:

  RTClock &RTC;
};


class ReportRTCAction : public RTCAction {

 public:

  using RTCAction::RTCAction;

  /* Report the real-time clock. */
  virtual void execute();
};


class SetRTCAction : public RTCAction {

 public:

  using RTCAction::RTCAction;

  /* Set the real-time clock. */
  virtual void execute();
};


#endif
