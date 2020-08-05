#include "macrodao.h"

#include <QDebug>
#include <QSqlQuery>

#include "library/queryutil.h"

MacroDAO::MacroDAO(const QSqlDatabase& database)
        : m_database(database) {
}

void MacroDAO::saveMacro(TrackId trackId,
        QString label,
        QVector<MacroAction> actions,
        Macro::State state) {
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
            "INSERT INTO macros "
            "(track_id, label, state, content) "
            "VALUES "
            "(:trackId, :label, :state, :content)"));
    query.bindValue(":trackId", trackId.toVariant());
    query.bindValue(":label", label);
    query.bindValue(":state", (int)state);
    query.bindValue(":content", Macro::serialize(actions));
    if (!query.exec()) {
        LOG_FAILED_QUERY(query);
        return;
    }
    qCDebug(macroLoggingCategory) << "Macro saved";
}

QList<Macro> MacroDAO::loadMacros(TrackId trackId) {
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
            "SELECT * FROM macros WHERE track_id=:trackId"));
    query.bindValue(":trackId", trackId.toVariant());
    QList<Macro> result;
    if (!query.exec()) {
        LOG_FAILED_QUERY(query);
        return result;
    }
    const QSqlRecord record = query.record();
    int stateColumn = record.indexOf("state");
    int labelColumn = record.indexOf("label");
    int contentColumn = record.indexOf("content");
    while (query.next()) {
        uint state = query.value(stateColumn).toUInt();
        result.append(Macro(
                query.value(labelColumn).toString(),
                Macro::deserialize(query.value(contentColumn).toByteArray()),
                Macro::State(state)));
    }
    return result;
}
