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
  ConfigureAction(const char *name, int roles=StreamInput);
  
  /* Initialize and add to configuration menu. */
  ConfigureAction(Configurable &menu, const char *name, int roles=StreamInput);
};


class ReportConfigAction : public Action {

 public:

  /* Initialize and add to default menu. */
  ReportConfigAction(const char *name);

  /* Initialize and add to configuration menu. */
  ReportConfigAction(Configurable &menu, const char *name);

  /* Report the configuration settings. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);  
};


class SDCardAction : public Action {

 public:

  /* Initialize and add to default menu. */
  SDCardAction(const char *name, SDCard &sd);

  /* Initialize and add to configuration menu. */
  SDCardAction(Configurable &menu, const char *name, SDCard &sd);

 protected:

  SDCard &SDC; 
};


class SaveConfigAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Save the configuration settings to configuration file. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);
};


class LoadConfigAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Load the configuration settings from configuration file. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);
};


class RemoveConfigAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Remove the configuration file from SD card. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);
};


class RTCAction : public Action {

 public:

  /* Initialize and add to default menu. */
  RTCAction(const char *name, RTClock &rtclock);

  /* Initialize and add to configuration menu. */
  RTCAction(Configurable &menu, const char *name, RTClock &rtclock);

 protected:

  RTClock &RTC;
};


class ReportRTCAction : public RTCAction {

 public:

  using RTCAction::RTCAction;

  /* Report the real-time clock. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);
};


class SetRTCAction : public RTCAction {

 public:

  using RTCAction::RTCAction;

  /* Set the real-time clock. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);
};


#endif
