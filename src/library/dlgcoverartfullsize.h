#pragma once

#include <QDialog>
#include <QPixmap>
#include <QPoint>
#include <QString>
#include <QTimer>
#include <QtCore>

#include "library/coverart.h"
#include "library/ui_dlgcoverartfullsize.h"
#include "mixer/basetrackplayer.h"
#include "track/track_decl.h"
#include "util/parented_ptr.h"
#include "widget/wcoverartmenu.h"

class BaseTrackPlayer;
class CoverInfo;
class CoverInfoRelative;
class QCloseEvent;
class QMouseEvent;
class QResizeEvent;
class QWheelEvent;
class QWidget;

class DlgCoverArtFullSize
        : public QDialog,
          public Ui::DlgCoverArtFullSize {
    Q_OBJECT
  public:
    explicit DlgCoverArtFullSize(QWidget* parent, BaseTrackPlayer* pPlayer = nullptr);
    ~DlgCoverArtFullSize() override = default;

    void init(TrackPointer pTrack);
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* ) override;
    void mouseMoveEvent(QMouseEvent* ) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

  public slots:
    void slotLoadTrack(TrackPointer);
    void slotCoverFound(
            const QObject* pRequestor,
            const CoverInfo& coverInfo,
            const QPixmap& pixmap,
            quint16 requestedHash,
            bool coverInfoUpdated);
    void slotTrackCoverArtUpdated();

    // slots that handle signals from WCoverArtMenu
    void slotCoverMenu(const QPoint& pos);
    void slotCoverInfoSelected(const CoverInfoRelative& coverInfo);
    void slotReloadCoverArt();

  private:
    QPixmap m_pixmap;
    TrackPointer m_pLoadedTrack;
    BaseTrackPlayer* m_pPlayer;
    parented_ptr<WCoverArtMenu> m_pCoverMenu;
    QTimer m_clickTimer;
    QPoint m_dragStartPosition;
    bool m_coverPressed;
};
