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

  /* Initialize parameter with identifying key, n selections
     and add it to cfg. */
  Parameter(Configurable *cfg, const char *key, size_t n=0);

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
     If selection, then val is the input in response to an offered
     selection (i.e. it might be an index to the selection).
     Return true if val was a valid string or the parameter was disabled. */
  virtual bool parseValue(char *val, bool selection=false) = 0;

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
  
  /* Initialize parameter with identifying key, list of n selections,
     and add to cfg. */
  BaseStringParameter(Configurable *cfg, const char *key,
		      const char **selection, size_t n);

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
  
  /* Initialize parameter with identifying key, value, list of n
     selections and add to cfg. */
  StringParameter(Configurable *cfg, const char *key,
		  const char str[N],
		  const char **selection=0, size_t n=0);

  /* Return the string. */
  const char* value() const { return Value; };

  /* Set the string to val.
     Return true if val was a valid string or the parameter was disabled. */
  bool setValue(char *val) { return parseValue(val, false); };
  
  /* Parse the string val and set the value of this parameter accordingly.
     If selection, then val is the input in response to an offered
     selection (i.e. it might be an index to the selection).
     Return true if val was a valid string or the parameter was disabled. */
  virtual bool parseValue(char *val, bool selection=false);

  /* Return the current value of this parameter as a string. */
  virtual void valueStr(char *str) const;

  
 protected:

  char Value[N];
  
};


template<int N>
class StringPointerParameter : public BaseStringParameter {

  /* Parameter with a pointer to a character array. */

 public:
  
  /* Initialize parameter with identifying key, pointer str to value
     variable, list of n selections, and add to cfg. */
  StringPointerParameter(Configurable *cfg, const char *key,
			 char (*str)[N], const char **selection=0,
			 size_t n=0);

  /* Return the string. */
  const char* value() const { return *Value; };

  /* Set the string to val.
     Return true if val was a valid string or the parameter was disabled. */
  bool setValue(char *val) { return parseValue(val, false); };
  
  /* Parse the string val and set the value of this parameter accordingly.
     If selection, then val is the input in response to an offered
     selection (i.e. it might be an index to the selection).
     Return true if val was a valid string or the parameter was disabled. */
  virtual bool parseValue(char *val, bool selection=false);

  /* Return the current value of this parameter as a string. */
  virtual void valueStr(char *str) const;

  
 protected:

  char (*Value)[N];
  
};


template<class T>
class BaseEnumParameter : public BaseStringParameter {

  /* Parameter with enum as value. */
  
 public:
  
  /* Initialize parameter with identifying key, list of n enum values
     and coresponding string representations, and add to cfg. */
  BaseEnumParameter(Configurable *cfg, const char *key,
		    const T *enums, const char **selection, size_t n);

  /* Provide a selection of n enums with corresponding string
     representations. */
  void setSelection(const T *enums, const char **selection, size_t n);

  /* Check whether val matches a string of the selection. Return
     corresponding enum alue. */
  T checkSelection(const char *val);

  /* Return string representation of enum value. */
  const char *enumStr(T val) const;

  
 protected:

  const T *Enums;
  
};


template<class T>
class EnumParameter : public BaseEnumParameter<T> {

  /* Parameter owning an enum . */
  
 public:
  
  /* Initialize parameter with identifying key, value, list of n enum
     values and coresponding string representations, and add to
     cfg. */
  EnumParameter(Configurable *cfg, const char *key, T val,
		const T *enums, const char **selection, size_t n);

  /* Return the enum value. */
  T value() const { return Value; };

  /* Set the enum to val.
     Return false if the parameter was disabled. */
  bool setValue(T val);
  
  /* Parse the string val and set the value of this parameter accordingly.
     If selection, then val is the input in response to an offered
     selection (i.e. it might be an index to the selection).
     Return true if val was a valid string or the parameter was disabled. */
  virtual bool parseValue(char *val, bool selection=false);

  /* Return the current value of this parameter as a string. */
  virtual void valueStr(char *str) const;

  
 protected:

  T Value;
  
};


template<class T>
class EnumPointerParameter : public BaseEnumParameter<T> {

