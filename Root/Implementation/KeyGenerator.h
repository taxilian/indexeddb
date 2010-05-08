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

	class KeyGenerator
		{
		public:
			virtual Key generateKey(const Data& context) const = 0;
		};
	}
}
}

#endif