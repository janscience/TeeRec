/*
  Parameter - base class for a configurable key-value pair.
  Created by Jan Benda, October 22, 2023.
*/

#ifndef Parameter_h
#define Parameter_h


#include <Arduino.h>


class File;
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

  /* Parse the string val and set the value of this parameter accordingly.
     Return true if val was a valid string or the parameter was disabled. */
  virtual bool parseValue(const char *val) = 0;

  /* Return the current value of this parameter as a string of maximum
     size MaxVal. */
  virtual void valueStr(char *str) const = 0;

  /* List selection of valid values. */
  virtual void listSelection(Stream &stream) const {};
  
  /* Interactive configuration via Serial stream. */
  void configure(Stream &stream=Serial, unsigned long timeout=0);

  /* Parse the string val, set the parameter accordingly and report
     result together with name on Serial. */
  void configure(const char *val, const char *name=0);

  /* Report the parameter's key and value on Serial. */
  void report(int w=0) const;

  /* Save the parameter's key and value to file. */
  void save(File &file, int w=0) const;

  /* Maximum size of string needed for valueStr() */
  static const size_t MaxVal = 64;

  /* Convert val with oldunit to newunit. */
  static float changeUnit(float val, const char *oldunit, const char *newunit);
  
  
 protected:

  static const size_t MaxKey = 64;
  char Key[MaxKey];

  bool Enabled;

  size_t NSelection;
  
  static const int NUnits = 50;
  static char UnitPref[NUnits][6];
  static float UnitFac[NUnits];
  
};


class BaseStringParameter : public Parameter {

  /* Parameter with character array as value. */
  
 public:
  
  /* Initialize parameter with identifying key and add to cfg. */
  BaseStringParameter(Configurable *cfg, const char *key);

  /* Provide a selection of n input values. */
  void setSelection(const char **selection, size_t n);

  /* Check whether val matches a string of the selection. Return
     index of matching selection, 0 when there is no selection, and -1
     if no match was found. */
  int checkSelection(const char *val);

  /* List selection of valid values. */
  virtual void listSelection(Stream &stream) const;

  
 protected:

  const char **Selection;
  
};


template<int N>
class StringParameter : public BaseStringParameter {

  /* Parameter owning a character array. */
  
 public:
  
  /* Initialize parameter with identifying key, value and add to cfg. */
  StringParameter(Configurable *cfg, const char *key, const char str[N]);

  /* Return the string. */
  const char* value() const { return Value; };

  /* Set the string to val.
     Return true if val was a valid string or the parameter was disabled. */
  bool setValue(const char *val) { return parseValue(val); };
  
  /* Parse the string val and set the value of this parameter accordingly.
     Return true if val was a valid string or the parameter was disabled. */
  virtual bool parseValue(const char *val);

  /* Return the current value of this parameter as a string. */
  virtual void valueStr(char *str) const;

  
 protected:

  char Value[N];
  
};


template<int N>
class StringPointerParameter : public BaseStringParameter {

  /* Parameter with a pointer to a character array. */

 public:
  
  /* Initialize parameter with identifying key, pointer str to value variable
     and add to cfg. */
  StringPointerParameter(Configurable *cfg, const char *key, char (*str)[N]);

  /* Return the string. */
  const char* value() const { return *Value; };

  /* Set the string to val.
     Return true if val was a valid string or the parameter was disabled. */
  bool setValue(const char *val) { return parseValue(val); };
  
  /* Parse the string val and set the value of this parameter accordingly.
     Return true if val was a valid string or the parameter was disabled. */
  virtual bool parseValue(const char *val);

  /* Return the current value of this parameter as a string. */
  virtual void valueStr(char *str) const;

  
 protected:

  char (*Value)[N];
  
};


template<class T>
class BaseNumberParameter : public Parameter {
  
 public:
  
  /* Initialize parameter with identifying key,
     format string, and unit and add to cfg. */
  BaseNumberParameter(Configurable *cfg, const char *key,
		      const char *format, const char *unit=0,
		      const char *outunit=0);

  /* The format string for formatting a number. */
  const char *format() const { return Format; };

  /* Set the format string to format.  If the number has a unit, then
     this is a format for a float, irrespective of the type of the
     number. Without a unit this is a format for the type of the number. */
  void setFormat(const char *format);

  /* The unit string of the internal unit of the value, i.e. value(). */
  const char *unit() const { return Unit; };

  /* Set the internal unit string to unit. */
  void setUnit(const char *unit);

  /* The unit string used forstring representations of the value,
     i.e. valueStr(). */
  const char *outunit() const { return OutUnit; };

  /* Set the unit string for the string representation of the value to unit. */
  void setOutUnit(const char *unit);

  /* Provide a selection of n input values. */
  void setSelection(const T *selection, size_t n);

  /* Check whether val matches a string of the selection. Return
     index of matching selection, 0 when there is no selection, and -1
     if no match was found. */
  int checkSelection(T val);