  /* Parameter with a pointer to an enum . */
  
 public:
  
  /* Initialize parameter with identifying key, value, list of n enum
     values and coresponding string representations, and add to
     cfg. */
  EnumPointerParameter(Configurable *cfg, const char *key, T *val,
		       const T *enums, const char **selection,
		       size_t n);

  /* Return the enum value. */
  T value() const { return *Value; };

  /* Set the enum to val.
     Return false if the parameter was disabled. */
  bool setValue(T val);
  
  /* Parse the string val and set the value of this parameter accordingly.
     If selection, then val is the input in response to an offered
     selection (i.e. it might be an index to the selection).
     Return true if val was a valid string or the parameter was disabled. */
  virtual bool parseValue(char *val, bool selection=false);

  /* Return the current value of this parameter as a string. */
  virtual void valueStr(char *str) const;

  
 protected:

  T *Value;
  
};


template<class T>
class BaseNumberParameter : public Parameter {
  
 public:
  
  /* Initialize parameter with identifying key,
     format string, and unit and add to cfg. */
  BaseNumberParameter(Configurable *cfg, const char *key,
		      const char *format, const char *unit=0,
		      const char *outunit=0, const T *selection=0,
		      size_t n=0);

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

  /* Check whether val matches a selection.
     Return index of matching selection, 0 when there is no selection,
     and -1 if no match was found. */
  int checkSelection(T val);

  /* List selection of valid values. */
  virtual void listSelection(Stream &stream) const;

  /* Return for val a properly formatted string of maximum size MaxVal
     with outUnit appended. */
  virtual void valueStr(T val, char *str) const;

  
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
		  const char *outunit=0, const T *selection=0,
		  size_t n=0);

  /* Return the value of the number in its unit(). */
  T value() const { return Value; };

  /* Return the value of the number in an alternative unit. */
  T value(const char *unit) const;

  /* Set the number to val. */
  void setValue(T val);

  /* Set the number to val unit. */
  void setValue(T val, const char *unit);
  
  /* Parse the string val and set the value of this parameter accordingly.
     If selection, then val is the input in response to an offered
     selection (i.e. it might be an index to the selection).
     Return true if val was a valid string or the parameter was disabled. */
  virtual bool parseValue(char *val, bool selection=false);

  /* Return the current value of this parameter as a string */
  virtual void valueStr(char *str) const;
  
  /* Return for val a properly formatted string of maximum size MaxVal
     with outUnit appended. */
  virtual void valueStr(T val, char *str) const { BaseNumberParameter<T>::valueStr(val, str); };
  
  
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
			 const char *outunit=0, const T *selection=0,
			 size_t n=0);

  /* Return the value of the number. */
  T value() const { return *Value; };

  /* Return the value of the number in an alternative unit. */
  T value(const char *unit) const;

  /* Set the number to val. */
  void setValue(T val);

  /* Set the number to val unit. */
  void setValue(T val, const char *unit);
  
  /* Parse the string val and set the value of this parameter accordingly.
     If selection, then val is the input in response to an offered
     selection (i.e. it might be an index to the selection).
     Return true if val was a valid string or the parameter was disabled. */
  virtual bool parseValue(char *val, bool selection=false);

  /* Return the current value of this parameter as a string */
  virtual void valueStr(char *str) const;
  
  /* Return for val a properly formatted string of maximum size MaxVal
     with outUnit appended. */
  virtual void valueStr(T val, char *str) const { BaseNumberParameter<T>::valueStr(val, str); };
  
  
 protected:

  T *Value;
  
};


template<int N>
StringParameter<N>::StringParameter(Configurable *cfg, const char *key,
				    const char str[N],
				    const char **selection, size_t n) :
  BaseStringParameter(cfg, key, selection, n) {
  strncpy(Value, str, N);
  Value[N-1] = '\0';
}


