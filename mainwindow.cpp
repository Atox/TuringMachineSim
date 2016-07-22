#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), m_commandsListModel(0), m_tapesTableModel(0) {
	ui->setupUi(this);
	m_commandsListModel.connect(&m_commands, &m_currentCommandIndex);
	m_tapesTableModel.connect(m_machine.tapes());
	ui->commandsListView->setModel(&m_commandsListModel);
	ui->tapesTableView->setModel(&m_tapesTableModel);
	m_currentCommandIndex = -1;
	m_editingCommand = 0;
	m_selectedTapeIndex = -1;
	connect(ui->commandsListView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(handleCommandsSelectionChanged(QItemSelection, QItemSelection)));
	connect(ui->tapesTableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(handleTapesSelectionChanged(QItemSelection, QItemSelection)));
	connect(ui->addCommandToolButton, SIGNAL(clicked()), this, SLOT(handleAddCommandPushButtonClicked()));
	connect(ui->removeCommandToolButton, SIGNAL(clicked()), this, SLOT(handleRemoveCommandPushButtonClicked()));
	connect(ui->stateSpinBox, SIGNAL(valueChanged(int)), this, SLOT(handleStateSpinBoxValueChanged(int)));
	connect(ui->conditionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleConditionComboboxCurrentIndexChanged(int)));
	connect(ui->conditionLineEdit, SIGNAL(textEdited(QString)), this, SLOT(handleConditionLineEditTextEdited(QString)));
	connect(ui->destinationSpinBox, SIGNAL(valueChanged(int)), this, SLOT(handleDestinationSpinBoxValueChanged(int)));
	connect(ui->changeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleChangeComboboxCurrentIndexChanged(int)));
	connect(ui->changeLineEdit, SIGNAL(textEdited(QString)), this, SLOT(handleChangeLineEditTextEdited(QString)));
	connect(ui->goLeftRadioButton, SIGNAL(clicked(bool)), this, SLOT(handleGoLeftRadioButtonClicked(bool)));
	connect(ui->goRightRadioButton, SIGNAL(clicked(bool)), this, SLOT(handleGoRightRadioButtonClicked(bool)));
	connect(ui->stayRadioButton, SIGNAL(clicked(bool)), this, SLOT(handleStayRadioButtonClicked(bool)));
	connect(ui->addTapeToolButton, SIGNAL(clicked()), this, SLOT(handleAddTapeToolButtonClicked()));
	connect(ui->eraseTapeToolButton, SIGNAL(clicked()), this, SLOT(handleEraseTapeToolButtonClicked()));
	connect(ui->removeTapeToolButton, SIGNAL(clicked()), this, SLOT(handleRemoveTapeToolButtonClicked()));
	connect(ui->currentStateSpinBox, SIGNAL(valueChanged(int)), this, SLOT(handleCurrentStateSpinBoxValueChanged(int)));
	connect(ui->headPositionSpinBox, SIGNAL(valueChanged(int)), this, SLOT(handleHeadPositionSpinBoxValueChanged(int)));
	connect(ui->leftFOVSpinBox, SIGNAL(valueChanged(int)), this, SLOT(handleLeftFOVSpinBoxValueChanged(int)));
	connect(ui->rightFOVSpinBox,SIGNAL(valueChanged(int)), this, SLOT(handleRightFOVSpinBoxValueChanged(int)));
	connect(ui->actionExecute, SIGNAL(triggered()), this, SLOT(handleActionExecuteTriggered()));
	connect(ui->actionEvaluate, SIGNAL(triggered()), this, SLOT(handleActionEvaluateTriggered()));
	connect(ui->actionStep, SIGNAL(triggered()), this, SLOT(handleActionStepTriggered()));
	connect(ui->actionStepAndEvaluate, SIGNAL(triggered()), this, SLOT(handleActionStepAndEvaluateTriggered()));
	connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(handleActionNewTriggered()));
	connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(handleActionOpenTriggered()));
	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(handleActionSaveTriggered()));
	connect(ui->actionSave_as, SIGNAL(triggered()), this, SLOT(handleActionSaveAsTriggered()));
	for (int i = 0; i < m_tapesTableModel.columnCount(QModelIndex()); i++)
		ui->tapesTableView->setColumnWidth(i, 32);
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::disableCommandEditor() {
	m_editingCommand = 0;
	ui->removeCommandToolButton->setEnabled(false);
	ui->stateSpinBox->setValue(0);
	ui->stateSpinBox->setEnabled(false);
	ui->conditionComboBox->clear();
	ui->conditionComboBox->setEnabled(false);
	ui->conditionLineEdit->setText(QString());
	ui->conditionLineEdit->setEnabled(false);
	ui->destinationSpinBox->setValue(0);
	ui->destinationSpinBox->setEnabled(false);
	ui->changeComboBox->clear();
	ui->changeComboBox->setEnabled(false);
	ui->changeLineEdit->setText(QString());
	ui->changeLineEdit->setEnabled(false);
	ui->goLeftRadioButton->setEnabled(false);
	ui->goRightRadioButton->setEnabled(false);
	ui->stayRadioButton->setEnabled(false);
	ui->stayRadioButton->setChecked(true);
}

