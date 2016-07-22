#include "machine.h"

namespace Turing
{

	Machine::Machine() {
		m_headPosition = m_state = 0;
		m_tapes.push_back(Tape());
	}

	void Machine::reset(int numTapes) {
		m_headPosition = m_state = 0;
		while (m_tapes.count() < numTapes)
			m_tapes.push_back(Tape());
		while (m_tapes.count() > numTapes)
			m_tapes.pop_back();
		for (int i = 0; i < m_tapes.count(); i++)
			m_tapes[i].Erase();
	}

	void Machine::execute(QList<Command> *commands, int commandsToExecute) {
		bool executed, executeAllCommands = commandsToExecute == 0;
		do {
			executed = false;
			if (executeAllCommands || commandsToExecute > 0)
				for (int i = 0; i < commands->count(); i++) {
					Command command = (*commands)[i];
					if (command.state == m_state) {
						int valid = 0;
						for (int j = 0; j < command.conditions.count(); j++)
							if (m_tapes.count() >= j + 1 && command.conditions.count() >= j + 1 && m_tapes[j].CellSymbol(m_headPosition) == command.conditions[j])
								valid++;
						if (valid == m_tapes.count()) {
							for (int j = 0; j < command.conditions.count(); j++)
								m_tapes[j].SetCellSymbol(m_headPosition, command.changes[j]);
							if (!executeAllCommands)
								commandsToExecute--;
							executed = true;
							if (command.direction > 0)
								m_headPosition++;
							else if (command.direction < 0)
								m_headPosition--;
							m_state = command.destination;
						}
					}

				}
		} while (executed);
	}

	void Machine::addTape() {
		m_tapes.push_back(Tape());
	}

	bool Machine::removeTapeAt(int index) {
		if (index < 0 || index >= m_tapes.count())
			return false;
		else
		{
			m_tapes.removeAt(index);
			return true;
		}
	}

	QList<Tape>* Machine::tapes() {
		return &m_tapes;
	}

	int Machine::headPosition() {
		return m_headPosition;
	}

	bool Machine::setHeadPosition(int headPosition) {
		if (headPosition >= -999999 && headPosition <= 999999) {
			m_headPosition = headPosition;
			return true;
		}
		else
			return false;
	}

	int Machine::state() {
		return m_state;
	}

	void Machine::setState(int state){
		m_state = state;
	}

}