template<int N>
bool StringParameter<N>::parseValue(char *val, bool selection) {
  if (disabled())
    return true;
  if (selection && NSelection > 0) {
    if (strcmp(val, "q") == 0) {
      strncpy(val, Value, MaxVal);
      val[MaxVal-1] = '\0';
    }
    else {
      char *end;
      long i = strtol(val, &end, 10) - 1;
      if (end == val || i < 0 || i >= (long)NSelection)
	return false;
      strncpy(Value, Selection[i], N);
      Value[N-1] = '\0';
      strncpy(val, Selection[i], MaxVal);
      val[MaxVal-1] = '\0';
    }
  }
  else {
    if (checkSelection(val) < 0)
      return false;
    strncpy(Value, val, N);
    Value[N-1] = '\0';
  }
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
						  char (*str)[N],
						  const char **selection,
						  size_t n) :
  BaseStringParameter(cfg, key, selection, n),
  Value(str) {
}


template<int N>
bool StringPointerParameter<N>::parseValue(char *val, bool selection) {
  if (disabled())
    return true;
  if (selection && NSelection > 0) {
    if (strcmp(val, "q") == 0) {
      strncpy(val, *Value, MaxVal);
      val[MaxVal-1] = '\0';
    }
    else {
      char *end;
      long i = strtol(val, &end, 10) - 1;
      if (end == val || i < 0 || i >= (long)NSelection)
	return false;
      else {
	strncpy(*Value, Selection[i], N);
	(*Value)[N-1] = '\0';
	strncpy(val, Selection[i], MaxVal);
	val[MaxVal-1] = '\0';
      }
    }
  }
  else {
    if (checkSelection(val) < 0)
      return false;
    strncpy(*Value, val, N);
    (*Value)[N-1] = '\0';
  }
  return true;
}


template<int N>
void StringPointerParameter<N>::valueStr(char *str) const {
  int n = MaxVal < N ? MaxVal : N;
  strncpy(str, *Value, n);
  str[n-1] = '\0';
}


template<class T>
BaseEnumParameter<T>::BaseEnumParameter(Configurable *cfg,
					const char *key,
					const T *enums,
					const char **selection,
					size_t n) :
  BaseStringParameter(cfg, key, selection, n),
  Enums(enums) {
}


template<class T>
void BaseEnumParameter<T>::setSelection(const T *enums,
					const char **selection,
					size_t n) {
  NSelection = n;
  Selection = selection;
  Enums = enums;
}


template<class T>
T BaseEnumParameter<T>::checkSelection(const char *val) {
  char lval[strlen(val)+1];
  for (size_t k=0; k<strlen(val)+1; k++)
    lval[k] = tolower(val[k]);
  for (size_t j=0; j<NSelection; j++) {
    char cval[strlen(Selection[j])+1];
    for (size_t k=0; k<strlen(Selection[j])+1; k++)
      cval[k] = tolower(Selection[j][k]);
    if (strcmp(cval, lval) == 0)
      return Enums[j];
  }
  return Enums[0];
}


template<class T>
const char *BaseEnumParameter<T>::enumStr(T val) const {
  for (size_t j=0; j<this->NSelection; j++) {
    if (val == Enums[j])
      return Selection[j];
  }
  return Selection[0];
}


template<class T>
EnumParameter<T>::EnumParameter(Configurable *cfg, const char *key,
				T val, const T *enums,
				const char **selection, size_t n) :
  BaseEnumParameter<T>(cfg, key, enums, selection, n),
  Value(val) {
}


template<class T>
bool EnumParameter<T>::setValue(T val) {
  if (this->disabled())
    return false;
  Value = val;
  return true;
}

  
template<class T>
bool EnumParameter<T>::parseValue(char *val, bool selection) {
  if (selection) {
    if (strcmp(val, "q") != 0) {
      char *end;
      long i = strtol(val, &end, 10) - 1;
      if (end == val || i < 0 || i >= (long)(this->NSelection))
	return false;
      Value = this->Enums[i];
    }
  }
  else {
    T eval = this->checkSelection(val);
    // lower case input string:
    char lval[strlen(val)+1];
    for (size_t k=0; k<strlen(val)+1; k++)
      lval[k] = tolower(val[k]);
    // lower case enum string:
    const char *es = this->enumStr(eval);
    char les[strlen(es)+1];
    for (size_t k=0; k<strlen(es)+1; k++)
      les[k] = tolower(es[k]);
    if (strcmp(lval, les) != 0)
      return false;
    Value = eval;
  }
  valueStr(val);
  return true;
}


