/*
  Parameter - base class for a configurable key-value pair.
  Created by Jan Benda, October 22, 2023.
*/

#ifndef Parameter_h
#define Parameter_h


#include <Arduino.h>


class Configurable;


class Parameter {

 public:

  /* Initialize parameter with identifying key and add it to cfg. */
  Parameter(Configurable *cfg, const char *key);

  /* The key identifying the parameter. */
  const char *key() const { return Key; }

  /* Set the key identifying the parameter to key. */
  void setKey(const char *key);

  /* True if this parameter is enabled for being configured. */
  bool enabled() const { return Enabled; };

  /* True if this parameter is disabled for being configured. */
  bool disabled() const { return !Enabled; };

  /* Make this parameter configurable (default). */
  void enable();

  /* Make this parameter non-configurable. */
  void disable();

  /* Parse the string val and set the value of this parameter accordingly. */
  virtual void parseValue(const char *val) = 0;

  /* Return the current value of this parameter as a string of maximum
     size MaxVal. */
  virtual void valueStr(char *str) = 0;

  static const size_t MaxVal = 64;
  

 protected:

  static const size_t MaxKey = 63;
  char Key[MaxKey + 1];

  bool Enabled;
  
};


class StringParameter : public Parameter {
  
 public:
  
  /* Initialize parameter with identifying key, pointer str to value variable
     and maximum size n and add to cfg. */
  StringParameter(Configurable *cfg, const char *key, char **str, size_t n);
  
  /* Parse the string val and set the value of this parameter accordingly. */
  virtual void parseValue(const char *val);

  /* Return the current value of this parameter as a string. */
  virtual void valueStr(char *str);

  
 protected:

  size_t MaxStr;
  char **Str;
  
};


template<class T>
class NumberParameter : public Parameter {
  
 public:
  
  /* Initialize parameter with identifying key, pointer number to value,
     format string, and unit and add to cfg. */
  NumberParameter(Configurable *cfg, const char *key, T *number,
		  const char *format, const char *unit=0);

  /* The format string for formatting a number. */
  const char *format() const { return Format; };

  /* Set the format string to format. */
  void setFormat(const char *format);

  /* The unit string. */
  const char *unit() const { return Unit; };

  /* Set the unit string to unit. */
  void setUnit(const char *unit);
  
  /* Parse the string val and set the value of this parameter accordingly. */
  virtual void parseValue(const char *val);

  /* Return the current value of this parameter as a string */
  virtual void valueStr(char *str);
  
  
 protected:

  static const size_t MaxFmt = 15;
  char Format[MaxFmt+1];
  
  static const size_t MaxUnit = 15;
  char Unit[MaxUnit+1];

  T *Number;
  
};


template<class T>
class TimeParameter : public NumberParameter<T> {
  
 public:
  
  /* Initialize parameter with identifying key, pointer time to value variable,
     and format and add to cfg. */
  TimeParameter(Configurable *cfg, const char *key, T *time,
		const char *format);
  
  /* Parse the string val and set the value of this parameter accordingly. */
  virtual void parseValue(const char *val);
  
};


template<class T>
class FrequencyParameter : public NumberParameter<T> {
  
 public:
  
  /* Initialize parameter with identifying key, pointer frequency to
     value variable, and format and add to cfg. */
  FrequencyParameter(Configurable *cfg, const char *key, T *frequency,
		     const char *format);
  
  /* Parse the string val and set the value of this parameter accordingly. */
  virtual void parseValue(const char *val);
  
};


template<class T>
NumberParameter<T>::NumberParameter(Configurable *cfg, const char *key,
				    T *number, const char *format,
				    const char *unit) :
  Parameter(cfg, key),
  Number(number) {
  setFormat(format);
  setUnit(unit);
}


template<class T>
void NumberParameter<T>::setFormat(const char *format) {
  strncpy(Format, format, MaxFmt);
}


template<class T>
void NumberParameter<T>::setUnit(const char *unit) {
  if (unit != NULL)
    strncpy(Unit, unit, MaxUnit);
}


template<class T>
void NumberParameter<T>::parseValue(const char *val) {
  if (disabled())
    return;
  float num = atof(val);
  *Number = (T)num;
}


template<class T>
void NumberParameter<T>::valueStr(char *str) {
  sprintf(str, Format, *Number);
  if (Unit != 0)
    strcat(str, Unit);
}


template<class T>
TimeParameter<T>::TimeParameter(Configurable *cfg, const char *key,
				T *time, const char *format) :
  NumberParameter<T>(cfg, key, time, format, "s") {
}


template<class T>
void TimeParameter<T>::parseValue(const char *val) {
  if (this->disabled())
    return;
  float time = atof(val);
  for (size_t k=0; k<strlen(val); k++) {
    char c = tolower(val[k]);
    if (c == 's')
      break;
    else if (c == 'h') {
      time *= 60.0*60.0;
      break;
    }
    else if (c == 'm' && k<strlen(val)-1 && tolower(val[k+1]) == 'i') {
      time *= 60.0;
      break;
    }
    else if (c == 'm' && k<strlen(val)-1 && tolower(val[k+1]) == 's') {
      time *= 0.001;
      break;
    }
    else if (c == 'u' && k<strlen(val)-1 && tolower(val[k+1]) == 's') {
      time *= 1e-6;
      break;
    }
    else if (c == 'n' && k<strlen(val)-1 && tolower(val[k+1]) == 's') {
      time *= 1e-9;
      break;
    }
  }
  *(this->Number) = (T)time;
}


template<class T>
FrequencyParameter<T>::FrequencyParameter(Configurable *cfg, const char *key,
					  T *frequency, const char *format) :
  NumberParameter<T>(cfg, key, frequency, format, "Hz") {
}


template<class T>
void FrequencyParameter<T>::parseValue(const char *val) {
  if (this->disabled())
    return;
  float freq = atof(val);
  for (size_t k=0; k<strlen(val); k++) {
    char c = tolower(val[k]);
    if (c == 'h' && k<strlen(val)-1 && tolower(val[k+1]) == 'z')
      break;
    else if (c == 'k' && k<strlen(val)-1 && tolower(val[k+1]) == 'h') {
      freq *= 1000.0;
      break;
    }
    else if (val[k] == 'M' && k<strlen(val)-1 && tolower(val[k+1]) == 'h') {
      freq *= 1e6;
      break;
    }
    else if (c == 'm' && k<strlen(val)-1 && tolower(val[k+1]) == 'h') {
      freq *= 0.001;
      break;
    }
  }
  *(this->Number) = (T)freq;
}

#endif
