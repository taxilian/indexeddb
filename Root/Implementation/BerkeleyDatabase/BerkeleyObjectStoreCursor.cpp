#include "BerkeleyObjectStoreCursor.h"
#include "BerkeleyObjectStore.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
namespace BerkeleyDB
	{
	BerkeleyObjectStoreCursor::BerkeleyObjectStoreCursor(BerkeleyObjectStore& objectStore, const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates, TransactionContext& transactionContext)
		: BerkeleyCursor(objectStore.getImplementation(), left, right, openLeft, openRight, isReversed, omitDuplicates, transactionContext)
		{ }
	}
}
}
}