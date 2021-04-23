#pragma once
#include <QNetworkAccessManager>

#include "network/jsonwebtask.h"
#include "update/updatelookuptask.h"
#include "update/updateversion.h"
#include "util/parented_ptr.h"

namespace mixxx {

enum class UpdateStatus {
    Idle,
    Checking,
    CheckFailed,
    AlreadyUpToDate,
    UpdateAvailable,
};
QDebug operator<<(QDebug debug, UpdateStatus status);

/// Class that provides a convenient, centralized way to check for app updates.
class UpdateChecker : public QObject {
    Q_OBJECT
  public:
    UpdateChecker();

    /// Return the update status.
    UpdateStatus status() {
        return m_status;
    }

    /// Return the the latest version (if available).
    std::optional<UpdateVersion> latestVersion() {
        return m_latestVersion;
    }

    /// Query the new status updates.
    void checkForUpdates();

  signals:
    /// Emitted whenever the status changes.
    void statusChanged(UpdateStatus);

  private slots:
    bool onUpdateLookupTaskTerminated();
    void slotUpdateLookupTaskSucceeded(
            const UpdateVersion& version);
    void slotUpdateLookupTaskFailed(
            const mixxx::network::JsonWebResponse& response);
    void slotUpdateLookupTaskAborted();
    void slotUpdateLookupTaskNetworkError(
            QNetworkReply::NetworkError errorCode,
            const QString& errorString,
            const mixxx::network::WebResponseWithContent& responseWithContent);

  private:
    UpdateStatus m_status;
    QNetworkAccessManager m_network;
    std::optional<UpdateVersion> m_latestVersion;
    parented_ptr<mixxx::UpdateLookupTask> m_pUpdateLookupTask;

    /// Set status and emit `statusChanged` signal if previous status was
    /// different.
    void setStatus(UpdateStatus status);
};

} // namespace mixxx
