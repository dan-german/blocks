#pragma once
#include "Theme.h"

struct ThemeListener {
  virtual void themeChanged(Theme theme) = 0;
};