#pragma once

#include "library/dao/dao.h"
#include "track/macro.h"
#include "track/trackid.h"

class MacroDAO : public virtual DAO {
  public:
    bool saveMacro(TrackId trackId, Macro* macro, int slot = 0) const;
    void saveMacros(TrackId trackId, QMap<int, MacroPtr> macros) const;

    int getFreeSlot(TrackId trackId) const;
    QMap<int, MacroPtr> loadMacros(TrackId trackId) const;

  private:
    QSqlQuery querySelect(QString columns, TrackId trackId) const;
};
