#include <fxcg/display.h>
#include <preader/common_definitions.h>

#ifndef _COLORDIALOG_H
#define _COLORDIALOG_H

#define N_PALETTES_READER 4
#define N_PALETTES_UI     2

extern presetPalette palettes_reader[];
extern presetPalette palettes_ui[];
extern color_t pick_a_color(color_t current_color);
extern int preset_chooser(presetPalette *palettes, int n_palettes, color_t *dest);

#endif