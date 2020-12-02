#pragma once

#include <QByteArray>
#include <QByteArrayData>
#include <QFutureWatcher>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>

#include "musicbrainz/web/acoustidlookuptask.h"
#include "musicbrainz/web/musicbrainzrecordingstask.h"
#include "track/track_decl.h"
#include "util/parented_ptr.h"

class QUrl;
class QUuid;
namespace mixxx {
namespace musicbrainz {
struct TrackRelease;
} // namespace musicbrainz
namespace network {
struct JsonWebResponse;
struct WebResponse;
} // namespace network
} // namespace mixxx

class TagFetcher : public QObject {
    Q_OBJECT

    // Implements retrieval of metadata in 3 subsequent stages:
    //   1. Chromaprint -> AcoustID fingerprint
    //   2. AcoustID -> MusicBrainz recording UUIDs
    //   3. MusicBrainz -> MusicBrainz track releases

  public:
    explicit TagFetcher(
            QObject* parent = nullptr);
    ~TagFetcher() override = default;

    void startFetch(
            TrackPointer pTrack);

  public slots:
    void cancel();

  signals:
    void resultAvailable(
            TrackPointer pTrack,
            const QList<mixxx::musicbrainz::TrackRelease>& guessedTrackReleases);
    void fetchProgress(
            const QString& message);
    void networkError(
            int httpStatus,
            const QString& app,
            const QString& message,
            int code);

  private slots:
    void slotFingerprintReady();

    void slotAcoustIdTaskSucceeded(
            QList<QUuid> recordingIds);
    void slotAcoustIdTaskFailed(
            const mixxx::network::JsonWebResponse& response);
    void slotAcoustIdTaskAborted();
    void slotAcoustIdTaskNetworkError(
            const QUrl& requestUrl,
            QNetworkReply::NetworkError errorCode,
            const QString& errorString,
            const QByteArray& errorContent);

    void slotMusicBrainzTaskSucceeded(
            const QList<mixxx::musicbrainz::TrackRelease>& guessedTrackReleases);
    void slotMusicBrainzTaskFailed(
            const mixxx::network::WebResponse& response,
            int errorCode,
            const QString& errorMessage);
    void slotMusicBrainzTaskAborted();
    void slotMusicBrainzTaskNetworkError(
            const QUrl& requestUrl,
            QNetworkReply::NetworkError errorCode,
            const QString& errorString,
            const QByteArray& errorContent);

  private:
    QNetworkAccessManager m_network;

    QFutureWatcher<QString> m_fingerprintWatcher;

    parented_ptr<mixxx::AcoustIdLookupTask> m_pAcoustIdTask;

    parented_ptr<mixxx::MusicBrainzRecordingsTask> m_pMusicBrainzTask;

    TrackPointer m_pTrack;
};
