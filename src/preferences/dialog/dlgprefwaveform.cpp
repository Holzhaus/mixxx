#include "preferences/dialog/dlgprefwaveform.h"

#include "library/dao/analysisdao.h"
#include "library/library.h"
#include "moc_dlgprefwaveform.cpp"
#include "preferences/waveformsettings.h"
#include "util/db/dbconnectionpooled.h"
#include "waveform/renderers/waveformwidgetrenderer.h"
#include "waveform/waveformwidgetfactory.h"
#ifdef MIXXX_USE_QML
#include "util/cmdlineargs.h"
#endif

DlgPrefWaveform::DlgPrefWaveform(
        QWidget* pParent,
        UserSettingsPointer pConfig,
        std::shared_ptr<Library> pLibrary)
        : DlgPreferencePage(pParent),
          m_pConfig(pConfig),
          m_pLibrary(pLibrary) {
    setupUi(this);

    // Waveform overview init
#ifdef MIXXX_USE_QML
    if (!CmdlineArgs::Instance().isQml()) {
        waveformOverviewComboBox->addItem(tr("Filtered"), 0); // "0"
        waveformOverviewComboBox->addItem(tr("HSV"), 1);      // "1"
    }
#endif
    waveformOverviewComboBox->addItem(tr("RGB"), 2); // "2"

#ifdef MIXXX_USE_QML
    if (CmdlineArgs::Instance().isQml()) {
        waveformTypeComboBox->addItem("RGB (GLSL)", 0);
    } else
#endif
    {
        // Populate waveform options.
        WaveformWidgetFactory* factory = WaveformWidgetFactory::instance();
        DEBUG_ASSERT(factory);
        // We assume that the original type list order remains constant.
        // We will use the type index later on to set waveform types and to
        // update the combobox.
        QVector<WaveformWidgetAbstractHandle> handles = factory->getAvailableTypes();
        for (int i = 0; i < handles.size(); ++i) {
            waveformTypeComboBox->addItem(handles[i].getDisplayName(), i);
        }
    }
    // Sort the combobox items alphabetically
    waveformTypeComboBox->model()->sort(0);

    // Populate zoom options.
    for (int i = static_cast<int>(WaveformWidgetRenderer::s_waveformMinZoom);
            i <= static_cast<int>(WaveformWidgetRenderer::s_waveformMaxZoom);
            i++) {
        defaultZoomComboBox->addItem(QString::number(100 / static_cast<double>(i), 'f', 1) + " %");
    }

    // The GUI is not fully setup so connecting signals before calling
    // slotUpdate can generate rebootMixxxView calls.
    // TODO(XXX): Improve this awkwardness.
    slotUpdate();

    connect(frameRateSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &DlgPrefWaveform::slotSetFrameRate);
    connect(endOfTrackWarningTimeSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &DlgPrefWaveform::slotSetWaveformEndRender);
    connect(beatGridAlphaSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &DlgPrefWaveform::slotSetBeatGridAlpha);
    connect(frameRateSlider,
            &QSlider::valueChanged,
            frameRateSpinBox,
            &QSpinBox::setValue);
    connect(frameRateSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            frameRateSlider,
            &QSlider::setValue);
    connect(endOfTrackWarningTimeSlider,
            &QSlider::valueChanged,
            endOfTrackWarningTimeSpinBox,
            &QSpinBox::setValue);
    connect(endOfTrackWarningTimeSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            endOfTrackWarningTimeSlider,
            &QSlider::setValue);
    connect(beatGridAlphaSlider,
            &QSlider::valueChanged,
            beatGridAlphaSpinBox,
            &QSpinBox::setValue);
    connect(beatGridAlphaSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            beatGridAlphaSlider,
            &QSlider::setValue);

    connect(waveformTypeComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &DlgPrefWaveform::slotSetWaveformType);
    connect(defaultZoomComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &DlgPrefWaveform::slotSetDefaultZoom);
    connect(synchronizeZoomCheckBox,
            &QCheckBox::clicked,
            this,
            &DlgPrefWaveform::slotSetZoomSynchronization);
    connect(constantScrollingRateCheckBox,
            &QCheckBox::clicked,
            this,
            &DlgPrefWaveform::slotSetConstantScrollingRate);
    connect(allVisualGain,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &DlgPrefWaveform::slotSetVisualGainAll);
    connect(lowVisualGain,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &DlgPrefWaveform::slotSetVisualGainLow);
    connect(midVisualGain,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &DlgPrefWaveform::slotSetVisualGainMid);
    connect(highVisualGain,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &DlgPrefWaveform::slotSetVisualGainHigh);
    connect(normalizeOverviewCheckBox,
            &QCheckBox::toggled,
            this,
            &DlgPrefWaveform::slotSetNormalizeOverview);
#ifdef MIXXX_USE_QML
    if (CmdlineArgs::Instance().isQml()) {
        frameRateAverage->hide();
        frameRateAverage->hide();
        frameRateSpinBox->hide();
        frameRateSlider->hide();
        frameRateLabel->hide();

        // TODO(XXX) to remove when end of track CO is available on QML
        endOfTrackWarningTimeLabel->setEnabled(false);
        endOfTrackWarningTimeSlider->setEnabled(false);
        endOfTrackWarningTimeSpinBox->setEnabled(false);
        endOfTrackWarningTimeSlider->setToolTip(
                endOfTrackWarningTimeSlider->toolTip() +
                tr(" Not yet available in the new interface."));
        endOfTrackWarningTimeSpinBox->setToolTip(
                endOfTrackWarningTimeSpinBox->toolTip() +
                tr(" Not yet available in the new interface."));
    } else
#endif
    {
        constantScrollingRateCheckBox->hide();
        WaveformWidgetFactory* factory = WaveformWidgetFactory::instance();
        DEBUG_ASSERT(factory);
        connect(factory,
                &WaveformWidgetFactory::waveformMeasured,
                this,
                &DlgPrefWaveform::slotWaveformMeasured);
    }
    connect(waveformOverviewComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &DlgPrefWaveform::slotSetWaveformOverviewType);
    connect(clearCachedWaveforms,
            &QAbstractButton::clicked,
            this,
            &DlgPrefWaveform::slotClearCachedWaveforms);
    connect(playMarkerPositionSlider,
            &QSlider::valueChanged,
            this,
            &DlgPrefWaveform::slotSetPlayMarkerPosition);

    setScrollSafeGuardForAllInputWidgets(this);
}

