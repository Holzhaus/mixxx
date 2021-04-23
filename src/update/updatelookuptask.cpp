#include <QJsonArray>
#include <QJsonObject>
#include <QMetaMethod>

#include "moc_updatelookuptask.cpp"
#include "musicbrainz/web/acoustidlookuptask.h"
#include "util/assert.h"
#include "util/logger.h"

namespace mixxx {

namespace {

const Logger kLogger("UpdateLookupTask");

const QString kBaseUrl = QStringLiteral("https://deploy-preview-237--mixxx-website.netlify.app/");
const QString kRequestPath = QStringLiteral("/download.json");

const QString kKeyStable = QStringLiteral("stable");
const QString kKeyVersion = QStringLiteral("version");
const QString kKeyName = QStringLiteral("name");
const QString kKeyReleaseAnnouncementUrl = QStringLiteral("release_announcement_url");
const QString kKeyDownloadUrl = QStringLiteral("download_url");

} // anonymous namespace

UpdateLookupTask::UpdateLookupTask(
        QNetworkAccessManager* networkAccessManager,
        QObject* parent)
        : network::JsonWebTask(
                  networkAccessManager,
                  kBaseUrl,
                  network::JsonWebRequest{
                          network::HttpRequestMethod::Get,
                          kRequestPath,
                          QUrlQuery(),
                          QJsonDocument(),
                  },
                  parent) {
}

void UpdateLookupTask::onFinished(
        const network::JsonWebResponse& response) {
    if (!response.isStatusCodeSuccess()) {
        kLogger.warning()
                << "Request failed with HTTP status code"
                << response.statusCode();
        emitFailed(response);
        return;
    }
    if (response.statusCode() != network::kHttpStatusCodeOk) {
        kLogger.warning()
                << "Unexpected HTTP status code"
                << response.statusCode();
        emitFailed(response);
        return;
    }

    if (!response.content().isObject()) {
        kLogger.warning()
                << "Invalid JSON content"
                << response.content();
        emitFailed(response);
        return;
    }
    const auto jsonRootObject = response.content().object();

    if (!jsonRootObject.value(kKeyStable).isObject()) {
        kLogger.warning()
                << "Invalid JSON content"
                << response.content();
        emitFailed(response);
        return;
    }

    const auto jsonVersionObject = jsonRootObject.value(kKeyStable).toObject();

    if (!jsonVersionObject.value(kKeyVersion).isString()) {
        kLogger.warning()
                << "Invalid member"
                << kKeyVersion
                << "in JSON content"
                << response.content();
        emitFailed(response);
        return;
    }
    QVersionNumber versionNumber = QVersionNumber::fromString(
            jsonVersionObject.value(kKeyVersion).toString());

    if (!jsonVersionObject.value(kKeyName).isString()) {
        kLogger.warning()
                << "Invalid member"
                << kKeyName
                << "in JSON content"
                << response.content();
        emitFailed(response);
        return;
    }
    QString name = jsonVersionObject.value(kKeyName).toString();

    QUrl releaseAnnouncementUrl;
    if (jsonVersionObject.value(kKeyReleaseAnnouncementUrl).isString()) {
        releaseAnnouncementUrl = QUrl(
                jsonVersionObject.value(kKeyReleaseAnnouncementUrl).toString());
    }

    if (!jsonVersionObject.value(kKeyDownloadUrl).isString()) {
        kLogger.warning()
                << "Invalid member"
                << kKeyDownloadUrl
                << "in JSON content"
                << response.content();
        emitFailed(response);
        return;
    }
    QUrl downloadUrl = QUrl(jsonVersionObject.value(kKeyDownloadUrl).toString());

    UpdateVersion version(versionNumber, name, releaseAnnouncementUrl, downloadUrl);
    emitSucceeded(version);
}

void UpdateLookupTask::emitSucceeded(
        const UpdateVersion& version) {
    VERIFY_OR_DEBUG_ASSERT(
            isSignalFuncConnected(&UpdateLookupTask::succeeded)) {
        kLogger.warning()
                << "Unhandled succeeded signal";
        deleteLater();
        return;
    }
    emit succeeded(version);
}

} // namespace mixxx
