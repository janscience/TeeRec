/*
  Parameter - Actions with configurable name-value pairs of various types.
  Created by Jan Benda, October 22, 2023.

  For various types of inputs (strings, enums, numbers), specialized
  classes are provided (see list below). For each type there are three
  classes.  The Base*Parameter class, which provides most of the
  infrastructure needed for handling this type. This class is
  inherited by two classes. The *Parameter lass owns the value, and it
  has to be retrieved by calling value().  The *PointerParameter class
  just stores a pointer to the value, so that this external variable
  is updated immediately, whenever it is configured.

  Via the constructors, parameter classes get their name and are added
  to a Configurable. Via the value() and the valueStr() member functions,
  the current value can be obtained directly or as a string representation.
  Everything else is handled by the Configurable class.

  All classes support selections. These are lists of valid
  values. When provided, values can be chosen from these selections
  and only value that are contained in selections are considered
  valid.

  Classes:
  - Parameter: Base class for configurable parameters, i.e. name-value pairs.
  - BaseStringParameter: Base class for string values.
  - StringParameter: A parameter whose value is a string.
  - StringPointerParameter: A parameter whose value points to a string.
  - BaseEnumParameter: Base class for enum values, i.e. strings encoding integers.
  - EnumParameter: A parameter whose value is an integer that is represented as a string.
  - EnumPointerParameter: A parameter whose value points to an integer that is represented as a string.
  - BaseNumberParameter: Base class for numerical values with optional unit (integers and floats).
  - NumberParameter: A parameter whose value is a number with optional unit (any type of integer or float).
  - NumberPointerParameter: A parameter whose value points to a number with optional unit (any type of integer or float). 
*/

#ifndef Parameter_h
#define Parameter_h


#include <Action.h>


class File;
class Configurable;


/* Base class for configurable parameters, i.e. name-value pairs. */
class Parameter : public Action {

 public:

  /* Initialize parameter with identifying name, n selections
     and add it to menu. */
  Parameter(Configurable &menu, const char *name, size_t n=0);

  /* Report the parameter's name and value on serial stream. */
  virtual void report(Stream &stream=Serial, size_t indent=0,
		      size_t w=0, bool descend=true) const;

  /* Save the parameter's key and value to file. */
  virtual void save(File &file, size_t indent=0, size_t w=0) const;
  
  /* Interactive configuration via Serial stream. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);

  /* Parse the string val and set the parameter accordingly.  If
     StreamOutput is enabled, report the new value together with name
     on stream. */
  virtual void set(const char *val, const char *name=0,
		   Stream &stream=Serial);

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

  /* Maximum size of string needed for valueStr() */
  static const size_t MaxVal = 64;

  /* Convert val with oldunit to newunit. */
  static float changeUnit(float val, const char *oldunit, const char *newunit);
  
  
 protected:

  size_t NSelection;
  
  static const int NUnits = 50;
  static char UnitPref[NUnits][6];
  static float UnitFac[NUnits];
  
};


/* Base class for string values. */
class BaseStringParameter : public Parameter {

  /* Parameter with character array as value. */
  
 public:
  
  /* Initialize parameter with identifying name and add it to menu. */
  BaseStringParameter(Configurable &menu, const char *name);
  
