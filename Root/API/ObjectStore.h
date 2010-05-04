#ifndef BRANDONHAYNES_INDEXEDDB_API_OBJECTSTORE_H
#define BRANDONHAYNES_INDEXEDDB_API_OBJECTSTORE_H

#include <vector>
#include <string>
#include <JSAPIAuto.h>
#include <variant_list.h>
#include "../Implementation/ObjectStore.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

class ObjectStore : public FB::JSAPIAuto
	{
	public:
		const Implementation::ObjectStore::Mode getMode() const { return mode; }
		const std::string getName() const { return name; }
		virtual const FB::variant getKeyPath() const = 0;
		virtual std::vector<std::string> getIndexNames() const = 0;

	protected:
		ObjectStore(const std::string& name, const Implementation::ObjectStore::Mode mode);

	private:
		const Implementation::ObjectStore::Mode mode;
		const std::string name;

		virtual FB::VariantList getIndexNameVariants() { return FB::make_variant_list(getIndexNames()); }
		int getModeInteger(void) const { return mode; }
		int getReadWrite(void) const  { return Implementation::ObjectStore::READ_WRITE; }
		int getReadOnly(void) const  { return Implementation::ObjectStore::READ_ONLY; }
		int getReadSnapshot(void) const  { return Implementation::ObjectStore::SNAPSHOT_READ; }
	};

}
}
}

#endif