void MainWindow::updateCommandEditor(Turing::Command *command) {
	if (!command)
		return;
	m_editingCommand = command;
	ui->removeCommandToolButton->setEnabled(true);
	ui->stateSpinBox->setEnabled(true);
	ui->stateSpinBox->setValue(command->state);
	ui->conditionComboBox->clear();
	ui->conditionComboBox->setEnabled(true);
	for (int i = 0; i < command->conditions.count(); i++)
		ui->conditionComboBox->addItem(QString::number(i + 1));
	ui->conditionLineEdit->setEnabled(true);
	ui->conditionLineEdit->setText(command->conditions[0]);
	ui->destinationSpinBox->setEnabled(true);
	ui->destinationSpinBox->setValue(command->destination);
	ui->changeComboBox->clear();
	ui->changeComboBox->setEnabled(true);
	for (int i = 0; i < command->changes.count(); i++)
		ui->changeComboBox->addItem(QString::number(i + 1));
	ui->changeLineEdit->setEnabled(true);
	ui->changeLineEdit->setText(command->changes[0]);
	ui->goLeftRadioButton->setEnabled(true);
	ui->goRightRadioButton->setEnabled(true);
	ui->stayRadioButton->setEnabled(true);
	if (command->direction == 0)
		ui->stayRadioButton->setChecked(true);
	else if (command->direction > 0)
		ui->goRightRadioButton->setChecked(true);
	else
		ui->goLeftRadioButton->setChecked(true);
}

void MainWindow::updateCurrentCommandIndex() {
	m_currentCommandIndex = -1;
	for (int i = 0; i < m_commands.count(); i++)
		if (m_commands.at(i).state == m_machine.state()) {
			int valid = 0;
			for (int j = 0; j < m_machine.tapes()->count(); j++) {
				Turing::Command *command = (Turing::Command*)(&(m_commands.at(i)));
				Turing::Tape *tape = (Turing::Tape*)(&(m_machine.tapes()->at(j)));
				if (command->conditions.count() > j && command->conditions[j] == tape->CellSymbol(m_machine.headPosition()))
					valid++;
			}
			if (valid == m_machine.tapes()->count()) {
				m_currentCommandIndex = i;
				break;
			}
		}
}

void MainWindow::handleAddCommandPushButtonClicked() {
	m_commands.append(Turing::Command(m_machine.tapes()->count()));
	emit m_commandsListModel.dataChanged(QModelIndex(), QModelIndex());
	if (ui->commandsListView->currentIndex().row() >= 0 && m_commands.count() > 0 && ui->commandsListView->currentIndex().row() < m_commands.count())
		updateCommandEditor(&m_commands[ui->commandsListView->currentIndex().row()]);
}

void MainWindow::handleRemoveCommandPushButtonClicked() {
	int index = ui->commandsListView->currentIndex().row();
	if (index >= 0 && index < m_commands.count()) {
		m_commands.removeAt(index);
		if (index < (int)m_currentCommandIndex)
			m_currentCommandIndex--;
		else if (index == (int)m_currentCommandIndex)
			m_currentCommandIndex = -1;
		emit m_commandsListModel.dataChanged(QModelIndex(), QModelIndex());
	}
	index = ui->commandsListView->currentIndex().row();
	if (index < 0 || index >= m_commands.count())
		disableCommandEditor();
	else if (index >= 0 && index < m_commands.count())
		updateCommandEditor(&m_commands[index]);
}

void MainWindow::handleCommandsSelectionChanged(const QItemSelection &selection1, const QItemSelection &) {
	if (selection1.indexes().count() == 1 && m_commands.count() > selection1.indexes()[0].row())
		updateCommandEditor(&m_commands[selection1.indexes()[0].row()]);
	else
		disableCommandEditor();
}

