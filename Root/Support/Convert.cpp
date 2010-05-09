/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include "Convert.h"
#include "BrowserObjectAPI.h"
#include "util/JSArray.h"
#include "DOM/JSAPI_DomWindow.h"
#include "../API/DatabaseException.h"

using std::string;

namespace BrandonHaynes {
namespace IndexedDB { 

using Implementation::Key;
using Implementation::Data;

namespace API { 

Data Convert::toData(FB::BrowserHost host, const FB::variant& variant)
	{ return convert<Data>(host, variant); }

Key Convert::toKey(FB::BrowserHost host, const FB::variant& variant)
	{ return convert<Key>(host, variant); }

template<class T>
T Convert::convert(FB::BrowserHost host, const FB::variant& variant)
	{
	switch(getType(variant))
		{
		case Data::String:
			return T(variant.cast<string>());
		case Data::Integer:
			return T((void *)&variant.cast<int>(), sizeof(int), Data::Integer);
		case Data::Boolean:
			return T((void *)&variant.cast<bool>(), sizeof(bool), Data::Boolean);
		case Data::Number:
			return T((void *)&variant.cast<double>(), sizeof(double), Data::Number);
		case Data::Object:	
			{
			string stringifiedObject = stringify(host, variant.cast<FB::JSObject>());
			return T((void *)stringifiedObject.c_str(), stringifiedObject.size() + 1, Data::Object);
			}
		case Data::Undefined:
			return T::getUndefinedData();
		case Data::Null:
			return T::getUndefinedData();
		default:
			throw DatabaseException("An unexpected type was encountered during conversion from variant.", DatabaseException::UNKNOWN_ERR);
		}
	}

FB::variant Convert::toVariant(FB::BrowserHost host, const Data& data)
	{
	switch(data.getType())
		{
		case Data::String:
			return string(static_cast<const char*>(data.getRawValue()));
		case Data::Integer:
			return *static_cast<const long*>(data.getRawValue());
		case Data::Boolean:
			return *static_cast<const bool *>(data.getRawValue());
		case Data::Number:
			return *static_cast<const double *>(data.getRawValue());
		case Data::Object:
			return parse(host, string(static_cast<const char*>(data.getRawValue())));
		case Data::Null:
			return FB::variant();
		case Data::Undefined:
			return FB::variant();
		default:
			throw DatabaseException("An unexpected type was encountered during conversion to variant.", DatabaseException::UNKNOWN_ERR);
		}
	}

Data::ECMAType Convert::getType(FB::variant variant)
	{
	if(variant.is_of_type<string>())
		return Data::String;
	else if(variant.is_of_type<int>())
		return Data::Integer;
	else if(variant.is_of_type<bool>())
		return Data::Boolean;
	else if(variant.is_of_type<double>())
		return Data::Number;
	else if(variant.is_of_type<FB::JSObject>())
		return Data::Object;
	else if(variant.empty())
		return Data::Undefined;
	else
		throw DatabaseException("An unexpected variant type was encountered.", DatabaseException::UNKNOWN_ERR);
	}

std::string Convert::stringify(FB::BrowserHost host, const FB::JSObject& object)
	{
	if(host == NULL)
		throw DatabaseException("Browser host was null.", DatabaseException::NOT_FOUND_ERR);
	else if(!host->getDOMWindow().getProperty<FB::variant>("JSON").is_of_type<FB::JSObject>())
		throw DatabaseException("Could not cast window.JSON to type object.", DatabaseException::UNKNOWN_ERR);

	FB::JSObject json = host->getDOMWindow().getProperty<FB::JSObject>("JSON");

	if(json == NULL)
		throw DatabaseException("window.JSON support not available.", DatabaseException::NOT_FOUND_ERR);
	else if(json->HasMethod("stringify"))
		{
		FB::VariantList arguments(1, object);
		FB::variant result = json->Invoke("stringify", arguments);

		if(result.empty())
			throw DatabaseException("JSON Stringification failed.", DatabaseException::RECOVERABLE_ERR);
		else
			return result.cast<std::string>();
		}
	else
		throw DatabaseException("window.JSON missing method stringify().", DatabaseException::NOT_FOUND_ERR);
	}

FB::JSObject Convert::parse(FB::BrowserHost host, const std::string& string)
	{
	if(host == NULL)
		throw DatabaseException("Browser host was null.", DatabaseException::NOT_FOUND_ERR);
	else if(!host->getDOMWindow().getProperty<FB::variant>("JSON").is_of_type<FB::JSObject>())
		throw DatabaseException("Could not cast window.JSON to type object.", DatabaseException::UNKNOWN_ERR);

	FB::JSObject json = host->getDOMWindow().getProperty<FB::JSObject>("JSON");

	if(json == NULL)
		throw DatabaseException("window.JSON support not available.", DatabaseException::NOT_FOUND_ERR);
	else if(json->HasMethod("parse"))
		{
		FB::VariantList arguments(1, string);
		FB::variant result = json->Invoke("parse", arguments);

		if(result.empty())
			throw DatabaseException("JSON parsing failed.", DatabaseException::RECOVERABLE_ERR);
		else
			return result.cast<FB::JSObject>();
		}
	else
		throw DatabaseException("window.JSON missing method parse().", DatabaseException::NOT_FOUND_ERR);
	}

}
}
}