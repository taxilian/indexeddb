/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_KEY_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_KEY_H

#include "Data.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	class Key : public Data
		{
		public:
			static const Key getUndefinedKey() { return Key(); }

			Key(void *value, size_t size, Data::ECMAType type) 
				: Data(value, size, type)
				{ }

			Key(const Key& key, bool managedAllocation)
				: Data(key)
				{ }

			Key(const std::string& value)
				: Data(value)
				{ }

			Key(const Data& data)
				: Data(data)
				{ }

		private:
			Key()
				: Data(0, 0, Data::Undefined)
				{ }
		};
	}
}
}

#endif