void MainWindow::handleTapesSelectionChanged(const QItemSelection &selection1, const QItemSelection &) {
	if (selection1.indexes().count() == 1) {
		ui->eraseTapeToolButton->setEnabled(true);
		ui->removeTapeToolButton->setEnabled(true);
		if (m_machine.tapes()->count() > selection1.indexes().at(0).row()) {
			m_selectedTape = (Turing::Tape*)(&(m_machine.tapes()->at(selection1.indexes().at(0).row())));
			m_selectedTapeIndex = selection1.indexes().at(0).row();
		}
		else {
			m_selectedTape = 0;
			m_selectedTapeIndex = -1;
		}
	}
	else {
		ui->eraseTapeToolButton->setEnabled(false);
		ui->removeTapeToolButton->setEnabled(false);
	}
}

void MainWindow::handleStateSpinBoxValueChanged(int i) {
	if (m_editingCommand) {
		m_editingCommand->state = i;
		emit m_commandsListModel.dataChanged(QModelIndex(), QModelIndex());
	}
}

void MainWindow::handleConditionComboboxCurrentIndexChanged(int i) {
	if (m_editingCommand && 0 <= i && i < m_editingCommand->conditions.count()) {
		ui->conditionLineEdit->setText(m_editingCommand->conditions[i]);
		m_editingCommandIndex = i;
	}
}

void MainWindow::handleConditionLineEditTextEdited(QString string) {
	if (m_editingCommand && string.length() == 1) {
		m_editingCommand->conditions[m_editingCommandIndex] = string.at(0);
		emit m_commandsListModel.dataChanged(QModelIndex(), QModelIndex());
	}
}

void MainWindow::handleDestinationSpinBoxValueChanged(int i) {
	if (m_editingCommand) {
		m_editingCommand->destination = i;
		emit m_commandsListModel.dataChanged(QModelIndex(), QModelIndex());
	}
}

void MainWindow::handleChangeComboboxCurrentIndexChanged(int i) {
	if (m_editingCommand && 0 <= i && i < m_editingCommand->changes.count()) {
		ui->changeLineEdit->setText(m_editingCommand->changes[i]);
		m_editingCommandIndex = i;
	}
}

void MainWindow::handleChangeLineEditTextEdited(QString string) {
	if (m_editingCommand && string.length() == 1) {
		m_editingCommand->changes[m_editingCommandIndex] = string.at(0);
		emit m_commandsListModel.dataChanged(QModelIndex(), QModelIndex());
	}
}

void MainWindow::handleGoLeftRadioButtonClicked(bool checked) {
	if (checked && m_editingCommand) {
		m_editingCommand->direction = -1;
		emit m_commandsListModel.dataChanged(QModelIndex(), QModelIndex());
	}
}

void MainWindow::handleGoRightRadioButtonClicked(bool checked) {
	if (checked && m_editingCommand) {
		m_editingCommand->direction = 1;
		emit m_commandsListModel.dataChanged(QModelIndex(), QModelIndex());
	}
}

void MainWindow::handleStayRadioButtonClicked(bool checked) {
	if (checked && m_editingCommand) {
		m_editingCommand->direction = 0;
		emit m_commandsListModel.dataChanged(QModelIndex(), QModelIndex());
	}
}

void MainWindow::handleAddTapeToolButtonClicked() {
	m_tapesTableModel.startAddRow(m_machine.tapes()->count());
	m_machine.addTape();
	m_tapesTableModel.endAddRow();
	for (int i = 0; i < m_commands.length(); i++) {
		Turing::Command *command = (Turing::Command*)(&m_commands.at(i));
		command->conditions.append(QChar('a'));
		command->changes.append(QChar('a'));
	}
	if (m_editingCommand)
		updateCommandEditor(m_editingCommand);
	emit m_commandsListModel.dataChanged(QModelIndex(), QModelIndex());
	emit m_tapesTableModel.dataChanged(QModelIndex(), QModelIndex());
}

void MainWindow::handleEraseTapeToolButtonClicked() {
	if (m_selectedTape) {
		m_selectedTape->Erase();
		emit m_tapesTableModel.dataChanged(QModelIndex(), QModelIndex());
	}
}

