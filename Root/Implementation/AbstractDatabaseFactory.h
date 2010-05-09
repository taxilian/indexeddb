/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_ABSTRACTDATABASEFACTORY_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_ABSTRACTDATABASEFACTORY_H

#include <boost/optional.hpp>
#include <list>
#include "Transaction.h"
#include "ObjectStore.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	class Database;
	class Index;
	class Cursor;
	class Key;
	class KeyGenerator;

	///<summary>
	/// This is an abstract base factory interface for various implementations that back the Indexed Database API.
	/// Any new implementation my implement all methods herein in order to interact with the API layer.
	///
	/// Note that the static getInstance is currently a bit lame, and is not configurable (nor does it read from
	/// a configuration file or anything remotely sophisticated).  This will need to be fixed if subsequent
	/// backing data stores are attempted.
	///</summary>
	class AbstractDatabaseFactory
		{
		public:
			/// Retreives the currently configured factory for the application
			static AbstractDatabaseFactory& getInstance();

			/// Creates a new Indexed Database API database with the given configuration
			virtual std::auto_ptr<Database> createDatabase(const std::string& origin, const std::string& name, const std::string& description, const bool modifyDatabase = true) = 0;

			/// Creates a new Indexed Database API object store with the given configuration (and within the context of an optional transaction)
			virtual std::auto_ptr<ObjectStore> createObjectStore(Database& database, const std::string& name, const bool autoIncrement = true, TransactionContext& transactionContext = TransactionContext()) = 0;
			/// Opens an existing Indexed Database API object store with the given name and mode (and within the context of an optional transaction)
			virtual std::auto_ptr<ObjectStore> openObjectStore(Database& database, const std::string& name, const ObjectStore::Mode mode, TransactionContext& transactionContext) = 0;

			/// Opens a new cursor over the given object store on the interval (left, right) (and possibly open and/or reversed)
			virtual std::auto_ptr<Cursor> openCursor(ObjectStore& objectStore, const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates, TransactionContext& transactionContext) = 0;
			/// Opens a new cursor over the given index on the interval (left, right) 
			virtual std::auto_ptr<Cursor> openCursor(Index& index, const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates, const bool returnkeys, TransactionContext& transactionContext) = 0;
		
			/// Creates a transaction over the given database.  Any object stores passed in are locked for the duration of the transaction (per spec).  
			/// This may be a nested transaction.
			virtual std::auto_ptr<Transaction> createTransaction(Database& database, const ObjectStoreImplementationList& objectStores, const boost::optional<unsigned int>& timeout, TransactionContext& transactionContext) = 0;

			/// Creates a new index over a given object store.  The key generator is used to generate secondary keys on the index.
			virtual std::auto_ptr<Index> createIndex(ObjectStore& objectStore, const std::string& name, const std::auto_ptr<KeyGenerator>& keyGenerator, const bool unique, TransactionContext& transactionContext) = 0;
			/// Creates a new index over the given object store.  This overload does not include a key generator.
			virtual std::auto_ptr<Index> createIndex(ObjectStore& objectStore, const std::string& name, const bool unique, TransactionContext& transactionContext) = 0;
			/// Opens an index over the object store with the given key generator
			virtual std::auto_ptr<Index> openIndex(ObjectStore& objectStore, const std::string& name, const std::auto_ptr<KeyGenerator>& keyGenerator, const bool unique, TransactionContext& transactionContext) = 0;
			/// Opens an index over the object store with no key generator
			virtual std::auto_ptr<Index> openIndex(ObjectStore& objectStore, const std::string& name, const bool unique, TransactionContext& transactionContext) = 0;

		protected:
			AbstractDatabaseFactory() { }
		};
	}
}
}

#endif