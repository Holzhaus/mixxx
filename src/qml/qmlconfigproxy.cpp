#include "qml/qmlconfigproxy.h"

#include "moc_qmlconfigproxy.cpp"
#include "preferences/colorpalettesettings.h"

namespace {
QVariantList paletteToQColorList(const ColorPalette& palette) {
    QVariantList colors;
    for (mixxx::RgbColor rgbColor : palette) {
        colors.append(mixxx::RgbColor::toQVariantColor(rgbColor));
    }
    return colors;
}

const QString kPreferencesGroup = QStringLiteral("[Preferences]");
const QString kMultiSamplingKey = QStringLiteral("multi_sampling");

} // namespace

namespace mixxx {
namespace qml {

QmlConfigProxy::QmlConfigProxy(
        UserSettingsPointer pConfig, QObject* parent)
        : QObject(parent), m_pConfig(pConfig) {
}

QVariantList QmlConfigProxy::getHotcueColorPalette() {
    ColorPaletteSettings colorPaletteSettings(m_pConfig);
    return paletteToQColorList(colorPaletteSettings.getHotcueColorPalette());
}

QVariantList QmlConfigProxy::getTrackColorPalette() {
    ColorPaletteSettings colorPaletteSettings(m_pConfig);
    return paletteToQColorList(colorPaletteSettings.getTrackColorPalette());
}

int QmlConfigProxy::getMultiSamplingLevel() {
    return m_pConfig->getValue(ConfigKey(kPreferencesGroup, kMultiSamplingKey), 0);
}

QString QmlConfigProxy::getString(QString group, QString item, QString defaultValue) {
    return m_pConfig->getValue(ConfigKey(group, item), defaultValue);
}

int QmlConfigProxy::getInt(QString group, QString item, int defaultValue) {
    return m_pConfig->getValue(ConfigKey(group, item), defaultValue);
}

bool QmlConfigProxy::getBool(QString group, QString item, bool defaultValue) {
    return m_pConfig->getValue(ConfigKey(group, item), defaultValue);
}

double QmlConfigProxy::getDouble(QString group, QString item, double defaultValue) {
    return m_pConfig->getValue(ConfigKey(group, item), defaultValue);
}

// static
QmlConfigProxy* QmlConfigProxy::create(QQmlEngine* pQmlEngine, QJSEngine* pJsEngine) {
    // The implementation of this method is mostly taken from the code example
    // that shows the replacement for `qmlRegisterSingletonInstance()` when
    // using `QML_SINGLETON`.
    // https://doc.qt.io/qt-6/qqmlengine.html#QML_SINGLETON

    // The instance has to exist before it is used. We cannot replace it.
    VERIFY_OR_DEBUG_ASSERT(s_pUserSettings) {
        qWarning() << "UserSettings hasn't been registered yet";
        return nullptr;
    }
    return new QmlConfigProxy(s_pUserSettings, pQmlEngine);
}

} // namespace qml
} // namespace mixxx
