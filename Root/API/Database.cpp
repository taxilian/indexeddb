#include "Database.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

Database::Database(const std::string& name, const std::string& description)
	: name(name), description(description)
	{
	registerProperty("name", make_property(this, &Database::getName));
	registerProperty("description", make_property(this, &Database::getDescription));
	registerProperty("version", make_property(this, &Database::getVersion));
	registerProperty("objectStores", make_property(this, &Database::getObjectStoreVariants));
	registerProperty("indexes", make_property(this, &Database::getIndexVariants));
	registerProperty("currentTransaction", make_property(this, &Database::getCurrentTransactionVariant));
	}

}
}
}