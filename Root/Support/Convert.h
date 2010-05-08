/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_SUPPORT_CONVERT_H
#define BRANDONHAYNES_INDEXEDDB_SUPPORT_CONVERT_H

#include <JSAPIAuto.h>
#include "../Implementation/Key.h"
#include "../Implementation/Data.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

class Convert
{
public:
	static Implementation::Data toData(FB::BrowserHost host, const FB::variant& variant);
	static Implementation::Key toKey(FB::BrowserHost host, const FB::variant& variant);
	static FB::variant toVariant(FB::BrowserHost host, const Implementation::Data& data);
	static Implementation::Data::ECMAType getType(FB::variant variant);

	static std::string stringify(FB::BrowserHost host, const FB::JSObject& object);
	static FB::JSObject parse(FB::BrowserHost host, const std::string& string);

private:
	Convert() { }

	template<class T>
	static T convert(FB::BrowserHost host, const FB::variant& variant);
};

}
}
}

#endif