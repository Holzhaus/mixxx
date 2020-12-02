//
// C++ Interface: parserpls
//
// Description: Interface header for the example parser PlsParser
//
//
// Author: Ingo Kossyk <kossyki@cs.tu-berlin.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PARSERPLS_H
#define PARSERPLS_H

#include <QByteArrayData>
#include <QList>
#include <QString>
#include <QTextStream>

#include "library/parser.h"

class QObject;
class QTextStream;

class ParserPls : public Parser {
    Q_OBJECT
  public:
    ParserPls();
    virtual ~ParserPls();
    /**Can be called to parse a pls file**/
    QList<QString> parse(const QString&);
    //Playlist Export
    static bool writePLSFile(const QString &file, const QList<QString> &items, bool useRelativePath);

  private:
    /**Returns the Number of entries in the pls file**/
    long getNumEntries(QTextStream*);
    /**Reads a line from the file and returns filepath**/
    QString getFilePath(QTextStream*, const QString& basePath);

};

#endif
