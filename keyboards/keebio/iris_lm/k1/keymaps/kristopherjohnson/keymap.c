// The Iris LM-K (STM32G431) uses the exact same physical layout as the Iris
// rev8 (RP2040): the LAYOUT macro and RGB-matrix LED map are identical between
// the two boards. To keep a single source of truth for the layer tables and
// the custom Caps Word / split-sync logic, this keymap compiles the rev8
// keymap source directly instead of duplicating it.
#include "../../../../iris/rev8/keymaps/kristopherjohnson/keymap.c"
