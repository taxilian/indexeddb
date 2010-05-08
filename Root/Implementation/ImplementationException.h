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

	class ImplementationException : public std::exception
		{
		public:
			enum ImplementationCode { UNKNOWN_ERR = 0,		NON_TRANSIENT_ERR = 1,	NOT_FOUND_ERR = 2,	CONSTRAINT_ERR = 3, 
									  DATA_ERR = 4,			NOT_ALLOWED_ERR = 5,	SERIAL_ERR = 11,	RECOVERABLE_ERR = 21, 
									  TRANSIENT_ERR = 31,	TIMEOUT_ERR = 32,		DEADLOCK_ERR = 33 };

			ImplementationException(const ImplementationCode code)
				: code(code), message(boost::lexical_cast<std::string>(code)), underlyingCode(-1)
				{ }
			ImplementationException(const ImplementationCode code, const unsigned int underlyingCode)
				: code(code), message(boost::lexical_cast<std::string>(code)), underlyingCode(underlyingCode)
				{ }
			ImplementationException(const std::string& message, const ImplementationCode code)
				: code(code), message(message), underlyingCode(-1)
				{ }
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