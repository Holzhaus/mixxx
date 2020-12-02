#pragma once
#include "util/color/colorpalette.h"

class ColorPalette;
template<class T>
class QList;

namespace mixxx {
class RgbColor;

class PredefinedColorPalettes {
  public:
    static const ColorPalette kMixxxHotcueColorPalette;
    static const ColorPalette kSeratoTrackMetadataHotcueColorPalette;
    static const ColorPalette kSeratoDJProHotcueColorPalette;

    static const ColorPalette kMixxxTrackColorPalette;
    static const ColorPalette kRekordboxTrackColorPalette;
    static const ColorPalette kSeratoDJProTrackColorPalette;
    static const ColorPalette kTraktorProTrackColorPalette;
    static const ColorPalette kVirtualDJTrackColorPalette;

    static const ColorPalette kDefaultHotcueColorPalette;
    static const ColorPalette kDefaultTrackColorPalette;

    static const QList<ColorPalette> kPalettes;
    static const mixxx::RgbColor kDefaultCueColor;
};

} // namespace mixxx
