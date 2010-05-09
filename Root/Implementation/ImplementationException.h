/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_IMPLEMENTATIONEXCEPTION_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_IMPLEMENTATIONEXCEPTION_H

#include <boost/lexical_cast.hpp>

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	///<summary>
	/// This class represents an exception at the implementation level of the application.  It is used
	/// to exchange error state between the implementation and API; no implementation should throw any
	/// exception except this one.
	///</summary>
	// TODO: We probably want to maintain a reference to the underlying cause exception here, even if it's not 
	// used in the API.
	class ImplementationException : public std::exception
		{
		public:
			// An enumeration of the potential error codes that the implementation might throw.
			// TODO: Repeating the API codes here and in DatabaseException is lame.  How do we reconcile?
			enum ImplementationCode { UNKNOWN_ERR = 0,		NON_TRANSIENT_ERR = 1,	NOT_FOUND_ERR = 2,	CONSTRAINT_ERR = 3, 
									  DATA_ERR = 4,			NOT_ALLOWED_ERR = 5,	SERIAL_ERR = 11,	RECOVERABLE_ERR = 21, 
									  TRANSIENT_ERR = 31,	TIMEOUT_ERR = 32,		DEADLOCK_ERR = 33 };


			// Create an instance with the given error code.  The code is converted to a string; this becomes 
			// the exeption's message
			ImplementationException(const ImplementationCode code)
				: code(code), message(boost::lexical_cast<std::string>(code)), underlyingCode(-1)
				{ }
			// Create an instance with the given code and underlying implementation-defined code
			ImplementationException(const ImplementationCode code, const unsigned int underlyingCode)
				: code(code), message(boost::lexical_cast<std::string>(code)), underlyingCode(underlyingCode)
				{ }
			// Create an instance with the given message and error code
			ImplementationException(const std::string& message, const ImplementationCode code)
				: code(code), message(message), underlyingCode(-1)
				{ }
			// Create an instance with the given message, error code, and implementation-defined underlying code
			ImplementationException(const std::string& message, const ImplementationCode code, const unsigned int underlyingCode)
				: code(code), message(message), underlyingCode(underlyingCode)
				{ }

			const ImplementationCode code;
			const unsigned int underlyingCode;
			const std::string message;
		};
	}
}
}

#endif