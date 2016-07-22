#include "command.h"

namespace Turing {

	Command::Command() {
		state = 0;
		conditions.append(QChar('a'));
		destination = -1;
		changes.append(QChar('a'));
		direction = 0;
	}

	Command::Command(int tapes) {
		state = 0;
		for (int i = 0; i < tapes; i++) {
			conditions.append(QChar('a'));
			changes.append(QChar('a'));
		}
		destination = -1;
		direction = 0;
	}

	Command::Command(int state, int destination, char direction, QList<QChar> conditions, QList<QChar> changes) {
		this->state = state;
		this->conditions = conditions;
		this->destination = destination;
		this->changes = changes;
		if (direction >= -1 && direction <= 1)
			this->direction = direction;
		else
			this->direction = 0;
	}

	QString Command::asString() {
		QString str = "Q";
		str += QString::number(state) + ',';
		for (int i = 0; i < conditions.count() - 1; i++)
			str += conditions.at(i) + '/';
		if (conditions.count() > 0)
			str += conditions.at(conditions.count() - 1);
		str += " → Q";
		if (destination == -1)
			str += 'z';
		else
			str += QString::number(destination);
		str += ',';
		for (int i = 0; i < changes.count() - 1; i++)
			str += changes.at(i) + '/';
		if (changes.count() > 0)
			str += changes.at(changes.count() - 1);
		str += ',';
		if (direction == 0)
			str += 'S';
		else if (direction > 0)
			str += 'R';
		else
			str += 'L';
		return str;
	}

}