  /* List selection of valid values. */
  virtual void listSelection(Stream &stream) const;
  
  
 protected:

  static const size_t MaxFmt = 16;
  char Format[MaxFmt];
  
  static const size_t MaxUnit = 16;
  char Unit[MaxUnit];
  char OutUnit[MaxUnit];

  const T *Selection;
  
};


template<class T>
class NumberParameter : public BaseNumberParameter<T> {
  
 public:
  
  /* Initialize parameter with identifying key, pointer number to value,
     format string, and unit and add to cfg. */
  NumberParameter(Configurable *cfg, const char *key, T number,
		  const char *format, const char *unit=0,
		  const char *outunit=0);

  /* Return the value of the number in its unit(). */
  T value() const { return Value; };

  /* Return the value of the number in an alternative unit. */
  T value(const char *unit) const;

  /* Set the number to val. */
  void setValue(T val);

  /* Set the number to val unit. */
  void setValue(T val, const char *unit);
  
  /* Parse the string val and set the value of this parameter accordingly.
     Return true if val was a valid string or the parameter was disabled. */
  virtual bool parseValue(const char *val);

  /* Return the current value of this parameter as a string */
  virtual void valueStr(char *str) const;
  
  
 protected:

  T Value;
  
};


template<class T>
class NumberPointerParameter : public BaseNumberParameter<T> {
  
 public:
  
  /* Initialize parameter with identifying key, pointer number to value,
     format string, and unit and add to cfg. */
  NumberPointerParameter(Configurable *cfg, const char *key, T *number,
			 const char *format, const char *unit=0,
			 const char *outunit=0);

  /* Return the value of the number. */
  T value() const { return *Value; };

  /* Return the value of the number in an alternative unit. */
  T value(const char *unit) const;

  /* Set the number to val. */
  void setValue(T val);

  /* Set the number to val unit. */
  void setValue(T val, const char *unit);
  
  /* Parse the string val and set the value of this parameter accordingly.
     Return true if val was a valid string or the parameter was disabled. */
  virtual bool parseValue(const char *val);

  /* Return the current value of this parameter as a string */
  virtual void valueStr(char *str) const;
  
  
 protected:

  T *Value;
  
};


template<int N>
StringParameter<N>::StringParameter(Configurable *cfg, const char *key,
				    const char str[N]) :
  BaseStringParameter(cfg, key) {
  strncpy(Value, str, N);
  Value[N-1] = '\0';
}


template<int N>
bool StringParameter<N>::parseValue(const char *val) {
  if (disabled())
    return true;
  if (checkSelection(val) < 0)
    return false;
  strncpy(Value, val, N);
  Value[N-1] = '\0';
  return true;
}


template<int N>
void StringParameter<N>::valueStr(char *str) const {
  int n = MaxVal < N ? MaxVal : N;
  strncpy(str, Value, n);
  str[n-1] = '\0';
}


template<int N>
StringPointerParameter<N>::StringPointerParameter(Configurable *cfg,
						  const char *key,
						  char (*str)[N]) :
  BaseStringParameter(cfg, key),
  Value(str) {
}


template<int N>
bool StringPointerParameter<N>::parseValue(const char *val) {
  if (disabled())
    return true;
  if (checkSelection(val) < 0)
    return false;
  strncpy(*Value, val, N);
  (*Value)[N-1] = '\0';
  return true;
}


template<int N>
void StringPointerParameter<N>::valueStr(char *str) const {
  int n = MaxVal < N ? MaxVal : N;
  strncpy(str, *Value, n);
  str[n-1] = '\0';
}


template<class T>
BaseNumberParameter<T>::BaseNumberParameter(Configurable *cfg, const char *key,
					    const char *format,
					    const char *unit,
					    const char *outunit) :
  Parameter(cfg, key),
  Format(""),
  Unit(""),
  OutUnit(""),
  Selection(0) {
  setFormat(format);
  setUnit(unit);
  setOutUnit(outunit);
  if (strlen(OutUnit) == 0)
    setOutUnit(unit);
}


template<class T>
void BaseNumberParameter<T>::setFormat(const char *format) {
  strncpy(Format, format, MaxFmt);
  Format[MaxFmt-1] = '\0';
}


template<class T>
void BaseNumberParameter<T>::setUnit(const char *unit) {
  if (unit != NULL) {
    strncpy(Unit, unit, MaxUnit);
    Unit[MaxUnit-1] = '\0';
  }
}


template<class T>
void BaseNumberParameter<T>::setOutUnit(const char *unit) {
  if (unit != NULL) {
    strncpy(OutUnit, unit, MaxUnit);
    OutUnit[MaxUnit-1] = '\0';
  }
}


template<class T>
void BaseNumberParameter<T>::setSelection(const T *selection, size_t n) {
  NSelection = n;
  Selection = selection;
}


