/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
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

class KeyGeneratorHelper : public Implementation::KeyGenerator
	{
	public:
		KeyGeneratorHelper(FB::BrowserHost host, const std::string& keyPath)
			: keyPath(keyPath), host(host)
			{ }

		virtual Implementation::Key generateKey(const Implementation::Data& context) const;
		const FB::variant generateKey(FB::variant value) const;

	private:
		FB::BrowserHost host;
		std::string keyPath;	
	};

}
}
}
}

#endif