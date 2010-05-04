#include "IndexedDatabase.h"
#include "Synchronized/DatabaseSync.h"
#include "DatabaseException.h"

using std::string;

namespace BrandonHaynes {
namespace IndexedDB { 

using Implementation::ImplementationException;

namespace API { 

IndexedDatabase::IndexedDatabase(FB::BrowserHost host)
	: host(host)
	{ registerMethod("open", make_method(this, &IndexedDatabase::open)); }

FB::JSOutObject IndexedDatabase::open(const string& name, const string& description, const FB::CatchAll& args)
	{
	const FB::VariantList& values = args.value;
	if(values.size() > 1)
		throw FB::invalid_arguments();
	else if(values.size() == 1 && !values[0].is_of_type<bool>())
		throw FB::invalid_arguments();

	bool modifyDatabase = values.size() == 1 ? values[0].cast<bool>() : true;

	try
		{ return new DatabaseSync(host, name, description, modifyDatabase); }
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}

}
}
}