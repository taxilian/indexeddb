/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_KEYRANGE_H
#define BRANDONHAYNES_INDEXEDDB_API_KEYRANGE_H

#include <JSAPIAuto.h>
#include <APITypes.h>
#include "../../IndexedDatabasePluginAPI.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

///<summary>
/// This class represents a range of keys as defined by the Indexed Database API.
/// Key ranges have a left and right bound along with a set of associated flags
///</summary>
class KeyRange : public FB::JSAPIAuto
	{
	public:
		KeyRange(FB::variant left, FB::variant right, int flags);

		enum IntervalType { SINGLE = 0, LEFT_OPEN = 1, RIGHT_OPEN = 2, LEFT_BOUND = 4, RIGHT_BOUND = 8 };

		FB::variant getLeft() const { return left; }
		FB::variant getRight() const { return right; }
		int getFlags() const { return flags; }

		FB::JSOutObject only(FB::variant value);
		FB::JSOutObject leftBound(FB::variant bound, const FB::CatchAll& args);
		FB::JSOutObject rightBound(FB::variant bound, const FB::CatchAll& args);
		FB::JSOutObject bound(FB::variant left, FB::variant right, const FB::CatchAll& args);

		FB::AutoPtr<KeyRange> bound(FB::variant left, FB::variant right, const bool openLeft, const bool openRight);
		FB::AutoPtr<KeyRange> leftBound(FB::variant bound, const bool open);
		FB::AutoPtr<KeyRange> rightBound(FB::variant bound, const bool open);

	private:
		KeyRange(void);

		FB::variant left;
		FB::variant right;
		const unsigned short flags;

		void initializeMethods();
		int getSingle() const { return SINGLE; }
		int GetLeftOpen() const { return LEFT_OPEN; }
		int GetRightOpen() const { return RIGHT_OPEN; }
		int GetLeftBound() const { return LEFT_BOUND; }
		int GetRightBound() const { return RIGHT_BOUND; }

		friend FB::JSOutObject IndexedDatabasePluginAPI::getKeyRange();
	};

}
}
}

#endif