DlgPrefWaveform::~DlgPrefWaveform() {
}

void DlgPrefWaveform::slotUpdate() {
    int currentIndex = -1;

#ifdef MIXXX_USE_QML
    if (CmdlineArgs::Instance().isQml()) {
        openGlStatusLabel->hide();
        openGlStatusData->hide();
        currentIndex = m_pConfig->getValue(ConfigKey("[Waveform]", "WaveformTypeQml"), 0);
    } else
#endif
    {
        WaveformWidgetFactory* factory = WaveformWidgetFactory::instance();
        if (factory->isOpenGlAvailable() || factory->isOpenGlesAvailable()) {
            openGlStatusData->setText(factory->getOpenGLVersion());
        } else {
            openGlStatusData->setText(tr("OpenGL not available") + ": " +
                    factory->getOpenGLVersion());
        }

        // The combobox holds a list of [handle name, handle index]
        currentIndex = waveformTypeComboBox->findData(factory->getHandleIndex());
    }
    if (currentIndex != -1 && waveformTypeComboBox->currentIndex() != currentIndex) {
        waveformTypeComboBox->setCurrentIndex(currentIndex);
    }

#ifdef MIXXX_USE_QML
    if (CmdlineArgs::Instance().isQml()) {
        // m_config->setValue(ConfigKey("[Waveform]", "WaveformTypeQml")
        int aEndOfTrackWarning = m_pConfig->getValue(
                ConfigKey("[Waveform]", "EndOfTrackWarningTime"), 30);
        endOfTrackWarningTimeSpinBox->setValue(aEndOfTrackWarning);
        endOfTrackWarningTimeSlider->setValue(aEndOfTrackWarning);

        synchronizeZoomCheckBox->setChecked(m_pConfig->getValue(
                ConfigKey("[Waveform]", "ZoomSynchronization"), true));
        constantScrollingRateCheckBox->setChecked(m_pConfig->getValue(
                ConfigKey("[Waveform]", "ConstantRate"), true));
        allVisualGain->setValue(m_pConfig->getValue(
                ConfigKey("[Waveform]",
                        "VisualGain_" +
                                QString::number(WaveformWidgetFactory::All)),
                1.0));
        lowVisualGain->setValue(m_pConfig->getValue(
                ConfigKey("[Waveform]",
                        "VisualGain_" +
                                QString::number(WaveformWidgetFactory::Low)),
                1.0));
        midVisualGain->setValue(m_pConfig->getValue(
                ConfigKey("[Waveform]",
                        "VisualGain_" +
                                QString::number(WaveformWidgetFactory::Mid)),
                1.0));
        highVisualGain->setValue(m_pConfig->getValue(
                ConfigKey("[Waveform]",
                        "VisualGain_" +
                                QString::number(WaveformWidgetFactory::High)),
                1.0));
        normalizeOverviewCheckBox->setChecked(m_pConfig->getValue(
                ConfigKey("[Waveform]", "OverviewNormalized"), true));
        // Round zoom to int to get a default zoom index.
        defaultZoomComboBox->setCurrentIndex(
                static_cast<int>(m_pConfig->getValue(
                        ConfigKey("[Waveform]", "DefaultZoom"),
                        WaveformWidgetRenderer::s_waveformDefaultZoom)) -
                1);
        playMarkerPositionSlider->setValue(static_cast<int>(
                m_pConfig->getValue(
                        ConfigKey("[Waveform]", "PlayMarkerPosition"),
                        WaveformWidgetRenderer::s_defaultPlayMarkerPosition) *
                100.0));

        int aBeatGridAlpha = m_pConfig->getValue(ConfigKey("[Waveform]", "beatGridAlpha"), 90);
        beatGridAlphaSpinBox->setValue(aBeatGridAlpha);
        beatGridAlphaSlider->setValue(aBeatGridAlpha);
    } else
#endif
    {
        WaveformWidgetFactory* factory = WaveformWidgetFactory::instance();
        frameRateSpinBox->setValue(factory->getFrameRate());
        frameRateSlider->setValue(factory->getFrameRate());
        endOfTrackWarningTimeSpinBox->setValue(factory->getEndOfTrackWarningTime());
        endOfTrackWarningTimeSlider->setValue(factory->getEndOfTrackWarningTime());
        synchronizeZoomCheckBox->setChecked(factory->isZoomSync());
        allVisualGain->setValue(factory->getVisualGain(WaveformWidgetFactory::All));
        lowVisualGain->setValue(factory->getVisualGain(WaveformWidgetFactory::Low));
        midVisualGain->setValue(factory->getVisualGain(WaveformWidgetFactory::Mid));
        highVisualGain->setValue(factory->getVisualGain(WaveformWidgetFactory::High));
        normalizeOverviewCheckBox->setChecked(factory->isOverviewNormalized());
        // Round zoom to int to get a default zoom index.
        defaultZoomComboBox->setCurrentIndex(static_cast<int>(factory->getDefaultZoom()) - 1);
        playMarkerPositionSlider->setValue(
                static_cast<int>(factory->getPlayMarkerPosition() * 100));
        beatGridAlphaSpinBox->setValue(factory->getBeatGridAlpha());
        beatGridAlphaSlider->setValue(factory->getBeatGridAlpha());
    }

    // By default we set RGB woverview = "2"
    int overviewType = waveformOverviewComboBox->findData(m_pConfig->getValue(
            ConfigKey("[Waveform]", "WaveformOverviewType"), 2));
    qDebug() << "WaveformOverviewType:" << overviewType;
    if (overviewType != waveformOverviewComboBox->currentIndex()) {
        waveformOverviewComboBox->setCurrentIndex(overviewType);
    }

    WaveformSettings waveformSettings(m_pConfig);
    enableWaveformCaching->setChecked(waveformSettings.waveformCachingEnabled());
    enableWaveformGenerationWithAnalysis->setChecked(
        waveformSettings.waveformGenerationWithAnalysisEnabled());
    calculateCachedWaveformDiskUsage();
}

