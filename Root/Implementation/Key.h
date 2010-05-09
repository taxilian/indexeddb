/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_KEY_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_KEY_H

#include "Data.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	///<summary>
	/// This class represents a key in the Indexed Database API implementation.  Key instances
	/// are exchanged between the API layer and implementation layer.
	/// 
	/// Note that there is really little difference between the Key and Data classes; we introduce
	/// the differentiation mostly just to clarify when each are used.
	///</summary>
	class Key : public Data
		{
		public:
			// Helper method to generate an undefined key
			static const Key getUndefinedKey() { return Key(); }

			// Creates a key with the underlying data, given size, and specified type
			Key(void *value, size_t size, Data::ECMAType type) 
				: Data(value, size, type)
				{ }

			// Copy constructor for the Key class
			Key(const Key& key)
				: Data(key)
				{ }

			// Creates a data value containing the specified string
			Key(const std::string& value)
				: Data(value)
				{ }

			// Create a key based off of another Data instance (or Key instance)
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