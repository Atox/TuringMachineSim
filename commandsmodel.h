#include <QAbstractListModel>
#include <QList>
#include "command.h"

#ifndef COMMANDSMODEL_H
#define COMMANDSMODEL_H

class CommandsListModel : public QAbstractListModel {
	Q_OBJECT

public:
	explicit CommandsListModel(QObject *parent = 0);
	void connect(QList<Turing::Command> *m_commands, uint *m_currentCommand);
	int rowCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;

private:
	QList<Turing::Command> *m_commands;
	uint *m_currentCommand;
};

#endif // COMMANDSMODEL_H
