#include <QMainWindow>
#include <QItemSelection>
#include <QFile>
#include <QTextStream>
#include "command.h"
#include "machine.h"
#include "commandsmodel.h"
#include "tapestablemodel.h"

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;
	CommandsListModel m_commandsListModel;
	TapesTableModel m_tapesTableModel;
	QList<Turing::Command> m_commands;
	Turing::Command *m_editingCommand;
	Turing::Tape *m_selectedTape;
	int m_editingCommandIndex, m_selectedTapeIndex;
	Turing::Machine m_machine;
	uint m_currentCommandIndex;
	QFile currentFile;

	void disableCommandEditor();
	void updateCommandEditor(Turing::Command *command);
	void updateCurrentCommandIndex();
	void readStateFromStream(QTextStream *stream);
	void writeStateToStream(QTextStream *stream);

private slots:
	void handleAddCommandPushButtonClicked();
	void handleRemoveCommandPushButtonClicked();
	void handleCommandsSelectionChanged(const QItemSelection &selection1, const QItemSelection &selection2);
	void handleTapesSelectionChanged(const QItemSelection &selection1, const QItemSelection &selection2);
	void handleStateSpinBoxValueChanged(int i);
	void handleConditionComboboxCurrentIndexChanged(int i);
	void handleConditionLineEditTextEdited(QString string);
	void handleDestinationSpinBoxValueChanged(int i);
	void handleChangeComboboxCurrentIndexChanged(int i);
	void handleChangeLineEditTextEdited(QString string);
	void handleGoLeftRadioButtonClicked(bool checked);
	void handleGoRightRadioButtonClicked(bool checked);
	void handleStayRadioButtonClicked(bool checked);
	void handleAddTapeToolButtonClicked();
	void handleEraseTapeToolButtonClicked();
	void handleRemoveTapeToolButtonClicked();
	void handleCurrentStateSpinBoxValueChanged(int i);
	void handleHeadPositionSpinBoxValueChanged(int i);
	void handleLeftFOVSpinBoxValueChanged(int i);
	void handleRightFOVSpinBoxValueChanged(int i);
	void handleActionExecuteTriggered();
	void handleActionEvaluateTriggered();
	void handleActionStepTriggered();
	void handleActionStepAndEvaluateTriggered();
	void handleActionNewTriggered();
	void handleActionOpenTriggered();
	void handleActionSaveTriggered();
	void handleActionSaveAsTriggered();
};

#endif // MAINWINDOW_H