void DlgPrefWaveform::slotApply() {
    ConfigValue overviewtype = ConfigValue(
            waveformOverviewComboBox
                    ->itemData(waveformOverviewComboBox->currentIndex())
                    .toInt());
    if (overviewtype != m_pConfig->get(ConfigKey("[Waveform]", "WaveformOverviewType"))) {
        m_pConfig->set(ConfigKey("[Waveform]", "WaveformOverviewType"), overviewtype);
    }
    WaveformSettings waveformSettings(m_pConfig);
    waveformSettings.setWaveformCachingEnabled(enableWaveformCaching->isChecked());
    waveformSettings.setWaveformGenerationWithAnalysisEnabled(
        enableWaveformGenerationWithAnalysis->isChecked());

#ifdef MIXXX_USE_QML
    if (CmdlineArgs::Instance().isQml()) {
        QMessageBox::information(this,
                tr("Information"),
                tr("Mixxx must be restarted before some of the waveform settings take effect."),
                QMessageBox::Ok);
    }
#endif
}

void DlgPrefWaveform::slotResetToDefaults() {
#ifdef MIXXX_USE_QML
    if (!CmdlineArgs::Instance().isQml())
#endif
    {
        WaveformWidgetFactory* factory = WaveformWidgetFactory::instance();

        // Get the default we ought to use based on whether the user has OpenGL or not.
        // Select the combobox index that holds the default handle's index in data column.
        int defaultIndex = waveformTypeComboBox->findData(
                factory->findHandleIndexFromType(factory->autoChooseWidgetType()));
        if (defaultIndex != -1 && waveformTypeComboBox->currentIndex() != defaultIndex) {
            waveformTypeComboBox->setCurrentIndex(defaultIndex);
        }
    }

    allVisualGain->setValue(1.0);
    lowVisualGain->setValue(1.0);
    midVisualGain->setValue(1.0);
    highVisualGain->setValue(1.0);

    // Default zoom level is 3 in WaveformWidgetFactory.
    defaultZoomComboBox->setCurrentIndex(3 + 1);

    synchronizeZoomCheckBox->setChecked(true);

    // RGB overview.
    waveformOverviewComboBox->setCurrentIndex(2);

    // Don't normalize overview.
    normalizeOverviewCheckBox->setChecked(false);

    // 60FPS is the default
    frameRateSlider->setValue(60);
    endOfTrackWarningTimeSlider->setValue(30);

    // Waveform caching enabled.
    enableWaveformCaching->setChecked(true);
    enableWaveformGenerationWithAnalysis->setChecked(false);

    // Beat grid alpha default is 90
    beatGridAlphaSlider->setValue(90);
    beatGridAlphaSpinBox->setValue(90);

    // 50 (center) is default
    playMarkerPositionSlider->setValue(50);
}

