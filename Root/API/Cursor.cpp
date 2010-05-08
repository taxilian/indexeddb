/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include "Cursor.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

Cursor::Cursor(void)
	: direction(NEXT)
	{ initializeMethods(); }

Cursor::Cursor(const Cursor::Direction direction)
	: direction(direction)
	{ initializeMethods(); }

void Cursor::initializeMethods()
	{
	registerProperty("direction", make_property(this, &Cursor::getDirection));

	registerProperty("NEXT", make_property(this, &Cursor::getNext));
	registerProperty("NEXT_NO_DUPLICATE", make_property(this, &Cursor::getNextNoDuplicate));
	registerProperty("PREV", make_property(this, &Cursor::getPrevious));
	registerProperty("PREV_NO_DUPLICATE", make_property(this, &Cursor::getPreviousNoDuplicate));
	}

}
}
}