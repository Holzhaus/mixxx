#pragma once

#include <QDir>
#include <QList>
#include <QSet>
#include <QWidget>

#include "preferences/usersettings.h"
#include "skin/skininfo.h"

namespace mixxx {
namespace skin {

class SkinLoader {
  public:
    SkinLoader(UserSettingsPointer pConfig);
    virtual ~SkinLoader();

    QWidget* loadConfiguredSkin(QWidget* pParent,
            QSet<ControlObject*>* skinCreatedControls,
            mixxx::CoreServices* pCoreServices);

    LaunchImage* loadLaunchImage(QWidget* pParent) const;

    SkinInfoPointer getSkin(const QString& skinName) const;
    SkinInfoPointer getConfiguredSkin() const;
    QString getDefaultSkinName() const;
    QList<QDir> getSkinSearchPaths() const;
    QList<SkinInfoPointer> getSkins() const;

  private:
    QString pickResizableSkin(const QString& oldSkin) const;
    SkinInfoPointer skinFromDirectory(const QDir& dir) const;

    UserSettingsPointer m_pConfig;
};

} // namespace skin
} // namespace mixxx
