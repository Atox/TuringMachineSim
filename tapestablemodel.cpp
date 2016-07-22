#include "tapestablemodel.h"

#include <QColor>
#include <QFont>

TapesTableModel::TapesTableModel(QObject *parent) : QAbstractTableModel(parent) {
	m_tapes = 0;
	m_headPosition = 0;
	m_leftIndex = 0;
	m_rightIndex = 15;
}

void TapesTableModel::connect(QList<Turing::Tape> *tapes) {
	m_tapes = tapes;
}

void TapesTableModel::setHeadPosition(int headPosition) {
	m_headPosition = headPosition;
	emit dataChanged(QModelIndex(), QModelIndex());
}

void TapesTableModel::setFOV(int leftIndex, int rightIndex) {
	int oldTotal = m_rightIndex - m_leftIndex + 1;
	m_leftIndex = leftIndex;
	m_rightIndex = rightIndex;
	int total = m_rightIndex - m_leftIndex + 1;
	while (total > oldTotal) {
		beginInsertColumns(QModelIndex(), oldTotal, oldTotal);
		endInsertColumns();
		oldTotal++;
	}
	while (total < oldTotal) {
		oldTotal--;
		beginRemoveColumns(QModelIndex(), oldTotal, oldTotal);
		endRemoveColumns();
	}
	emit dataChanged(QModelIndex(), QModelIndex());
}

void TapesTableModel::clear() {
	int toRemove = m_tapes->count();
	while (toRemove > 0) {
		beginRemoveRows(QModelIndex(), 0, 0);
		endRemoveRow();
		toRemove--;
	}
}

int TapesTableModel::headPosition() {
	return m_headPosition;
}

int TapesTableModel::leftIndex() {
	return m_leftIndex;
}

int TapesTableModel::rightIndex() {
	return m_rightIndex;
}

int TapesTableModel::rowCount(const QModelIndex &) const {
	return m_tapes->count();
}

int TapesTableModel::columnCount(const QModelIndex &) const {
	return m_rightIndex - m_leftIndex + 1;
}

QVariant TapesTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Vertical)
			return QString::number(section + 1);
		else if (orientation == Qt::Horizontal)
			return QString::number(m_leftIndex + section);
		else
			return QVariant();
	}
	else
		return QVariant();
}

QVariant TapesTableModel::data(const QModelIndex &index, int role) const {
	if (role == Qt::DisplayRole) {
		Turing::Tape tape = m_tapes->at(index.row());
		return QVariant(tape.CellSymbol(m_leftIndex + index.column()));
	}
	else if (role == Qt::BackgroundRole && m_leftIndex + index.column() == m_headPosition)
		return QColor(192, 192, 192);
	else if (role == Qt::FontRole && m_leftIndex + index.column() == m_headPosition) {
		QFont boldFont;
		boldFont.setBold(true);
		return boldFont;
	}
	else if (role == Qt::TextAlignmentRole)
		return Qt::AlignHCenter + Qt::AlignVCenter;
	return QVariant();
}

void TapesTableModel::startAddRow(int index) {
	beginInsertRows(QModelIndex(), index, index);
}

void TapesTableModel::endAddRow() {
	endInsertRows();
}

void TapesTableModel::startRemoveRow(int index) {
	beginRemoveRows(QModelIndex(), index, index);
}

void TapesTableModel::endRemoveRow() {
	endRemoveRows();
}

Qt::ItemFlags TapesTableModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;
	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool TapesTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	QString newValue = value.toString();
	if (index.isValid() && role == Qt::EditRole && newValue.length() == 1) {
		Turing::Tape *tape = (Turing::Tape*)(&(m_tapes->at(index.row())));
		tape->SetCellSymbol(m_leftIndex + index.column(), newValue.at(0));
		emit dataChanged(index, index);
		return true;
	}
	return false;
}
