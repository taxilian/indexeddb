/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_CURSOR_H
#define BRANDONHAYNES_INDEXEDDB_API_CURSOR_H

#include <JSAPIAuto.h>

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

class Cursor : public FB::JSAPIAuto
{
public:
	enum Direction { NEXT = 0, NEXT_NO_DUPLICATE = 1, PREV = 2, PREV_NO_DUPLICATE = 3 };

	static const FB::AutoPtr<Cursor> getInstance()
		{ return FB::AutoPtr<Cursor>(new Cursor()); }

	const Cursor::Direction getDirection() { return direction; }

protected:
	Cursor();
	Cursor(const Cursor::Direction direction);

	int getNext() const { return NEXT; }
	int getNextNoDuplicate() const { return NEXT_NO_DUPLICATE; }
	int getPrevious() const { return PREV; }
	int getPreviousNoDuplicate() const { return PREV_NO_DUPLICATE; }

private:
	const Cursor::Direction direction;

	void initializeMethods();
};

}
}
}

#endif