void DlgPrefWaveform::slotSetFrameRate(int frameRate) {
#ifdef MIXXX_USE_QML
    if (!CmdlineArgs::Instance().isQml())
#endif
    {
        WaveformWidgetFactory::instance()->setFrameRate(frameRate);
    }
}

void DlgPrefWaveform::slotSetWaveformEndRender(int endTime) {
#ifdef MIXXX_USE_QML
    if (CmdlineArgs::Instance().isQml()) {
        m_pConfig->setValue(ConfigKey("[Waveform]", "EndOfTrackWarningTime"), endTime);
    } else
#endif
    {
        WaveformWidgetFactory::instance()->setEndOfTrackWarningTime(endTime);
    }
}

void DlgPrefWaveform::slotSetWaveformType(int index) {
    // Ignore sets for -1 since this happens when we clear the combobox.
    if (index < 0) {
        return;
    }
    int handleIndex = waveformTypeComboBox->itemData(index).toInt();
#ifdef MIXXX_USE_QML
    if (CmdlineArgs::Instance().isQml()) {
        m_pConfig->setValue(ConfigKey("[Waveform]", "WaveformTypeQml"), handleIndex);
    } else
#endif
    {
        WaveformWidgetFactory::instance()->setWidgetTypeFromHandle(handleIndex);
    }
}

void DlgPrefWaveform::slotSetWaveformOverviewType(int index) {
    int handleIndex = waveformOverviewComboBox->itemData(index).toInt();
    m_pConfig->set(ConfigKey("[Waveform]", "WaveformOverviewType"), ConfigValue(handleIndex));
    emit reloadUserInterface();
}

void DlgPrefWaveform::slotSetDefaultZoom(int index) {
#ifdef MIXXX_USE_QML
    if (CmdlineArgs::Instance().isQml()) {
        m_pConfig->setValue(ConfigKey("[Waveform]", "DefaultZoom"), (double)index + 1.0);
    } else
#endif
    {
        WaveformWidgetFactory::instance()->setDefaultZoom(index + 1);
    }
}

void DlgPrefWaveform::slotSetZoomSynchronization(bool checked) {
#ifdef MIXXX_USE_QML
    if (CmdlineArgs::Instance().isQml()) {
        m_pConfig->setValue(ConfigKey("[Waveform]", "ZoomSynchronization"), checked);
    } else
#endif
    {
        WaveformWidgetFactory::instance()->setZoomSync(checked);
    }
}

#ifdef MIXXX_USE_QML
void DlgPrefWaveform::slotSetConstantScrollingRate(bool checked) {
    if (CmdlineArgs::Instance().isQml()) {
        m_pConfig->setValue(ConfigKey("[Waveform]", "ConstantRate"), checked);
    }
}
#endif

void DlgPrefWaveform::slotSetVisualGainAll(double gain) {
#ifdef MIXXX_USE_QML
    if (CmdlineArgs::Instance().isQml()) {
        m_pConfig->setValue(
                ConfigKey("[Waveform]",
                        "VisualGain_" +
                                QString::number(WaveformWidgetFactory::All)),
                gain);
    } else
#endif
    {
        WaveformWidgetFactory::instance()->setVisualGain(WaveformWidgetFactory::All, gain);
    }
}

