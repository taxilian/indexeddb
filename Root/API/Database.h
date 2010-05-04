/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_DATABASE_H
#define BRANDONHAYNES_INDEXEDDB_API_DATABASE_H

#include <JSAPIAuto.h>
#include <variant_list.h>
#include "Transaction.h"

namespace BrandonHaynes {
namespace IndexedDB { 

	namespace Implementation { class Database; }

	namespace API 
		{ 
		class Transaction;

		class Database : public FB::JSAPIAuto
		{
		public:
			Database(const std::string& name, const std::string& description);

			std::string getName() const { return name; }
			std::string getDescription() const { return description; }
			virtual FB::variant getVersion() const = 0;

			virtual std::vector<std::string> getObjectStoreNames() = 0;
			virtual std::vector<std::string> getIndexNames() = 0;

		protected:
			//TODO after FB 1.0 upgrade, change long return to void
			virtual long setVersion(const std::string& version) = 0;
			virtual FB::variant getCurrentTransactionVariant() const = 0;

			virtual void onTransactionCommitted(const Transaction& transaction) = 0;
			virtual void onTransactionAborted(const Transaction& transaction) = 0;

		private:	
			const std::string name;
			const std::string description;
			
			friend long Transaction::commit();
			friend long Transaction::abort();

			FB::VariantList getIndexVariants() { return FB::make_variant_list(getIndexNames()); }
			FB::VariantList getObjectStoreVariants() { return FB::make_variant_list(getObjectStoreNames()); }
		};
	}
}
}

#endif