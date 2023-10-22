#include <Parameter.h>


Parameter::Parameter(const char *key) :
  Configurable(true) {
  setKey(key);
}


void Parameter::setKey(const char *key) {
  strncpy(Key, key, MaxStr);
}


void Parameter::setConfigurable(bool configurable) {
  Configurable = configurable;
}


StringParameter::StringParameter(const char *key, char **str) :
  Parameter(key),
  Str(str) {
}


void StringParameter::parseValue(const char *val) {
  if (!Configurable)
    return;
  strncpy(*Str, val, MaxStr);
}


void StringParameter::valueStr(char *str) {
  strncpy(str, *Str, MaxStr);
}


NumberParameter::NumberParameter(const char *key, const char *format) :
  Parameter(key) {
  setFormat(format);
}


void NumberParameter::setFormat(const char *format) {
  strncpy(Format, format, MaxFmt);
}


IntegerParameter::IntegerParameter(const char *key, int *integer,
				   const char *format) :
  NumberParameter(key, format),
  Integer(integer) {
}


void IntegerParameter::parseValue(const char *val) {
  if (!Configurable)
    return;
  *Integer = atoi(val);
}


void IntegerParameter::valueStr(char *str) {
  sprintf(str, Format, *Integer);
}


TimeParameter::TimeParameter(const char *key, float *time,
			     const char *format) :
  NumberParameter(key, format),
  Time(time) {
}


void TimeParameter::parseValue(const char *val) {
  if (!Configurable)
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
  *Time = time;
}


void TimeParameter::valueStr(char *str) {
  sprintf(str, Format, *Time);
  strcat(str, "s");
}


FrequencyParameter::FrequencyParameter(const char *key, float *frequency,
				       const char *format) :
  NumberParameter(key, format),
  Frequency(frequency) {
}


void FrequencyParameter::parseValue(const char *val) {
  if (!Configurable)
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
  *Frequency = freq;
}


void FrequencyParameter::valueStr(char *str) {
  sprintf(str, Format, *Frequency);
  strcat(str, "Hz");
}

