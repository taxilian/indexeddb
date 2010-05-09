/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_CURSOR_H
#define BRANDONHAYNES_INDEXEDDB_API_CURSOR_H

#include <JSAPIAuto.h>

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

///<summary>
/// This class represents the Cursor interface in the Indexed Database API.
///</summary>
class Cursor : public FB::JSAPIAuto
{
public:
	// The direction of a cursor
	enum Direction { NEXT = 0, NEXT_NO_DUPLICATE = 1, PREV = 2, PREV_NO_DUPLICATE = 3 };

	// Returns a stub instance of this interface so clients may access the direction enum
	static const FB::AutoPtr<Cursor> getInstance()
		{ return FB::AutoPtr<Cursor>(new Cursor()); }

	// Gets the direction associated with this cursor
	Cursor::Direction getDirection() { return direction; }

protected:
	// Creates a cursor with the given direction
	Cursor(const Cursor::Direction direction);
	// If we make this fully an interface, kill this constructor
	Cursor();

	// Private helper methods used to grant clients access to the direction constants via FireBreath
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