/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#include "Index.h"

using std::string;
using boost::optional;

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

Index::Index(const string& indexName, const string& objectStoreName, const optional<string>& keyPath, const bool unique)
	: indexName(indexName), objectStoreName(objectStoreName), keyPath(keyPath), unique(unique)
	{ initializeMethods(); }

Index::Index(const string& indexName, const string& objectStoreName)
	: indexName(indexName), objectStoreName(objectStoreName), keyPath(optional<string>()), unique(false)
	{ initializeMethods(); }

void Index::initializeMethods()
	{
	registerProperty("name", make_property(this, &Index::getName));
	registerProperty("storeName", make_property(this, &Index::getObjectStoreName));
	registerProperty("keyPath", make_property(this, &Index::getKeyPath));
	registerProperty("unique", make_property(this, &Index::getUnique));
	}

}
}
}