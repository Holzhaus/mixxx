#include "preferences/dlgpreferencepage.h"

#include "defs_urls.h"

DlgPreferencePage::DlgPreferencePage(QWidget* pParent)
        : QWidget(pParent) {
}

DlgPreferencePage::~DlgPreferencePage() {
}

QString DlgPreferencePage::helpDocument() const {
    return QString();
}
