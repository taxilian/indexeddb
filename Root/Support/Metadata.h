/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_SUPPORT_METADATA_H
#define BRANDONHAYNES_INDEXEDDB_SUPPORT_METADATA_H

#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>
#include "TransactionFactory.h"
#include "../Implementation/Transaction.h"
#include "../Implementation/ObjectStore.h"
#include "../Implementation/Key.h"
#include "../Implementation/Data.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

///<summary>
/// This class wraps an object store instance and provides some convenience methods for API metadata-related
/// requirements.
///
/// This class does not perform cross-entity checks on keys, and therefore it should not be directly
/// exposed to external consumers.
///
/// The keyspace is divided as follows: [type]..[entity name]..[key]
///</summary>
// TODO: Would probably be stronger to use duplicate keys rather than a tokenized collection
class Metadata
{
public:
	// Enumeration identifying the type of metadata that is being stored in this instance
	// This enumeration is used to partition the keyspace; multiple entities may share the underlying
	// object store.
	enum MetadataType { Database, ObjectStore, Index };

	Metadata(Metadata& metadataSource, const MetadataType type, const std::string& name)
		: metadata(metadataSource.metadata), keyPrefix(boost::lexical_cast<std::string>(type) + + separatorToken + name + separatorToken)
		{ }

	Metadata(Implementation::ObjectStore& metadata, const MetadataType type, const std::string& name)
		: metadata(metadata), keyPrefix(boost::lexical_cast<std::string>(type) + + separatorToken + name + separatorToken)
		{ }

	// Puts a value into the metabase with the given key
	void putMetadata(const std::string& key, const Implementation::Data& data, Implementation::TransactionContext& transactionContext)
		{ putMetadata(key, data, false, transactionContext); }
	// Puts a string vector into the metabase under the given key; the vector is token-separated
	void putMetadata(const std::string& key, const StringVector& values, Implementation::TransactionContext& transactionContext)
		{ putMetadata(key, values, false, transactionContext); }
	// Adds a single value to an existing string vector stored in the metabase
	void addToMetadataCollection(const std::string& key, const std::string& value, TransactionFactory& transactionFactory, Implementation::TransactionContext& transactionContext)
		{ addToMetadataCollection(key, value, false, transactionFactory, transactionContext); }

	// Given a key, retrieves the value associated therewith
	Implementation::Data getMetadata(const std::string& key, Implementation::TransactionContext& transactionContext) const;
	// Given a key, retrieves the string vectory associated therewith
	StringVector getMetadataCollection(const std::string& key, Implementation::TransactionContext& transactionContext) const;
	// Puts a value into the metabase under a given key
	void putMetadata(const std::string& key, const Implementation::Data& data, const bool noOverwrite, Implementation::TransactionContext& transactionContext);
	// Puts a string vector into the metabase under a given key
	void putMetadata(const std::string& key, const StringVector& values, const bool noOverwrite, Implementation::TransactionContext& transactionContext);
	// Adds a single string value to a string vector stored in the metabase
	void addToMetadataCollection(const std::string& key, const std::string& value, const bool noOverwrite, TransactionFactory& transactionFactory, Implementation::TransactionContext& transactionContext);
	// Removes a key/value pair from the metabase
	void removeFromMetadataCollection(const std::string& key, const std::string& value, TransactionFactory& transactionFactory, Implementation::TransactionContext& transactionContext);

private:
	// We cache the key prefix: [type]..[entity name]..
	const std::string keyPrefix;
	// The underlying object store that backs this metabase
	Implementation::ObjectStore& metadata;

	// A static separator token for the keys
	static const char* separatorToken;
};

}
}
}

#endif