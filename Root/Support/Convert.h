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

	///<summary>
	/// This utility class is used to perform conversions to and from FireBreath variants and 
	/// implementation-layer Data and Key classes.  It also provides helper methods to convert
	/// to and from FireBreath JSOutObjects (via JSON encoding).  
	///
	/// Our JSON use is off-spec here; we should be using structured cloning, but I'm lazy
	/// and want to wait for browsers to do it for me.  No one is right now, so this is 
	/// a great way out until then.  (NB: I believe we have a JavaScript security issue
	/// with IE; I'm getting a property-not-found when I try to utilize it.  Using
	/// an external script until I get things sorted out.)
	///</summary>
	class Convert
	{
	public:
		/// Convert a given variant into an implementation Data instance.  Note that we require the host
		/// value in order to perform JSON encoding, in case we're handed an object.
		static Implementation::Data toData(FB::BrowserHost host, const FB::variant& variant);
		/// Convert a given variant into an implementation Key instance.  Note that we require the host
		/// value in order to perform JSON parsing, in case we're handed an object.
		static Implementation::Key toKey(FB::BrowserHost host, const FB::variant& variant);
		/// Convert a given Data instance into an implementation FireBreath variant.  Note that we require the host
		/// value in order to perform JSON stringification, in case we're handed an object.
		static FB::variant toVariant(FB::BrowserHost host, const Implementation::Data& data);
		/// Given a variant, returns the ECMAType associated with that value (in a form digestiable by the implementation)
		static Implementation::Data::ECMAType getType(FB::variant variant);

		/// Stringifies the given object (requires a host instance to access the browser JSON implementation)
		static std::string stringify(FB::BrowserHost host, const FB::JSObject& object);
		/// Parses a given string into a FireBreath JSOutObject instance (requires a host instance to access the browser JSON implementation)
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