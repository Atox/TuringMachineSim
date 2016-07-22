#include <QAbstractTableModel>
#include <QList>
#include <tape.h>

#ifndef TAPESTABLEMODEL_H
#define TAPESTABLEMODEL_H

class TapesTableModel : public QAbstractTableModel {
	Q_OBJECT

public:
	explicit TapesTableModel(QObject *parent = 0);
	void connect(QList<Turing::Tape> *tapes);
	void setHeadPosition(int headPosition);
	void setFOV(int leftIndex, int rightIndex);
	void clear();
	int headPosition();
	int leftIndex();
	int rightIndex();
	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	QVariant data(const QModelIndex &index, int role) const;
	void startAddRow(int index);
	void endAddRow();
	void startRemoveRow(int index);
	void endRemoveRow();
	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData(const QModelIndex &index,const QVariant &value, int role);

private:
	QList<Turing::Tape> *m_tapes;
	int m_headPosition, m_leftIndex, m_rightIndex;
};

#endif // TAPESTABLEMODEL_H