void MainWindow::handleRemoveTapeToolButtonClicked() {
	if (m_selectedTapeIndex != -1 && m_selectedTape) {
		int selectedTapeIndex = m_selectedTapeIndex;
		m_tapesTableModel.startRemoveRow(selectedTapeIndex);
		m_machine.removeTapeAt(selectedTapeIndex);
		m_tapesTableModel.endRemoveRow();
		if (selectedTapeIndex >= m_machine.tapes()->count())
			selectedTapeIndex--;
		else
			m_selectedTapeIndex = selectedTapeIndex;
		for (int i = 0; i < m_commands.count(); i++) {
			Turing::Command *command = (Turing::Command*)(&(m_commands.at(i)));
			command->conditions.removeAt(selectedTapeIndex);
			command->changes.removeAt(selectedTapeIndex);
		}
		updateCommandEditor(m_editingCommand);
	}
	emit m_commandsListModel.dataChanged(QModelIndex(), QModelIndex());
	emit m_tapesTableModel.dataChanged(QModelIndex(), QModelIndex());
}

void MainWindow::handleCurrentStateSpinBoxValueChanged(int i) {
	m_machine.setState(i);
}

void MainWindow::handleHeadPositionSpinBoxValueChanged(int i) {
	if (!m_machine.setHeadPosition(i))
		ui->headPositionSpinBox->setValue(m_machine.headPosition());
	else
		m_tapesTableModel.setHeadPosition(i);
}

void MainWindow::handleLeftFOVSpinBoxValueChanged(int i) {
	if (i <= ui->rightFOVSpinBox->value()) {
		m_tapesTableModel.setFOV(i, ui->rightFOVSpinBox->value());
		for (int i = 0; i < m_tapesTableModel.columnCount(QModelIndex()); i++)
			ui->tapesTableView->setColumnWidth(i, 32);
	}
	else
		ui->leftFOVSpinBox->setValue(ui->rightFOVSpinBox->value());
}

void MainWindow::handleRightFOVSpinBoxValueChanged(int i) {
	if (i >= ui->leftFOVSpinBox->value()) {
		m_tapesTableModel.setFOV(ui->leftFOVSpinBox->value(), i);
		for (int i = 0; i < m_tapesTableModel.columnCount(QModelIndex()); i++)
			ui->tapesTableView->setColumnWidth(i, 32);
	}
	else
		ui->rightFOVSpinBox->setValue(ui->leftFOVSpinBox->value());
}

void MainWindow::handleActionExecuteTriggered() {
	m_machine.execute(&m_commands);
	m_tapesTableModel.setHeadPosition(m_machine.headPosition());
	m_currentCommandIndex = -1;
	ui->currentStateSpinBox->setValue(m_machine.state());
	ui->headPositionSpinBox->setValue(m_machine.headPosition());
	emit m_commandsListModel.dataChanged(QModelIndex(), QModelIndex());
}

void MainWindow::handleActionEvaluateTriggered() {
	updateCurrentCommandIndex();
	emit m_commandsListModel.dataChanged(QModelIndex(), QModelIndex());
}

void MainWindow::handleActionStepTriggered() {
	m_machine.execute(&m_commands, 1);
	m_tapesTableModel.setHeadPosition(m_machine.headPosition());
	m_currentCommandIndex = -1;
	ui->currentStateSpinBox->setValue(m_machine.state());
	ui->headPositionSpinBox->setValue(m_machine.headPosition());
	emit m_commandsListModel.dataChanged(QModelIndex(), QModelIndex());
}

void MainWindow::handleActionStepAndEvaluateTriggered() {
	handleActionStepTriggered();
	handleActionEvaluateTriggered();
}

void MainWindow::handleActionNewTriggered() {
	m_tapesTableModel.clear();
	m_commands.clear();
	disableCommandEditor();
	m_machine.tapes()->clear();
	handleAddTapeToolButtonClicked();
	ui->currentStateSpinBox->setValue(0);
	handleCurrentStateSpinBoxValueChanged(0);
	ui->leftFOVSpinBox->setValue(0);
	handleLeftFOVSpinBoxValueChanged(0);
	ui->rightFOVSpinBox->setValue(15);
	handleRightFOVSpinBoxValueChanged(15);
	ui->headPositionSpinBox->setValue(0);
	handleHeadPositionSpinBoxValueChanged(0);
	emit m_commandsListModel.dataChanged(QModelIndex(), QModelIndex());
	emit m_tapesTableModel.dataChanged(QModelIndex(), QModelIndex());
}

