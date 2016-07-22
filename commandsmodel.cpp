#include "commandsmodel.h"
#include "command.h"

CommandsListModel::CommandsListModel(QObject *parent) :
	QAbstractListModel(parent) {
	m_commands = 0;
}

void CommandsListModel::connect(QList<Turing::Command> *commands, uint *currentCommand) {
	this->m_commands = commands;
	this->m_currentCommand = currentCommand;
}

int CommandsListModel::rowCount(const QModelIndex &) const {
	return m_commands->count();
}

QVariant CommandsListModel::data(const QModelIndex &index, int role) const {
	if (role == Qt::DisplayRole)
		if (index.row() >= 0 && index.row() < m_commands->count()) {
			Turing::Command command = (*m_commands)[index.row()];
			if (m_currentCommand != 0 && *m_currentCommand == (uint)index.row())
				return command.asString() + QString(" «");
			else
				return command.asString();
		}
	return QVariant();
}
