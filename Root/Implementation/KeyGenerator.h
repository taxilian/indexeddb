/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_KEYGENERATOR_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_KEYGENERATOR_H

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	class Key;
	class Data;

	///<summary>
	/// This interface represents a key generator for an Indexed Database API index.
	/// When a new secondary key is needed for the index, the index delegates this task to
	/// an instance of a class that implements this interface.
	///</summary>
	class KeyGenerator
		{
		public:
			// Given a primary data value, generate a secondary key
			virtual Key generateKey(const Data& context) const = 0;
		};
	}
}
}

#endif