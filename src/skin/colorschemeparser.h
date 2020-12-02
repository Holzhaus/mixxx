#ifndef COLORSCHEMEPARSER_H
#define COLORSCHEMEPARSER_H

#include <QDomElement>
#include <QDomNode>

#include "preferences/usersettings.h"
#include "skin/legacyskinparser.h"
#include "skin/skincontext.h"

class ImgSource;
class QString;
class SkinContext;

class ColorSchemeParser {
  public:
    static void setupLegacyColorSchemes(
            const QDomElement& docElem,
            UserSettingsPointer pConfig,
            QString* pStyle,
            SkinContext* pContext);

  private:
    static ImgSource* parseFilters(const QDomNode& filter);
    ColorSchemeParser() { }
    ~ColorSchemeParser() { }
};

#endif /* COLORSCHEMEPARSER_H */
