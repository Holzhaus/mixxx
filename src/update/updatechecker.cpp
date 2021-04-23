#include "updatechecker.h"

#include "moc_updatechecker.cpp"
#include "util/logger.h"
#include "util/thread_affinity.h"

namespace mixxx {

namespace {
const Logger kLogger("mixxx::update::UpdateChecker");

constexpr int kUpdateLookupTaskTimeoutMillis = 1000;
}

UpdateChecker::UpdateChecker()
        : QObject(), m_status(UpdateStatus::Idle) {
}

void UpdateChecker::setStatus(UpdateStatus status) {
    if (m_status != status) {
        m_status = status;
        kLogger.info() << "Update status changed to" << status;
        emit statusChanged(m_status);
    }
}

void UpdateChecker::checkForUpdates() {
    DEBUG_ASSERT_QOBJECT_THREAD_AFFINITY(this);
    VERIFY_OR_DEBUG_ASSERT(m_status != UpdateStatus::Checking && !m_pUpdateLookupTask) {
        return;
    }

    setStatus(UpdateStatus::Checking);
    m_pUpdateLookupTask = make_parented<mixxx::UpdateLookupTask>(
            &m_network,
            this);
    connect(m_pUpdateLookupTask,
            &mixxx::UpdateLookupTask::succeeded,
            this,
            &UpdateChecker::slotUpdateLookupTaskSucceeded);
    connect(m_pUpdateLookupTask,
            &mixxx::UpdateLookupTask::failed,
            this,
            &UpdateChecker::slotUpdateLookupTaskFailed);
    connect(m_pUpdateLookupTask,
            &mixxx::UpdateLookupTask::aborted,
            this,
            &UpdateChecker::slotUpdateLookupTaskAborted);
    connect(m_pUpdateLookupTask,
            &mixxx::UpdateLookupTask::networkError,
            this,
            &UpdateChecker::slotUpdateLookupTaskNetworkError);
    m_pUpdateLookupTask->invokeStart(kUpdateLookupTaskTimeoutMillis);
}

bool UpdateChecker::onUpdateLookupTaskTerminated() {
    DEBUG_ASSERT_QOBJECT_THREAD_AFFINITY(this);
    auto* const pUpdateLookupTask = m_pUpdateLookupTask.get();
    DEBUG_ASSERT(sender());
    VERIFY_OR_DEBUG_ASSERT(pUpdateLookupTask ==
            qobject_cast<mixxx::UpdateLookupTask*>(sender())) {
        return false;
    }
    m_pUpdateLookupTask = nullptr;
    const auto taskDeleter = mixxx::ScopedDeleteLater(pUpdateLookupTask);
    pUpdateLookupTask->disconnect(this);
    return true;
}

void UpdateChecker::slotUpdateLookupTaskSucceeded(
        const UpdateVersion& version) {
    if (!onUpdateLookupTaskTerminated()) {
        return;
    }

    // TODO: Actually compare version with the currently running version
    m_latestVersion = std::optional(version);
    setStatus(UpdateStatus::AlreadyUpToDate);
}

void UpdateChecker::slotUpdateLookupTaskFailed(
        const mixxx::network::JsonWebResponse& response) {
    Q_UNUSED(response);

    DEBUG_ASSERT_QOBJECT_THREAD_AFFINITY(this);
    if (!onUpdateLookupTaskTerminated()) {
        return;
    }

    m_latestVersion = std::nullopt;
    setStatus(UpdateStatus::CheckFailed);
}

void UpdateChecker::slotUpdateLookupTaskAborted() {
    DEBUG_ASSERT_QOBJECT_THREAD_AFFINITY(this);
    if (!onUpdateLookupTaskTerminated()) {
        return;
    }

    m_latestVersion = std::nullopt;
    setStatus(UpdateStatus::CheckFailed);
}

void UpdateChecker::slotUpdateLookupTaskNetworkError(
        QNetworkReply::NetworkError errorCode,
        const QString& errorString,
        const mixxx::network::WebResponseWithContent& responseWithContent) {
    Q_UNUSED(errorCode);
    Q_UNUSED(errorString);
    Q_UNUSED(responseWithContent);

    DEBUG_ASSERT_QOBJECT_THREAD_AFFINITY(this);
    if (!onUpdateLookupTaskTerminated()) {
        return;
    }

    m_latestVersion = std::nullopt;
    setStatus(UpdateStatus::CheckFailed);
}

QDebug operator<<(QDebug debug, UpdateStatus status) {
    switch (status) {
    case UpdateStatus::Idle:
        debug << "Idle";
        break;
    case UpdateStatus::Checking:
        debug << "Checking";
        break;
    case UpdateStatus::CheckFailed:
        debug << "CheckFailed";
        break;
    case UpdateStatus::AlreadyUpToDate:
        debug << "AlreadyUpToDate";
        break;
    case UpdateStatus::UpdateAvailable:
        debug << "UpdateAvailable";
        break;
    }
    return debug;
}

} // namespace mixxx
