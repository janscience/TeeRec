/*
  Parameter - base class for a configurable key-value pair.
  Created by Jan Benda, October 22, 2023.
*/

#ifndef Parameter_h
#define Parameter_h


#include <Arduino.h>


class Parameter {

 public:

  /* Initialize parameter with identifying key. */
  Parameter(const char *key);

  /* The key identifying the parameter. */
  const char *key() const { return Key; }

  /* Set the key identifying the parameter to key. */
  void setKey(const char *key);

  /* True if this parameter can be confogured. */
  bool configurable() const { return Configurable; };

  /* Make this parameter non-configurable by passing false. */
  void setConfigurable(bool configurable);

  /* Parse the string val and set the value of this parameter accordingly. */
  virtual void parseValue(const char *val) = 0;

  /* Return the current value of this parameter as a string. */
  virtual void valueStr(char *str) = 0;
  

 protected:

  static const int MaxStr = 63;
  char Key[MaxStr + 1];

  bool Configurable;
  
};


class StringParameter : public Parameter {
  
 public:
  
  /* Initialize parameter with identifying key and pointer to value variable. */
  StringParameter(const char *key, char **str);
  
  /* Parse the string val and set the value of this parameter accordingly. */
  virtual void parseValue(const char *val);

  /* Return the current value of this parameter as a string. */
  virtual void valueStr(char *str);

  
 protected:

  char **Str;
  
};


class NumberParameter : public Parameter {
  
 public:
  
  /* Initialize parameter with identifying key and a format string. */
  NumberParameter(const char *key, const char *format);

  /* The format string for formatting a number. */
  const char *format() const { return Format; };

  /* Set the format string to format. */
  void setFormat(const char *format);

  
 protected:

  static const int MaxFmt = 15;
  char Format[MaxFmt+1];
  
};


class IntegerParameter : public NumberParameter {
  
 public:
  
  /* Initialize parameter with identifying key, pointer to value variable,
     and format. */
  IntegerParameter(const char *key, int *integer, const char *format);
  
  /* Parse the string val and set the value of this parameter accordingly. */
  virtual void parseValue(const char *val);

  /* Return the current value of this parameter as a string. */
  virtual void valueStr(char *str);

  
 protected:

  int *Integer;
  
};


class TimeParameter : public NumberParameter {
  
 public:
  
  /* Initialize parameter with identifying key, pointer to value variable,
     and format. */
  TimeParameter(const char *key, float *time, const char *format);
  
  /* Parse the string val and set the value of this parameter accordingly. */
  virtual void parseValue(const char *val);

  /* Return the current value of this parameter as a string. */
  virtual void valueStr(char *str);

  
 protected:

  float *Time;
  
};


class FrequencyParameter : public NumberParameter {
  
 public:
  
  /* Initialize parameter with identifying key, pointer to value variable,
     and format. */
  FrequencyParameter(const char *key, float *frequency, const char *format);
  
  /* Parse the string val and set the value of this parameter accordingly. */
  virtual void parseValue(const char *val);

  /* Return the current value of this parameter as a string. */
  virtual void valueStr(char *str);

  
 protected:

  float *Frequency;
  
};


#endif
