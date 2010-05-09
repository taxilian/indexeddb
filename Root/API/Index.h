/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_INDEX_H
#define BRANDONHAYNES_INDEXEDDB_API_INDEX_H

#include <string>
#include <JSAPIAuto.h>
#include <boost/optional.hpp>

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

///<summary>
/// This class represents the an index in the Indexed Database API.  
///</summary>
class Index : public FB::JSAPIAuto
{
public:
	boost::optional<std::string> getKeyPath() const { return keyPath; }
	std::string getName() const { return indexName; }
	std::string getObjectStoreName() const { return objectStoreName; }
	bool getUnique() const { return unique; }

protected:
	Index(const std::string& indexName, const std::string& storeName, const boost::optional<std::string>& keyPath, const bool unique);
	Index(const std::string& indexName, const std::string& storeName);

	boost::optional<std::string> keyPath;
	bool unique;

private:
	const std::string indexName;
	const std::string objectStoreName;

	void initializeMethods();
};

}
}
}

#endif