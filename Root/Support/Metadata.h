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

class Metadata
{
public:
	enum MetadataType { Database, ObjectStore, Index };

	Metadata(Metadata& metadataSource, const MetadataType type, const std::string& name)
		: metadata(metadataSource.metadata), keyPrefix(boost::lexical_cast<std::string>(type) + + separatorToken + name + separatorToken)
		{ }

	Metadata(Implementation::ObjectStore& metadata, const MetadataType type, const std::string& name)
		: metadata(metadata), keyPrefix(boost::lexical_cast<std::string>(type) + + separatorToken + name + separatorToken)
		{ }

	void putMetadata(const std::string& key, const Implementation::Data& data, Implementation::TransactionContext& transactionContext)
		{ putMetadata(key, data, false, transactionContext); }
	void putMetadata(const std::string& key, const StringVector& values, Implementation::TransactionContext& transactionContext)
		{ putMetadata(key, values, false, transactionContext); }
	void addToMetadataCollection(const std::string& key, const std::string& value, TransactionFactory& transactionFactory, Implementation::TransactionContext& transactionContext)
		{ addToMetadataCollection(key, value, false, transactionFactory, transactionContext); }

	Implementation::Data getMetadata(const std::string& key, Implementation::TransactionContext& transactionContext) const;
	StringVector getMetadataCollection(const std::string& key, Implementation::TransactionContext& transactionContext) const;
	void putMetadata(const std::string& key, const Implementation::Data& data, const bool noOverwrite, Implementation::TransactionContext& transactionContext);
	void putMetadata(const std::string& key, const StringVector& values, const bool noOverwrite, Implementation::TransactionContext& transactionContext);
	void addToMetadataCollection(const std::string& key, const std::string& value, const bool noOverwrite, TransactionFactory& transactionFactory, Implementation::TransactionContext& transactionContext);
	void removeFromMetadataCollection(const std::string& key, const std::string& value, TransactionFactory& transactionFactory, Implementation::TransactionContext& transactionContext);

private:
	const std::string keyPrefix;
	Implementation::ObjectStore& metadata;
	static const char* separatorToken;
};

}
}
}

#endif