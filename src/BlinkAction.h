/*
  BlinkAction - Action reporting Blink pins.
  Created by Jan Benda, November 10th, 2025.
*/

#ifndef BlinkAction_h
#define BlinkAction_h


#include <MicroConfig.h>


class Blink;


class BlinkAction : public Action {

 public:

  /* Initialize and add to configuration menu. */
  BlinkAction(Menu &menu, const char *name,
	      Blink *blink0, Blink *blink1=0, Blink *blink2=0,
	      Blink *blink3=0, Blink *blink4=0, Blink *blink5=0);

  /* Print blink infos. */
  virtual void write(Stream &stream=Serial, unsigned int roles=AllRoles,
		     size_t indent=0, size_t width=0, bool descend=true) const;

  /* Return Blink at index. Return zero for invalid index. */
  Blink *blink(size_t index);

protected:

  static const size_t MaxBlinks = 6;
  size_t NBlinks;
  Blink *Blinks[MaxBlinks];
  
};


#endif
