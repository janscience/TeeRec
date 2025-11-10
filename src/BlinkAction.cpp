#include <Blink.h>
#include <BlinkAction.h>


BlinkAction::BlinkAction(Menu &menu, const char *name,
			 Blink* blink0, Blink* blink1,
			 Blink* blink2, Blink* blink3,
			 Blink* blink4, Blink* blink5) :
  Action(menu, name, StreamIO | Report) {
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


void BlinkAction::write(Stream &stream, unsigned int roles,
			 size_t indent, size_t width, bool descend) const {
  if (disabled(roles))
    return;
  if (descend) {
    if (strlen(name()) > 0) {
      stream.printf("%*s%s:\n", indent, "", name());
      indent += indentation();
    }
    for (size_t k=0; k<NBlinks; k++)
      Blinks[k]->write(stream, indent, indentation());
  }
  else if (strlen(name()) > 0)
    Action::write(stream, roles, indent, width, descend);
}