  /* Initialize parameter with identifying name, list of n selections,
     and add it to menu. */
  BaseStringParameter(Configurable &menu, const char *name,
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


/* A parameter whose value is a string of size N. */
template<int N>
class StringParameter : public BaseStringParameter {

  /* Parameter owning a character array. */
  
 public:
  
  /* Initialize parameter with identifying name, value, list of n
     selections and add it to menu. */
  StringParameter(Configurable &menu, const char *name,
		  const char str[N],
		  const char **selection=0, size_t n=0);

  /* Return the string. */
  const char* value() const { return Value; };

  /* Set the string to val.
     Return true if val was a valid string or the parameter was disabled. */
  bool setValue(const char *val) { return parseValue(const_cast<char *>(val), false); };
  
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


/* A parameter whose value points to a string of size N. */
template<int N>
class StringPointerParameter : public BaseStringParameter {

  /* Parameter with a pointer to a character array. */

 public:
  
  /* Initialize parameter with identifying name, pointer str to value
     variable, list of n selections, and add it to menu. */
  StringPointerParameter(Configurable &menu, const char *name,
			 char (*str)[N], const char **selection=0,
			 size_t n=0);

  /* Return the string. */
  const char* value() const { return *Value; };

  /* Set the string to val.
     Return true if val was a valid string or the parameter was disabled. */
  bool setValue(const char *val) { return parseValue(const_cast<char *>(val), false); };
  
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


/* Base class for enum values, i.e. strings encoding integers of type T. */
template<class T>
class BaseEnumParameter : public BaseStringParameter {
  
 public:
  
  /* Initialize parameter with identifying name, list of n enum values
     and coresponding string representations, and add it to menu. */
  BaseEnumParameter(Configurable &menu, const char *name,
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


/* A parameter whose value is an integer of type T that is represented as a string. */
template<class T>
class EnumParameter : public BaseEnumParameter<T> {
  
 public:
  
  /* Initialize parameter with identifying name, value, list of n enum
     values and coresponding string representations, and add to
     menu. */
  EnumParameter(Configurable &menu, const char *name, T val,
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


/* A parameter whose value points to an integer of type T that is represented as a string. */
template<class T>
class EnumPointerParameter : public BaseEnumParameter<T> {
  
 public:
  
  /* Initialize parameter with identifying name, value, list of n enum
     values and coresponding string representations, and add to
     menu. */
  EnumPointerParameter(Configurable &menu, const char *name, T *val,
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


/* Base class for numerical values (integers and floats). */
template<class T>
class BaseNumberParameter : public Parameter {
  
 public:
  
  /* Initialize parameter with identifying name,
     format string, unit, and selection, and add it to menu. */
  BaseNumberParameter(Configurable &menu, const char *name,
		      const char *format, const char *unit=0,
		      const char *outunit=0, const T *selection=0,
		      size_t n=0);
  
  /* Initialize parameter with identifying name,
     minimum, maximum, format string, and unit, and add it to menu. */
  BaseNumberParameter(Configurable &menu, const char *name,
		      T minimu, T maximum, const char *format,
		      const char *unit=0, const char *outunit=0);

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

  /* The unit string used for string representations of the value,
     i.e. valueStr(). */
  const char *outunit() const { return OutUnit; };

  /* Set the unit string for the string representation of the value to unit. */
  void setOutUnit(const char *unit);

  /* Set special value that is encoded as a string. 
     For example, setSpecial(0, "infinity"); */
  void setSpecial(T value, const char *str);

  /* Provide a selection of n input values.
     If provided, only numbers in this list are valid inputs. */
  void setSelection(const T *selection, size_t n);

  /* Check whether val matches a selection.
     Return index of matching selection, 0 when there is no selection,
     and -1 if no match was found. */
  int checkSelection(T val);

  /* List selection of valid values. */
  virtual void listSelection(Stream &stream) const;

  /* Set minimum value a number can have. */
  T setMinimum(T minimum);

  /* Set maximum value a number can have. */
  T setMaximum(T maximum);

  /* Check whether val is in range. Return -2 if it is smaller than
     minimum, return -1 if it is larger than maximum, return 1 if it
     is in range. */
  int checkMinMax(T val);

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

  T SpecialValue;
  const char *SpecialStr;

  bool CheckMin;
  T Minimum;

  bool CheckMax;
  T Maximum;
  
};


/* A parameter whose value is a number with optional unit (any type of integer or float). */
template<class T>
class NumberParameter : public BaseNumberParameter<T> {
  
 public:
  
  /* Initialize parameter with identifying name, value,
     format string, unit, and selection, and add it to menu. */
  NumberParameter(Configurable &menu, const char *name, T value,
		  const char *format, const char *unit=0,
		  const char *outunit=0, const T *selection=0,
		  size_t n=0);
  
  /* Initialize parameter with identifying name, value,
     minimum and maximum, format string, unit, and add it to menu. */
  NumberParameter(Configurable &menu, const char *name, T value,
		  T minimum, T maximum, const char *format,
		  const char *unit=0, const char *outunit=0);

  /* Return the value of the number in its unit(). */
  T value() const { return Value; };

  /* Return the value of the number in an alternative unit. */
  T value(const char *unit) const;

  /* Set the number to val.
     If a selection was provided, then the number is only set,
     if it matches an element of the selection. */
  void setValue(T val);

  /* Set the number to val unit. The number is converted to the internal unit.
     If a selection was provided, then the number is only set,
     if it matches an element of the selection, after it was
     converted to the internal unit. */
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


/* A parameter whose value points to a number with optional unit (any type of integer or float). */
template<class T>
class NumberPointerParameter : public BaseNumberParameter<T> {
  
 public:
  
  /* Initialize parameter with identifying name, pointer to value,
     format string, unit, and selection, and add it to menu. */
  NumberPointerParameter(Configurable &menu, const char *name, T *value,
			 const char *format, const char *unit=0,
			 const char *outunit=0, const T *selection=0,
			 size_t n=0);
  
  /* Initialize parameter with identifying name, pointer to value,
     minimum, maximum, format string, and unit, and add it to menu. */
  NumberPointerParameter(Configurable &menu, const char *name, T *value,
			 T minimum, T maximum, const char *format,
			 const char *unit=0, const char *outunit=0);

  /* Return the value of the number. */
  T value() const { return *Value; };

  /* Return the value of the number in an alternative unit. */
  T value(const char *unit) const;

  /* Set the number to val.
     If a selection was provided, then the number is only set,
     if it matches an element of the selection. */
  void setValue(T val);

  /* Set the number to val unit. The number is converted to the internal unit.
     If a selection was provided, then the number is only set,
     if it matches an element of the selection, after it was
     converted to the internal unit. */
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
StringParameter<N>::StringParameter(Configurable &menu, const char *name,
				    const char str[N],
				    const char **selection, size_t n) :
  BaseStringParameter(menu, name, selection, n) {
  strncpy(Value, str, N);
  Value[N-1] = '\0';
}


template<int N>
bool StringParameter<N>::parseValue(char *val, bool selection) {
  if (disabled(Action::SetValue))
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
StringPointerParameter<N>::StringPointerParameter(Configurable &menu,
						  const char *name,
						  char (*str)[N],
						  const char **selection,
						  size_t n) :
  BaseStringParameter(menu, name, selection, n),
  Value(str) {
}


template<int N>
bool StringPointerParameter<N>::parseValue(char *val, bool selection) {
  if (disabled(Action::SetValue))
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
BaseEnumParameter<T>::BaseEnumParameter(Configurable &menu,
					const char *name,
					const T *enums,
					const char **selection,
					size_t n) :
  BaseStringParameter(menu, name, selection, n),
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
EnumParameter<T>::EnumParameter(Configurable &menu, const char *name,
				T val, const T *enums,
				const char **selection, size_t n) :
  BaseEnumParameter<T>(menu, name, enums, selection, n),
  Value(val) {
}


template<class T>
bool EnumParameter<T>::setValue(T val) {
  if (this->disabled(Action::SetValue))
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
EnumPointerParameter<T>::EnumPointerParameter(Configurable &menu,
					      const char *name,
					      T *val, const T *enums,
					      const char **selection,
					      size_t n) :
  BaseEnumParameter<T>(menu, name, enums, selection, n),
  Value(val) {
}


template<class T>
bool EnumPointerParameter<T>::setValue(T val) {
  if (this->disabled(Action::SetValue))
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
BaseNumberParameter<T>::BaseNumberParameter(Configurable &menu,
					    const char *name,
					    const char *format,
					    const char *unit,
					    const char *outunit,
					    const T *selection,
					    size_t n) :
  Parameter(menu, name, n),
  Format(""),
  Unit(""),
  OutUnit(""),
  Selection(selection),
  SpecialValue(0),
  SpecialStr(NULL),
  CheckMin(false),
  Minimum(0),
  CheckMax(false),
  Maximum(0) {
  setFormat(format);
  setUnit(unit);
  setOutUnit(outunit);
  if (strlen(OutUnit) == 0)
    setOutUnit(unit);
}


template<class T>
BaseNumberParameter<T>::BaseNumberParameter(Configurable &menu,
					    const char *name,
					    T minimum, T maximum,
					    const char *format,
					    const char *unit,
					    const char *outunit) :
  Parameter(menu, name, 0),
  Format(""),
  Unit(""),
  OutUnit(""),
  Selection(0),
  SpecialValue(0),
  SpecialStr(NULL),
  CheckMin(true),
  Minimum(minimum),
  CheckMax(true),
  Maximum(maximum) {
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
void BaseNumberParameter<T>::setSpecial(T value, const char *str) {
  SpecialValue = value;
  SpecialStr = str;
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
T BaseNumberParameter<T>::setMinimum(T minimum) {
  CheckMin = true;
  Minimum = minimum;
}


template<class T>
T BaseNumberParameter<T>::setMaximum(T maximum) {
  CheckMax = true;
  Maximum = maximum;
}


template<class T>
int BaseNumberParameter<T>::checkMinMax(T val) {
  if (CheckMin && val < Minimum)
    return -2;
  if (CheckMax && val > Maximum)
    return -1;
  return 1;
}


template<class T>
void BaseNumberParameter<T>::valueStr(T val, char *str) const {
  if (this->Unit != NULL && strlen(this->Unit) > 0) {
    float value = this->changeUnit((float)val, this->Unit, this->OutUnit);
    if (SpecialStr != NULL && strlen(SpecialStr) > 0 && value == SpecialValue)
      strcpy(str, SpecialStr);
    else {
      sprintf(str, this->Format, value);
      if (this->OutUnit != 0)
	strcat(str, this->OutUnit);
    }
  }
  else {
    if (SpecialStr != NULL && strlen(SpecialStr) > 0 && val == SpecialValue)
      strcpy(str, SpecialStr);
    else
      sprintf(str, this->Format, val);
  }
}


template<class T>
NumberParameter<T>::NumberParameter(Configurable &menu, const char *name,
				    T number, const char *format,
				    const char *unit,
				    const char *outunit,
				    const T *selection, size_t n) :
  BaseNumberParameter<T>(menu, name, format, unit, outunit,
			 selection, n),
  Value(number) {
}


template<class T>
NumberParameter<T>::NumberParameter(Configurable &menu, const char *name,
				    T number, T minimum, T maximum,
				    const char *format, const char *unit,
				    const char *outunit) :
  BaseNumberParameter<T>(menu, name, minimum, maximum, format, unit, outunit),
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
  if (this->checkMinMax(val) < 0)
    return;
  Value = val;
}


template<class T>
void NumberParameter<T>::setValue(T val, const char *unit) {
  float nv = changeUnit((float)val, unit, this->Unit);
  if (this->checkSelection(val) < 0)
    return;
  if (this->checkMinMax(nv) < 0)
    return;
  Value = (T)nv;
}


template<class T>
bool NumberParameter<T>::parseValue(char *val, bool selection) {
  if (this->disabled(Action::SetValue))
    return true;
  // when a selection was offered and 'q' was entered, keep the value:
  if (selection && this->NSelection > 0 && strcmp(val, "q") == 0) {
    valueStr(val);
    return true;
  }
  if (strlen(val) == 0)
    return true;
  if (this->SpecialStr != NULL && strlen(this->SpecialStr) > 0 &&
      strcmp(val, this->SpecialStr) == 0) {
    Value = this->SpecialValue;
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
  if (this->checkMinMax(nv) < 0)
    return false;
  Value = (T)nv;
  return true;
}


template<class T>
void NumberParameter<T>::valueStr(char *str) const {
  valueStr(Value, str);
}


template<class T>
NumberPointerParameter<T>::NumberPointerParameter(Configurable &menu,
						  const char *name,
						  T *number,
						  const char *format,
						  const char *unit,
						  const char *outunit,
						  const T *selection,
						  size_t n) :
  BaseNumberParameter<T>(menu, name, format, unit, outunit,
			 selection, n),
  Value(number) {
}


template<class T>
NumberPointerParameter<T>::NumberPointerParameter(Configurable &menu,
						  const char *name,
						  T *number,
						  T minimum, T maximum,
						  const char *format,
						  const char *unit,
						  const char *outunit) :
  BaseNumberParameter<T>(menu, name, minimum, maximum, format, unit, outunit),
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
  if (this->checkMinMax(val) < 0)
    return;
  *Value = val;
}


template<class T>
void NumberPointerParameter<T>::setValue(T val, const char *unit) {
  float nv = changeUnit((float)val, unit, this->Unit);
  if (this->checkSelection(nv) < 0)
    return;
  if (this->checkMinMax(nv) < 0)
    return;
  *Value = (T)nv;
}


template<class T>
bool NumberPointerParameter<T>::parseValue(char *val, bool selection) {
  if (this->disabled(Action::SetValue))
    return true;
  // when a selection was offered and 'q' was entered, keep the value:
  if (selection && this->NSelection > 0 && strcmp(val, "q") == 0) {
    valueStr(val);
    return true;
  }
  if (strlen(val) == 0)
    return true;
  if (this->SpecialStr != NULL && strlen(this->SpecialStr) > 0 &&
      strcmp(val, this->SpecialStr) == 0) {
    *Value = this->SpecialValue;
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
  if (this->checkMinMax(nv) < 0)
    return false;
  *Value = (T)nv;
  return true;
}


template<class T>
void NumberPointerParameter<T>::valueStr(char *str) const {
  valueStr(*Value, str);
}


#endif
