#include "core/virtual-display.hpp"
#include "input.hpp"
#include <helpers/logger.hpp>
#include <libevdev/libevdev.h>
#include <string>

namespace wolf::platforms::input {
using namespace std::string_literals;

/**
 * A map of linux scan code -> Moonlight keyboard code
 */
static const std::map<short, short> key_mappings = {
    {KEY_BACKSPACE, 0x08}, {KEY_TAB, 0x09},        {KEY_ENTER, 0x0D},      {KEY_LEFTSHIFT, 0x10},
    {KEY_LEFTCTRL, 0x11},  {KEY_CAPSLOCK, 0x14},   {KEY_ESC, 0x1B},        {KEY_SPACE, 0x20},
    {KEY_PAGEUP, 0x21},    {KEY_PAGEDOWN, 0x22},   {KEY_END, 0x23},        {KEY_HOME, 0x24},
    {KEY_LEFT, 0x25},      {KEY_UP, 0x26},         {KEY_RIGHT, 0x27},      {KEY_DOWN, 0x28},
    {KEY_SYSRQ, 0x2C},     {KEY_INSERT, 0x2D},     {KEY_DELETE, 0x2E},     {KEY_0, 0x30},
    {KEY_1, 0x31},         {KEY_2, 0x32},          {KEY_3, 0x33},          {KEY_4, 0x34},
    {KEY_5, 0x35},         {KEY_6, 0x36},          {KEY_7, 0x37},          {KEY_8, 0x38},
    {KEY_9, 0x39},         {KEY_A, 0x41},          {KEY_B, 0x42},          {KEY_C, 0x43},
    {KEY_D, 0x44},         {KEY_E, 0x45},          {KEY_F, 0x46},          {KEY_G, 0x47},
    {KEY_H, 0x48},         {KEY_I, 0x49},          {KEY_J, 0x4A},          {KEY_K, 0x4B},
    {KEY_L, 0x4C},         {KEY_M, 0x4D},          {KEY_N, 0x4E},          {KEY_O, 0x4F},
    {KEY_P, 0x50},         {KEY_Q, 0x51},          {KEY_R, 0x52},          {KEY_S, 0x53},
    {KEY_T, 0x54},         {KEY_U, 0x55},          {KEY_V, 0x56},          {KEY_W, 0x57},
    {KEY_X, 0x58},         {KEY_Y, 0x59},          {KEY_Z, 0x5A},          {KEY_LEFTMETA, 0x5B},
    {KEY_RIGHTMETA, 0x5C}, {KEY_KP0, 0x60},        {KEY_KP1, 0x61},        {KEY_KP2, 0x62},
    {KEY_KP3, 0x63},       {KEY_KP4, 0x64},        {KEY_KP5, 0x65},        {KEY_KP6, 0x66},
    {KEY_KP7, 0x67},       {KEY_KP8, 0x68},        {KEY_KP9, 0x69},        {KEY_KPASTERISK, 0x6A},
    {KEY_KPPLUS, 0x6B},    {KEY_KPMINUS, 0x6D},    {KEY_KPDOT, 0x6E},      {KEY_KPSLASH, 0x6F},
    {KEY_F1, 0x70},        {KEY_F2, 0x71},         {KEY_F3, 0x72},         {KEY_F4, 0x73},
    {KEY_F5, 0x74},        {KEY_F6, 0x75},         {KEY_F7, 0x76},         {KEY_F8, 0x77},
    {KEY_F9, 0x78},        {KEY_F10, 0x79},        {KEY_F11, 0x7A},        {KEY_F12, 0x7B},
    {KEY_NUMLOCK, 0x90},   {KEY_SCROLLLOCK, 0x91}, {KEY_LEFTSHIFT, 0xA0},  {KEY_RIGHTSHIFT, 0xA1},
    {KEY_LEFTCTRL, 0xA2},  {KEY_RIGHTCTRL, 0xA3},  {KEY_LEFTALT, 0xA4},    {KEY_RIGHTALT, 0xA5},
    {KEY_SEMICOLON, 0xBA}, {KEY_EQUAL, 0xBB},      {KEY_COMMA, 0xBC},      {KEY_MINUS, 0xBD},
    {KEY_DOT, 0xBE},       {KEY_SLASH, 0xBF},      {KEY_GRAVE, 0xC0},      {KEY_LEFTBRACE, 0xDB},
    {KEY_BACKSLASH, 0xDC}, {KEY_RIGHTBRACE, 0xDD}, {KEY_APOSTROPHE, 0xDE}, {KEY_102ND, 0xE2}};

void paste_utf(state::KeyboardTypes &keyboard, const std::basic_string<char32_t> &utf32) {
  /* To HEX string */
  auto hex_unicode = to_hex(utf32);
  logs::log(logs::debug, "[INPUT] Typing U+{}", hex_unicode);

  std::visit(
      [hex_unicode](auto &kb) {
        kb.press(0xA2);   // LEFTCTRL
        kb.press(0xA0);   // LEFTSHIFT
        kb.press(0x55);   // U
        kb.release(0x55); // U

        for (auto &ch : hex_unicode) {
          auto key_str = "KEY_"s + ch;
          auto keycode = libevdev_event_code_from_name(EV_KEY, key_str.c_str());
          if (keycode == -1) {
            logs::log(logs::warning, "[INPUT] Unable to find keycode for: {}", ch);
          } else {
            kb.press(key_mappings.at(keycode));
            kb.release(key_mappings.at(keycode));
          }
        }

        kb.release(0xA0); // LEFTSHIFT
        kb.release(0xA2); // LEFTCTRL
      },
      keyboard);
}

} // namespace wolf::platforms::input