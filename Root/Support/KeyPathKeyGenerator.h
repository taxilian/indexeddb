/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_SUPPORT_KEYGENERATORHELPER_H
#define BRANDONHAYNES_INDEXEDDB_SUPPORT_KEYGENERATORHELPER_H

#include <string>
#include <BrowserObjectAPI.h>
#include "../Implementation/KeyGenerator.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 
namespace Support {


///<summary>
/// This class is a key generator that uses the specified key path as a lookup
/// into the passed-in Data instance.  The Data instance must be of type JSOutObject;
/// generation fails otherwise.
///</summary>
class KeyPathKeyGenerator : public Implementation::KeyGenerator
	{
	public:
		KeyPathKeyGenerator(FB::BrowserHost host, const std::string& keyPath)
			: keyPath(keyPath), host(host)
			{ }

		virtual Implementation::Key generateKey(const Implementation::Data& context) const;
		// Generate a key using the key path for the given variant if the variant is of type JSObject (ECMA undefined otherwise)
		const FB::variant generateKey(FB::variant value) const;
		// Generate a key using the key path as a property lookup in the JSObject argument
		const FB::variant generateKey(FB::JSObject object) const;

	private:
		FB::BrowserHost host;
		std::string keyPath;	
	};

}
}
}
}

#endif