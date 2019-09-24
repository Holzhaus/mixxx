#include "track/seratomarkers2.h"

#include <QtEndian>

namespace mixxx {

SeratoMarkers2EntryPointer SeratoMarkers2CueEntry::parse(const QByteArray &data)
{
    // Unknown field, make sure it's 0 in case it's a
    // null-terminated string
    if (data.at(0) != '\x00') {
        return nullptr;
    }

    quint8 index(data.at(1));
    quint32 position(qFromBigEndian<quint32>(data.mid(2, 6)));

    // Unknown field, make sure it's 0 in case it's a
    // null-terminated string
    if (data.at(6) != '\x00') {
        return nullptr;
    }

    QColor color(static_cast<quint8>(data.at(7)),
                 static_cast<quint8>(data.at(8)),
                 static_cast<quint8>(data.at(9)));

    // Unknown field(s), make sure it's 0 in case it's a
    // null-terminated string
    if (data.at(10) != '\x00' || data.at(11) != '\x00') {
        return nullptr;
    }

    int labelEndPos = data.indexOf('\x00', 12);
    if (labelEndPos < 0) {
        return nullptr;
    }
    QString label(data.mid(12, labelEndPos - 12));

    if (data.length() > labelEndPos + 1) {
        return nullptr;
    }

    SeratoMarkers2CueEntry* pEntry = new SeratoMarkers2CueEntry(index, position, color, label);
    qDebug() << "SeratoMarkers2CueEntry" << *pEntry;
    return SeratoMarkers2EntryPointer(pEntry);
}

QByteArray SeratoMarkers2CueEntry::data() const {
    QByteArray data;
    data.resize(length());

    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << (quint8)0
           << m_index
           << m_position
           << (quint8)0
           << (quint8)m_color.red()
           << (quint8)m_color.green()
           << (quint8)m_color.blue()
           << (quint8)0
           << (quint8)0;

    QByteArray labelData = m_label.toUtf8();
    stream.writeRawData(labelData.constData(), labelData.length());

    return data;
}

quint32 SeratoMarkers2CueEntry::length() const {
    return (13 + m_label.length());
}

bool SeratoMarkers2::parse(SeratoMarkers2 *seratoMarkers2, const QByteArray &outerData) {
    if (outerData.left(2).compare("\x01\x01") != 0) {
        qDebug() << "Unknown outer Serato Markers2 tag version";
        return false;
    }

    QByteArray data(QByteArray::fromBase64(outerData.mid(2)));

    if (data.left(2).compare("\x01\x01") != 0) {
        qDebug() << "Unknown inner Serato Markers2 tag version";
        return false;
    }

    QList<std::shared_ptr<SeratoMarkers2Entry>> entries;

    int offset = 2;
    int entryTypeEndPos;
    while((entryTypeEndPos = data.indexOf('\x00', offset)) >= 0) {
        // Entry Name
        QString entryType(data.mid(offset, entryTypeEndPos - offset));
        offset = entryTypeEndPos + 1;

        // Entry Size
        quint32 entrySize(qFromBigEndian<quint32>(data.mid(offset, 4)));
        offset += 4;

        QByteArray entryData(data.mid(offset, entrySize));
        offset += entrySize;

        // Entry Content
        SeratoMarkers2EntryPointer pEntry;
        if(entryType.compare("CUE") == 0) {
            pEntry = SeratoMarkers2CueEntry::parse(entryData);
        } else {
            pEntry = SeratoMarkers2EntryPointer(new SeratoMarkers2UnknownEntry(entryType, entryData));
            qDebug() << "SeratoMarkers2UnknownEntry" << *pEntry;
        }

        if(!pEntry) {
            return false;
        }
        entries.append(pEntry);
    }

    seratoMarkers2->setEntries(entries);
    return true;
}

} //namespace mixxx