template<class T>
void EnumParameter<T>::valueStr(char *str) const {
  const char *es = this->enumStr(Value);
  strncpy(str, es, Parameter::MaxVal);
  str[Parameter::MaxVal-1] = '\0';
}


template<class T>
EnumPointerParameter<T>::EnumPointerParameter(Configurable *cfg,
					      const char *key,
					      T *val, const T *enums,
					      const char **selection,
					      size_t n) :
  BaseEnumParameter<T>(cfg, key, enums, selection, n),
  Value(val) {
}


template<class T>
bool EnumPointerParameter<T>::setValue(T val) {
  if (this->disabled())
    return false;
  *Value = val;
  return true;
}

  
template<class T>
bool EnumPointerParameter<T>::parseValue(char *val, bool selection) {
  if (selection) {
    if (strcmp(val, "q") != 0) {
      char *end;
      long i = strtol(val, &end, 10) - 1;
      if (end == val || i < 0 || i >= (long)(this->NSelection))
	return false;
      *Value = this->Enums[i];
    }
  }
  else {
    T eval = this->checkSelection(val);
    // lower case input string:
    char lval[strlen(val)+1];
    for (size_t k=0; k<strlen(val)+1; k++)
      lval[k] = tolower(val[k]);
    // lower case enum string:
    const char *es = enumStr(eval);
    char les[strlen(es)+1];
    for (size_t k=0; k<strlen(es)+1; k++)
      les[k] = tolower(es[k]);
    if (strcmp(lval, les) != 0)
      return false;
    *Value = eval;
  }
  valueStr(val);
  return true;
}


template<class T>
void EnumPointerParameter<T>::valueStr(char *str) const {
  const char *es = enumStr(*Value);
  strncpy(str, es, Parameter::MaxVal);
  str[Parameter::MaxVal-1] = '\0';
}


template<class T>
BaseNumberParameter<T>::BaseNumberParameter(Configurable *cfg, const char *key,
					    const char *format,
					    const char *unit,
					    const char *outunit,
					    const T *selection,
					    size_t n) :
  Parameter(cfg, key, n),
  Format(""),
  Unit(""),
  OutUnit(""),
  Selection(selection) {
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
    valueStr(Selection[k], str);
    stream.printf("  - %s\n", str);
  }
}


template<class T>
void BaseNumberParameter<T>::valueStr(T val, char *str) const {
  if (this->Unit != NULL && strlen(this->Unit) > 0) {
    float value = this->changeUnit((float)val, this->Unit, this->OutUnit);
    sprintf(str, this->Format, value);
    if (this->OutUnit != 0)
      strcat(str, this->OutUnit);
  }
  else
    sprintf(str, this->Format, val);
}


template<class T>
NumberParameter<T>::NumberParameter(Configurable *cfg, const char *key,
				    T number, const char *format,
				    const char *unit,
				    const char *outunit,
				    const T *selection, size_t n) :
  BaseNumberParameter<T>(cfg, key, format, unit, outunit,
			 selection, n),
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
bool NumberParameter<T>::parseValue(char *val, bool selection) {
  if (this->disabled())
    return true;
  if (selection && this->NSelection > 0 && strcmp(val, "q") == 0) {
    valueStr(val);
    return true;
  }
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
  valueStr(Value, str);
}


template<class T>
NumberPointerParameter<T>::NumberPointerParameter(Configurable *cfg,
						  const char *key,
						  T *number,
						  const char *format,
						  const char *unit,
						  const char *outunit,
						  const T *selection,
						  size_t n) :
  BaseNumberParameter<T>(cfg, key, format, unit, outunit,
			 selection, n),
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
bool NumberPointerParameter<T>::parseValue(char *val, bool selection) {
  if (this->disabled())
    return true;
  if (selection && this->NSelection > 0 && strcmp(val, "q") == 0) {
    valueStr(val);
    return true;
  }
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
  valueStr(*Value, str);
}


#endif
