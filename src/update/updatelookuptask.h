#pragma once

#include "network/jsonwebtask.h"
#include "update/updateversion.h"

namespace mixxx {

class UpdateLookupTask : public network::JsonWebTask {
    Q_OBJECT

  public:
    UpdateLookupTask(
            QNetworkAccessManager* networkAccessManager,
            QObject* parent = nullptr);
    ~UpdateLookupTask() override = default;

  signals:
    void succeeded(const UpdateVersion& version);

  private:
    void onFinished(
            const network::JsonWebResponse& response) override;

    void emitSucceeded(const UpdateVersion& version);

    const QUrlQuery m_urlQuery;
};

} // namespace mixxx
