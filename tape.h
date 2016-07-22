#include <QMap>
#include <QChar>
#include "command.h"

#ifndef TAPE_H
#define TAPE_H

namespace Turing {

    class Tape {
    public:
        Tape();
        QChar CellSymbol(const int position);
        void SetCellSymbol(const int position, QChar symbol);
        void Erase();

		QMap<int, QChar> data;
    };

}

#endif // TAPE_H
