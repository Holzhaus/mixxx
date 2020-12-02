#ifndef MIXXX_CRATEFEATUREHELPER_H
#define MIXXX_CRATEFEATUREHELPER_H

#include <QByteArrayData>
#include <QObject>
#include <QString>

#include "library/crate/crate.h"
#include "library/crate/crateid.h"
#include "preferences/usersettings.h"

// forward declaration(s)
class TrackCollection;
class Crate;

class CrateFeatureHelper: public QObject {
    Q_OBJECT
  public:
    CrateFeatureHelper(
            TrackCollection* pTrackCollection,
            UserSettingsPointer pConfig);
    ~CrateFeatureHelper() override {}

    CrateId createEmptyCrate();
    CrateId duplicateCrate(const Crate& oldCrate);

  private:
    QString proposeNameForNewCrate(
            const QString& initialName = QString()) const;

    TrackCollection* m_pTrackCollection;

    UserSettingsPointer m_pConfig;
};


#endif // MIXXX_CRATEFEATUREHELPER_H
