/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include "KeyGeneratorHelper.h"
#include "../Implementation/Key.h"
#include "Convert.h"

namespace BrandonHaynes {
namespace IndexedDB { 

	using class Implementation::Key;
	using class Implementation::Data;
	
namespace API { 
namespace Support {

Key KeyGeneratorHelper::generateKey(const Data& context) const
	// TODO Does the spec support a keypath that looks like path1.path2?
	{ return Convert::toKey(host, generateKey(Convert::toVariant(host, context))); }

const FB::variant KeyGeneratorHelper::generateKey(FB::variant value) const
	{
	if(!value.can_be_type<FB::JSObjectPtr>())
		return FB::variant();
	else 
		return value.convert_cast<FB::JSObjectPtr>()->GetProperty(keyPath);
	}

}
}
}
}