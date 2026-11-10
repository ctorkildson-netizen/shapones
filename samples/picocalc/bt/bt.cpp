// bt/bt.cpp -- a Bluetooth gamepad and keyboard for ShapoNES on the PicoCalc,
// through btkbd from picocalc-extras (paired beforehand with its btpair).
//
// Buttons are mapped by where they sit, as on the NES pad: the right-hand
// face button is A, the bottom one B; Options/Start is Start and
// Share/Select is Select; the D-pad or the left stick steers.  The numbers
// are a PS4 pad's (Square 1, Cross 2, Circle 3, Triangle 4, Share 9,
// Options 10, PS 13), which most gamepads follow in HID mode.
#include "bt.hpp"

extern "C" {
#include "btkbd.h"
}

// btkbd hooks the text starter's keyboard read (sb_read_keyboard) so the two
// keyboards come out together; this port reads its keyboard itself, so there
// is nothing to merge with.
extern "C" uint16_t sb_read_keyboard(void) { return 0; }

namespace picocalc::bt {

static constexpr uint32_t BTN(int n) { return 1u << (n - 1); }
static constexpr uint32_t A_BUTTONS = BTN(3);          // Circle (right)
static constexpr uint32_t B_BUTTONS = BTN(2);          // Cross (bottom)
static constexpr uint32_t SELECT_BUTTONS = BTN(9);     // Share
static constexpr uint32_t START_BUTTONS = BTN(10);     // Options
static constexpr uint32_t MENU_BUTTONS = BTN(13);      // PS
static constexpr int STICK = 64;                       // how far the stick counts as the D-pad

void init() {
  btkbd_init();
  btkbd_merge(false);
}

pad_t pad() {
  static uint32_t was;
  btkbd_pad_t p;
  btkbd_gamepad(&p);
  pad_t r{};
  int h = p.hat;
  r.up = h == 7 || h == 0 || h == 1 || p.y < -STICK;
  r.right = (h >= 1 && h <= 3) || p.x > STICK;
  r.down = (h >= 3 && h <= 5) || p.y > STICK;
  r.left = (h >= 5 && h <= 7) || p.x < -STICK;
  r.A = (p.buttons & A_BUTTONS) != 0;
  r.B = (p.buttons & B_BUTTONS) != 0;
  r.select = (p.buttons & SELECT_BUTTONS) != 0;
  r.start = (p.buttons & START_BUTTONS) != 0;
  r.menu = (p.buttons & MENU_BUTTONS) && !(was & MENU_BUTTONS);
  r.square = (p.buttons & BTN(1)) && !(was & BTN(1));
  was = p.buttons;
  return r;
}

uint16_t key() { return btkbd_read(); }
int pad_state() { return btkbd_device_state(BTKBD_GAMEPAD); }
uint32_t pad_reports() { return btkbd_gamepad_reports(); }
bool sniff(int ms) { return btkbd_gamepad_sniff(ms); }

}  // namespace picocalc::bt
