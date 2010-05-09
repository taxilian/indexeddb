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

///<summary>
/// This utility class is used to retreive operating system-specific paths for implementations.  It was
/// designed with Berkeley DB in mind, but is probably useful across other environments.
///</summary>
class DatabaseLocation
	{
	public:
		// Gets a valid database storage path for this origin, specific to a given user (via an environment variable)
		static const std::string getDatabasePath(const std::string& origin, const std::string& databaseName);
		// Gets a valid object storage path for this origin, specific to a given user (and underneath the database path)
		static const std::string getObjectStorePath(const std::string& origin, const std::string& databaseName, const std::string& objectStoreName);
		// Given a path, performs some sanity checks on it to ensure that there is no cross-origin or naming issues
		static void ensurePathValid(const std::string& path);

	private:
		DatabaseLocation() { }

		// Stores a reference to the user-specific home
		static ::boost::filesystem::path userHome;
		// Stores a relative reference to the database path
		static ::boost::filesystem::path databaseHome;
		// A set of invalid filename characters specific to "this" operating system
		static const char* illegalFilenameCharacters;
	};

}
}
}

#endif