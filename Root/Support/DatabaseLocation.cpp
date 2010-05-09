/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include "DatabaseLocation.h"
#include "../Implementation/ImplementationException.h"

using std::string;
using boost::filesystem::path;
using boost::filesystem::create_directory;
using boost::filesystem::exists;
using boost::filesystem::basic_filesystem_error;

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)
	path DatabaseLocation::userHome(getenv("APPDATA"));
	path DatabaseLocation::databaseHome("indexedDB");
	const char* DatabaseLocation::illegalFilenameCharacters = "\\/*?<>|`";
#else
	path DatabaseLocation::userHome(getenv("HOME"));
	path DatabaseLocation::databaseHome(".indexedDB");
	const char* DatabaseLocation::illegalFilenameCharacters = "\\/*?<>|`";
#endif

const string DatabaseLocation::getDatabasePath(const string& origin, const string& databaseName)
	{
	ensurePathValid(origin);
	ensurePathValid(databaseName);

	const path databaseRoot(userHome / databaseHome);
	const path originRoot(databaseRoot / (origin.size() != 0 ? origin : "local_filesystem"));
	const path databaseHome(originRoot / databaseName);

	try
		{
		if(!exists(databaseRoot))
			create_directory(databaseRoot);
		if(!exists(originRoot))
			create_directory(originRoot);
		if(!exists(databaseHome))
			create_directory(databaseHome);
		}
	catch(basic_filesystem_error<path>& e)
		{ throw ImplementationException(e.what(), ImplementationException::NON_TRANSIENT_ERR); }

	return databaseHome.file_string();
	}

const string DatabaseLocation::getObjectStorePath(const string& origin, const string& databaseName, const string& objectStoreName)
	{
	ensurePathValid(databaseName);
	ensurePathValid(objectStoreName);
	return (userHome / databaseHome / (origin.size() != 0 ? origin : "local_filesystem") / databaseName / objectStoreName).file_string();
	}

void DatabaseLocation::ensurePathValid(const std::string& path)
	{
	if(path.find("..") != string::npos || path.find_first_of(illegalFilenameCharacters) != string::npos)
		throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);
	}

}
}
}