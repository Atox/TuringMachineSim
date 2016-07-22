#include <QList>
#include "tape.h"
#include "command.h"

#ifndef MACHINE_H
#define MACHINE_H

namespace Turing {

	class Machine {
	public:
		Machine();
		void reset(int numTapes);
		void execute(QList<Command> *commands, int commandsToExecute = 0);
		void addTape();
		bool removeTapeAt(int index);
		QList<Tape> *tapes();
		int headPosition();
		bool setHeadPosition(int headPosition);
		int state();
		void setState(int state);

	private:
		QList<Tape> m_tapes;
		int m_headPosition, m_state;
	};

}

#endif // MACHINE_H