void MainWindow::readStateFromStream(QTextStream *stream) {
	int state, headPosition, leftIndex, rightIndex;
	*stream >> state >> headPosition >> leftIndex >> rightIndex;
	ui->currentStateSpinBox->setValue(state);
	handleCurrentStateSpinBoxValueChanged(state);
	ui->headPositionSpinBox->setValue(headPosition);
	handleHeadPositionSpinBoxValueChanged(headPosition);
	ui->leftFOVSpinBox->setValue(leftIndex);
	handleLeftFOVSpinBoxValueChanged(leftIndex);
	ui->rightFOVSpinBox->setValue(rightIndex);
	handleRightFOVSpinBoxValueChanged(rightIndex);
	m_tapesTableModel.clear();
	m_commands.clear();
	disableCommandEditor();
	m_machine.tapes()->clear();
	int tapesCount, symbolsCount, symbolPosition;
	QString symbol;
	*stream >> tapesCount;
	for (int i = 0; i < tapesCount; i++) {
		m_tapesTableModel.startAddRow(m_machine.tapes()->count());
		m_machine.addTape();
		m_tapesTableModel.endAddRow();
		*stream >> symbolsCount;
		for (int j = 0; j < symbolsCount; j++) {
			*stream >> symbolPosition;
			*stream >> symbol;
			m_machine.tapes()->last().SetCellSymbol(symbolPosition, symbol.at(0));
		}
	}
	int commandsCount, destination, direction;
	QString condition, change;
	*stream >> commandsCount;
	for (int i = 0; i < commandsCount; i++) {
		m_commands.append(Turing::Command(0));
		*stream >> state >> destination;
		*stream >> direction;
		m_commands.last().state = state;
		m_commands.last().destination = destination;
		m_commands.last().direction = direction;
		for (int j = 0; j < m_machine.tapes()->count(); j++) {
			*stream >> condition;
			*stream >> change;
			m_commands.last().conditions.append(condition.at(0));
			m_commands.last().changes.append(change.at(0));
		}
	}
	int index = ui->commandsListView->currentIndex().row();
	if (index < 0 || index >= m_commands.count())
		disableCommandEditor();
	else if (index >= 0 && index < m_commands.count())
		updateCommandEditor(&m_commands[index]);
	emit m_commandsListModel.dataChanged(QModelIndex(), QModelIndex());
	emit m_tapesTableModel.dataChanged(QModelIndex(), QModelIndex());

}

void MainWindow::writeStateToStream(QTextStream *stream) {
	*stream << m_machine.state() << ' ' << m_tapesTableModel.headPosition() << ' ' << m_tapesTableModel.leftIndex() << ' ' << m_tapesTableModel.rightIndex() << '\n';
	*stream << m_machine.tapes()->size() << '\n';
	for (int i = 0; i < m_machine.tapes()->size(); i++) {
		const QMap<int, QChar> *data = &(m_machine.tapes()->at(i).data);
		*stream << data->keys().size() << '\n';
		for (int j = 0; j < data->keys().size(); j++) {
			if (j > 0)
				*stream << ' ';
			*stream << data->keys().at(j) << ' ' << data->value(data->keys().at(j));
		}
		if (data->keys().size() > 0)
			*stream << '\n';
	}
	*stream << m_commands.size() << '\n';
	// Command
	for (int i = 0; i < m_commands.size(); i++) {
		*stream << m_commands.at(i).state << ' ' << m_commands.at(i).destination << ' ' << (int)m_commands.at(i).direction << '\n';
		for (int j = 0; j < m_commands.at(i).conditions.size(); j++) {
			*stream << m_commands.at(i).conditions.at(j) << ' ' << m_commands.at(i).changes.at(j) << '\n';
		}
	}
}

void MainWindow::handleActionOpenTriggered() {
	QString filePath = QFileDialog::getOpenFileName(this, QString("Open file"), QString(), QString("Turing machine state (*.dat)"));
	QFile f(filePath);
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
		if (!filePath.isEmpty()) {
			QMessageBox msgBox;
			msgBox.critical(0, QString("Error!"), QString("Can't open that file!"));
		}
	}
	else {
		currentFile.setFileName(filePath);
		QTextStream stream(&f);
		readStateFromStream(&stream);
		f.close();
	}
}

void MainWindow::handleActionSaveTriggered() {
	if (currentFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream stream(&currentFile);
		writeStateToStream(&stream);
		currentFile.close();
	}
	else
		handleActionSaveAsTriggered();
}

void MainWindow::handleActionSaveAsTriggered() {
	QString filePath = QFileDialog::getSaveFileName(this, QString("Save file"), QString(), QString("Turing machine state (*.dat)"));
	QFile f(filePath);
	if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
		if (!filePath.isEmpty()) {
			QMessageBox msgBox;
			msgBox.critical(0, QString("Error!"), QString("Can't create the file!"));
		}
	}
	else {
		currentFile.setFileName(filePath);
		QTextStream stream(&f);
		writeStateToStream(&stream);
		f.close();
	}
}
