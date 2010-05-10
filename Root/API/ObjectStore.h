/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

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

///<summary>
/// This class represents an object store as defined by the Indexed Database API.  It exposes all
/// functionality required by the specification.
///</summary>
class ObjectStore : public FB::JSAPIAuto
	{
	public:
		Implementation::ObjectStore::Mode getMode() const { return mode; }
		std::string getName() const { return name; }
		virtual FB::variant getKeyPath() const = 0;
		virtual std::vector<std::string> getIndexNames() const = 0;

	protected:
		ObjectStore(const std::string& name, const Implementation::ObjectStore::Mode mode);

	private:
		const Implementation::ObjectStore::Mode mode;
		const std::string name;

		// These helper methods convert strongly-typed values into weakly-typed FireBreath-compatible values
		FB::VariantList getIndexNameVariants() const { return FB::make_variant_list(getIndexNames()); }
		int getModeInteger(void) const { return mode; }
		int getReadWrite(void) const  { return Implementation::ObjectStore::READ_WRITE; }
		int getReadOnly(void) const  { return Implementation::ObjectStore::READ_ONLY; }
		int getReadSnapshot(void) const  { return Implementation::ObjectStore::SNAPSHOT_READ; }
	};

}
}
}

#endif