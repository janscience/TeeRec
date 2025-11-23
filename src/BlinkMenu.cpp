#include <Blink.h>
#include <BlinkMenu.h>


ListLEDsAction::ListLEDsAction(Menu &menu, const char *name,
			       Blink* blink0, Blink* blink1,
			       Blink* blink2, Blink* blink3,
			       Blink* blink4, Blink* blink5) :
  Action(menu, name, ReportRoles) {
  if (blink0 != 0)
    Blinks[NBlinks++] = blink0;
  if (blink1 != 0)
    Blinks[NBlinks++] = blink1;
  if (blink2 != 0)
    Blinks[NBlinks++] = blink2;
  if (blink3 != 0)
    Blinks[NBlinks++] = blink3;
  if (blink4 != 0)
    Blinks[NBlinks++] = blink4;
  if (blink5 != 0)
    Blinks[NBlinks++] = blink5;
}


void ListLEDsAction::write(Stream &stream, unsigned int roles,
			   size_t indent, size_t width) const {
  stream.printf("%*s%s:\n", indent, "", "LEDs");
  indent += indentation();
  for (size_t k=0; k<NBlinks; k++)
    Blinks[k]->write(stream, indent, indentation());
}


BlinkLEDsAction::BlinkLEDsAction(Menu &menu, const char *name,
				 Blink* blink0, Blink* blink1,
				 Blink* blink2, Blink* blink3,
				 Blink* blink4, Blink* blink5) :
  ListLEDsAction(menu, name, blink0, blink1, blink2,
		 blink3, blink4, blink5) {
  setRoles(ActionRoles);
}


void BlinkLEDsAction::execute(Stream &stream) {
  if (NBlinks == 0)
    return;
  stream.println("LED blinking test");
  // store LED states and switch off:
  bool blink_on[NBlinks];
  for (size_t k=0; k<NBlinks; k++) {
    blink_on[k] = Blinks[k]->isOn();
    Blinks[k]->switchOff();
  }
  // blink them one after each other:
  for (size_t k=0; k<NBlinks; k++) {
    Blinks[k]->write(stream, 0, indentation());
    delay(1000);
    for (size_t j=0; j<3; j++) {
      Blinks[k]->switchOn();
      delay(Blinks[k]->ontime());
      Blinks[k]->switchOff();
      delay(Blinks[k]->offtime());
    }
  }
  // restore:
  delay(500);
  for (size_t k=0; k<NBlinks; k++)
    Blinks[k]->switchOn(blink_on[k]);
  stream.println("Done");
  stream.println();
}


BlinkMenu::BlinkMenu(Menu &menu, Blink *blink0, Blink *blink1, Blink *blink2,
	    Blink *blink3, Blink *blink4, Blink *blink5) :
  Menu(menu, "LEDs", StreamInput),
  ListAct(*this, "List LED pins", blink0, blink1, blink2,
	  blink3, blink4, blink5),
  BlinkAct(*this, "Blink LEDs", blink0, blink1, blink2,
	   blink3, blink4, blink5) {
}

