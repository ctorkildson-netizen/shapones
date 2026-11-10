// bt/bt.hpp -- a Bluetooth gamepad and keyboard (the Bluetooth build only)
#pragma once
#include <stdint.h>

namespace picocalc::bt {

struct pad_t {
  bool up, down, left, right, A, B, start, select;
  bool menu;      // the pad's PS/Home button was just pressed
};

void init();                 // after the card is mounted (it reads pairing.dat)
pad_t pad();                 // the gamepad now (all false with none connected)
uint16_t key();              // the next key event from a Bluetooth keyboard, or 0
int pad_state();             // btkbd's state for the gamepad (btkbd_state_t)
uint32_t pad_reports();      // how many reports have come from the gamepad

}  // namespace picocalc::bt