template<class T>
int BaseNumberParameter<T>::checkSelection(T val) {
  if (NSelection == 0)
    return 0;
  for (size_t k=0; k<NSelection; k++)
    if (abs(float(Selection[k]) - float(val)) < 1e-8)
      return k;
  return -1;
}


template<class T>
void BaseNumberParameter<T>::listSelection(Stream &stream) const {
  for (size_t k=0; k<NSelection; k++) {
    char str[MaxVal];
    if (this->Unit != NULL && strlen(this->Unit) > 0) {
      float val = this->changeUnit((float)Selection[k],
				   this->Unit, this->OutUnit);
      sprintf(str, this->Format, val);
      if (this->OutUnit != 0)
	strcat(str, this->OutUnit);
    }
    else
      sprintf(str, this->Format, Selection[k]);
    stream.printf("  - %s\n", str);
  }
}


template<class T>
NumberParameter<T>::NumberParameter(Configurable *cfg, const char *key,
				    T number, const char *format,
				    const char *unit, const char *outunit) :
  BaseNumberParameter<T>(cfg, key, format, unit, outunit),
  Value(number) {
}


template<class T>
T NumberParameter<T>::value(const char *unit) const {
  float val = changeUnit((float)Value, this->Unit, unit);
  return (T)val;
}


template<class T>
void NumberParameter<T>::setValue(T val) {
  if (this->checkSelection(val) < 0)
    return;
  Value = val;
}


template<class T>
void NumberParameter<T>::setValue(T val, const char *unit) {
  float nv = changeUnit((float)val, unit, this->Unit);
  if (this->checkSelection(val) < 0)
    return;
  Value = (T)nv;
}


template<class T>
bool NumberParameter<T>::parseValue(const char *val) {
  if (this->disabled())
    return true;
  float num = atof(val);
  const char *up = val;
  for (; *up != '\0' && (isdigit(*up) || *up == '+' || *up == '-' ||
			   *up == '.' || *up == 'e'); ++up);
  char unit[this->MaxUnit] = "";
  if (strlen(up) == 0 && strlen(this->OutUnit) > 0)
    strncpy(unit, this->OutUnit, this->MaxUnit);
  else
    strncpy(unit, up, this->MaxUnit);
  unit[this->MaxUnit-1] = '\0';
  float nv = this->changeUnit(num, unit, this->Unit);
  if (this->checkSelection(nv) < 0)
    return false;
  Value = (T)nv;
  return true;
}


template<class T>
void NumberParameter<T>::valueStr(char *str) const {
  if (this->Unit != NULL && strlen(this->Unit) > 0) {
    float val = this->changeUnit((float)Value, this->Unit, this->OutUnit);
    sprintf(str, this->Format, val);
    if (this->OutUnit != 0)
      strcat(str, this->OutUnit);
  }
  else
    sprintf(str, this->Format, Value);
}


template<class T>
NumberPointerParameter<T>::NumberPointerParameter(Configurable *cfg,
						  const char *key,
						  T *number,
						  const char *format,
						  const char *unit,
						  const char *outunit) :
  BaseNumberParameter<T>(cfg, key, format, unit, outunit),
  Value(number) {
}


template<class T>
T NumberPointerParameter<T>::value(const char *unit) const {
  float val = changeUnit(*Value, this->Unit, unit);
  return (T)val;
}


template<class T>
void NumberPointerParameter<T>::setValue(T val) {
  if (this->checkSelection(val) < 0)
    return;
  *Value = val;
}


template<class T>
void NumberPointerParameter<T>::setValue(T val, const char *unit) {
  float nv = changeUnit((float)val, unit, this->Unit);
  if (this->checkSelection(nv) < 0)
    return;
  *Value = (T)nv;
}


template<class T>
bool NumberPointerParameter<T>::parseValue(const char *val) {
  if (this->disabled())
    return true;
  float num = atof(val);
  const char *up = val;
  for (; *up != '\0' && (isdigit(*up) || *up == '+' || *up == '-' ||
			   *up == '.' || *up == 'e'); ++up);
  char unit[this->MaxUnit] = "";
  if (strlen(up) == 0 && strlen(this->OutUnit) > 0)
    strncpy(unit, this->OutUnit, this->MaxUnit);
  else
    strncpy(unit, up, this->MaxUnit);
  unit[this->MaxUnit-1] = '\0';
  float nv = this->changeUnit(num, unit, this->Unit);
  if (this->checkSelection(nv) < 0)
    return false;
  *Value = (T)nv;
  return true;
}


template<class T>
void NumberPointerParameter<T>::valueStr(char *str) const {
  if (this->Unit != NULL && strlen(this->Unit) > 0) {
    float val = this->changeUnit((float)(*Value), this->Unit, this->OutUnit);
    sprintf(str, this->Format, val);
    if (this->OutUnit != 0)
      strcat(str, this->OutUnit);
  }
  else
    sprintf(str, this->Format, *Value);
}


#endif
