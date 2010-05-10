/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_DATABASE_H
#define BRANDONHAYNES_INDEXEDDB_API_DATABASE_H

#include <JSAPIAuto.h>
#include <boost/optional.hpp>
#include <variant_list.h>
#include "Transaction.h"

namespace BrandonHaynes {
namespace IndexedDB { 

	namespace Implementation { class Database; }

	namespace API 
		{ 
		class Transaction;

		///<summary>
		/// This class represents a database in the Indexed Database API implementation.
		///</summary>
		class Database : public FB::JSAPIAuto
		{
		public:
			// Gets the name associated with this database
			std::string getName() const { return name; }
			// Gets the database associated with this database
			std::string getDescription() const { return description; }
			// Get this database's version
			virtual boost::optional<std::string> getVersion() const = 0;

			// Gets the current transaction associated with this database, if any
			virtual boost::optional<Transaction&> getCurrentTransaction() const = 0;

			// Get the set of object store names associated with this database
			virtual std::vector<std::string> getObjectStoreNames() = 0;
			// Gets the set of index names associated with this database
			virtual std::vector<std::string> getIndexNames() = 0;

		protected:
			// Creates a database with the given name and description
			Database(const std::string& name, const std::string& description);

			virtual void setVersion(const std::string& version) = 0;

			// Internal method to convert the current transac
			virtual FB::variant getCurrentTransactionVariant() const = 0;

			// Methods by which a Transaction instance notifies its parent database that it has committed or aborted
			virtual void onTransactionCommitted(const Transaction& transaction) = 0;
			virtual void onTransactionAborted(const Transaction& transaction) = 0;

		private:	
			const std::string name;
			const std::string description;
			
			// Grant access to these methods so that they can notify us when the transaction state changes
			friend void Transaction::commit();
			friend void Transaction::abort();

			// These methods convert the name vectors into FireBreath-compatible variants/variant lists
			FB::VariantList getIndexVariants() { return FB::make_variant_list(getIndexNames()); }
			FB::VariantList getObjectStoreVariants() { return FB::make_variant_list(getObjectStoreNames()); }
			FB::variant getVersionVariant() 
				{ 
				boost::optional<std::string> version = getVersion();
				return version.is_initialized()
					? *version
					: FB::variant();
				}
		};
	}
}
}

#endif