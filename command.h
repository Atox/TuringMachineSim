#include <QList>
#include <QString>

#ifndef COMMAND_H
#define COMMAND_H

namespace Turing {

	class Command {
	public:
		Command();
		Command(int tapes);
		Command(int state, int destination, char direction, QList<QChar> conditions, QList<QChar> changes);
		int state, destination;
		QList<QChar> conditions, changes;
		char direction;
		QString asString();
	};

}

#endif // COMMAND_H
