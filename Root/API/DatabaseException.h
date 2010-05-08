/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_DATABASEEXCEPTION_H
#define BRANDONHAYNES_INDEXEDDB_API_DATABASEEXCEPTION_H

#include <JSAPI.h>
#include <boost/lexical_cast.hpp>
#include "../Implementation/ImplementationException.h"

namespace BrandonHaynes {
namespace IndexedDB 
	{ 
	namespace Implementation { class ImplementationException; }
	
	namespace API 
		{ 
		class DatabaseException : public FB::script_error
			{
			public:
				enum APICode { UNKNOWN_ERR = 0,		NON_TRANSIENT_ERR = 1,	NOT_FOUND_ERR = 2,	CONSTRAINT_ERR = 3, 
							   DATA_ERR = 4,		NOT_ALLOWED_ERR = 5,	SERIAL_ERR = 11,	RECOVERABLE_ERR = 21, 
							   TRANSIENT_ERR = 31,	TIMEOUT_ERR = 32,		DEADLOCK_ERR = 33 };

				DatabaseException(const APICode code)
					: code(code), 
					  message(boost::lexical_cast<std::string>(code)), script_error(boost::lexical_cast<std::string>(code))
					{ }

				DatabaseException(const std::string& message, APICode code)
					: code(code), message(message), script_error(message)
					{ }

				DatabaseException(Implementation::ImplementationException& cause)
					: code(static_cast<APICode>(cause.code)), message(cause.message), script_error(cause.message)
					{ }

				DatabaseException(std::string message, Implementation::ImplementationException& cause)
					: code(static_cast<APICode>(cause.code)), message(message + " (implementation reported " + cause.message + ")"), script_error(cause.message)
					{ }

				APICode get_code() const { return code; }
				std::string get_message() const { return message; }

			private:
				const APICode code;
				const std::string message;

				APICode getUnknownErrorCode() const { return UNKNOWN_ERR; }
				APICode getNonTransientErrorCode() const { return NON_TRANSIENT_ERR; }
				APICode getNotFoundErrorCode() const { return NOT_FOUND_ERR; }
				APICode getConstraintErrorCode() const { return CONSTRAINT_ERR; }
				APICode getDataErrorCode() const { return DATA_ERR; }
				APICode getNotAllowedErrorCode() const { return NOT_ALLOWED_ERR; }
				APICode getSerialErrorCode() const { return SERIAL_ERR; }
				APICode getRecoverableErrorCode() const { return RECOVERABLE_ERR; }
				APICode getTransientErrorCode() const { return TRANSIENT_ERR; }
				APICode getTimeoutErrorCode() const { return TIMEOUT_ERR; }
				APICode getDeadlockErrorCode() const { return DEADLOCK_ERR; }
			};
		}
	}
}

#endif