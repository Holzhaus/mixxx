#pragma once
#include <QUrl>
#include <QVersionNumber>

namespace mixxx {

class UpdateVersion {
  public:
    UpdateVersion(const QVersionNumber& versionNumber,
            const QString& name,
            const QUrl& releaseAnnouncementUrl,
            const QUrl& downloadUrl)
            : m_number(versionNumber),
              m_name(name),
              m_releaseAnnouncementUrl(releaseAnnouncementUrl),
              m_downloadUrl(downloadUrl){};

    QString name() {
        return m_name;
    }

    QVersionNumber number() {
        return m_number;
    }

    QUrl releaseAnnouncementUrl() {
        return m_releaseAnnouncementUrl;
    }

    QUrl downloadUrl() {
        return m_downloadUrl;
    }

  private:
    QVersionNumber m_number;
    QString m_name;
    QUrl m_releaseAnnouncementUrl;
    QUrl m_downloadUrl;
};

} // namespace mixxx
