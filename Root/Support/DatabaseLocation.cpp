/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
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
	path DatabaseLocation::environmentHome("indexedDB");
	const char* DatabaseLocation::illegalFilenameCharacters = "\\/*?<>|`";
#else
	path DatabaseLocation::userHome(getenv("HOME"));
	path DatabaseLocation::environmentHome(".indexedDB");
	const char* DatabaseLocation::illegalFilenameCharacters = "\\/*?<>|`";
#endif

const string DatabaseLocation::getEnvironmentPath(const string& origin, const string& environmentName)
	{
	ensurePathValid(origin);
	ensurePathValid(environmentName);

	const path environmentRoot(userHome / environmentHome);
	const path originRoot(environmentRoot / (origin.size() != 0 ? origin : "local_filesystem"));
	const path environmentHome(originRoot / environmentName);

	try
		{
		if(!exists(environmentRoot))
			create_directory(environmentRoot);
		if(!exists(originRoot))
			create_directory(originRoot);
		if(!exists(environmentHome))
			create_directory(environmentHome);
		}
	catch(basic_filesystem_error<path>& e)
		{ throw ImplementationException(e.what(), ImplementationException::NON_TRANSIENT_ERR); }

	return environmentHome.file_string();
	}

const string DatabaseLocation::getObjectStorePath(const string& origin, const string& environmentName, const string& objectStoreName)
	{
	ensurePathValid(environmentName);
	ensurePathValid(objectStoreName);
	return (userHome / environmentHome / (origin.size() != 0 ? origin : "local_filesystem") / environmentName / objectStoreName).file_string();
	}

void DatabaseLocation::ensurePathValid(const std::string& path)
	{
	if(path.find("..") != string::npos || path.find_first_of(illegalFilenameCharacters) != string::npos)
		throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);
	}

}
}
}