#include "tape.h"

namespace Turing
{

	Tape::Tape() { }

	QChar Tape::CellSymbol(const int position) {
		if (data.contains(position))
			return data[position];
		else
			return QChar('a');
	}

	void Tape::SetCellSymbol(const int position, QChar symbol) {
		if (symbol == QChar('a'))
			data.remove(position);
		else
			data[position] = symbol;
	}

	void Tape::Erase() {
		data.clear();
	}

}