void DlgPrefWaveform::slotSetVisualGainLow(double gain) {
#ifdef MIXXX_USE_QML
    if (CmdlineArgs::Instance().isQml()) {
        m_pConfig->setValue(
                ConfigKey("[Waveform]",
                        "VisualGain_" +
                                QString::number(WaveformWidgetFactory::Low)),
                gain);
    } else
#endif
    {
        WaveformWidgetFactory::instance()->setVisualGain(WaveformWidgetFactory::Low, gain);
    }
}

void DlgPrefWaveform::slotSetVisualGainMid(double gain) {
#ifdef MIXXX_USE_QML
    if (CmdlineArgs::Instance().isQml()) {
        m_pConfig->setValue(
                ConfigKey("[Waveform]",
                        "VisualGain_" +
                                QString::number(WaveformWidgetFactory::Mid)),
                gain);
    } else
#endif
    {
        WaveformWidgetFactory::instance()->setVisualGain(WaveformWidgetFactory::Mid, gain);
    }
}

void DlgPrefWaveform::slotSetVisualGainHigh(double gain) {
#ifdef MIXXX_USE_QML
    if (CmdlineArgs::Instance().isQml()) {
        m_pConfig->setValue(
                ConfigKey("[Waveform]",
                        "VisualGain_" +
                                QString::number(WaveformWidgetFactory::High)),
                gain);
    } else
#endif
    {
        WaveformWidgetFactory::instance()->setVisualGain(WaveformWidgetFactory::High, gain);
    }
}

void DlgPrefWaveform::slotSetNormalizeOverview(bool normalize) {
#ifdef MIXXX_USE_QML
    if (CmdlineArgs::Instance().isQml()) {
        m_pConfig->setValue(ConfigKey("[Waveform]", "OverviewNormalized"), normalize);
    } else
#endif
    {
        WaveformWidgetFactory::instance()->setOverviewNormalized(normalize);
    }
}

void DlgPrefWaveform::slotWaveformMeasured(float frameRate, int droppedFrames) {
    frameRateAverage->setText(
            QString::number((double)frameRate, 'f', 2) + " : " +
            tr("dropped frames") + " " + QString::number(droppedFrames));
}

void DlgPrefWaveform::slotClearCachedWaveforms() {
    AnalysisDao analysisDao(m_pConfig);
    QSqlDatabase dbConnection = mixxx::DbConnectionPooled(m_pLibrary->dbConnectionPool());
    analysisDao.deleteAnalysesByType(dbConnection, AnalysisDao::TYPE_WAVEFORM);
    analysisDao.deleteAnalysesByType(dbConnection, AnalysisDao::TYPE_WAVESUMMARY);
    calculateCachedWaveformDiskUsage();
}

void DlgPrefWaveform::slotSetBeatGridAlpha(int alpha) {
    m_pConfig->setValue(ConfigKey("[Waveform]", "beatGridAlpha"), alpha);
#ifdef MIXXX_USE_QML
    if (!CmdlineArgs::Instance().isQml())
#endif
    {
        WaveformWidgetFactory::instance()->setDisplayBeatGridAlpha(alpha);
    }
}

void DlgPrefWaveform::slotSetPlayMarkerPosition(int position) {
    // QSlider works with integer values, so divide the percentage given by the
    // slider value by 100 to get a fraction of the waveform width.
#ifdef MIXXX_USE_QML
    if (CmdlineArgs::Instance().isQml()) {
        m_pConfig->setValue(ConfigKey("[Waveform]", "PlayMarkerPosition"), position / 100.0);
    } else
#endif
    {
        WaveformWidgetFactory::instance()->setPlayMarkerPosition(position / 100.0);
    }
}

void DlgPrefWaveform::calculateCachedWaveformDiskUsage() {
    AnalysisDao analysisDao(m_pConfig);
    QSqlDatabase dbConnection = mixxx::DbConnectionPooled(m_pLibrary->dbConnectionPool());
    size_t numBytes = analysisDao.getDiskUsageInBytes(dbConnection, AnalysisDao::TYPE_WAVEFORM) +
            analysisDao.getDiskUsageInBytes(dbConnection, AnalysisDao::TYPE_WAVESUMMARY);

    // Display total cached waveform size in mebibytes with 2 decimals.
    QString sizeMebibytes = QString::number(
            numBytes / (1024.0 * 1024.0), 'f', 2);

    waveformDiskUsage->setText(
            tr("Cached waveforms occupy %1 MiB on disk.").arg(sizeMebibytes));
}
