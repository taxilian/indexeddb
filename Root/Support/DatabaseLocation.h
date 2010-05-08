/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_SUPPORT_DATABASELOCATION_H
#define BRANDONHAYNES_INDEXEDDB_SUPPORT_DATABASELOCATION_H

#include <string>
#include <boost/filesystem.hpp>

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

class DatabaseLocation
	{
	public:
		static const std::string getEnvironmentPath(const std::string& origin, const std::string& environmentName);
		static const std::string getObjectStorePath(const std::string& origin, const std::string& environmentName, const std::string& objectStoreName);
		static void ensurePathValid(const std::string& path);

	private:
		DatabaseLocation() { }

		static ::boost::filesystem::path userHome;
		static ::boost::filesystem::path environmentHome;
		static const char* illegalFilenameCharacters;
	};

}
}
}

#endif