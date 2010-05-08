/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include <sstream>
#include <boost/tokenizer.hpp>
#include "Metadata.h"
#include "../API/DatabaseException.h"
#include "../Implementation/Transaction.h"

using std::auto_ptr;
using std::string;
using std::ostringstream;
using std::ostream_iterator;

namespace BrandonHaynes {
namespace IndexedDB { 

using class Implementation::Key;
using class Implementation::Data;
using class Implementation::Transaction;
using class Implementation::ImplementationException;
using Implementation::TransactionContext;

namespace API { 

const char* Metadata::separatorToken = "\\..\\";

Data Metadata::getMetadata(const std::string& key, TransactionContext& transactionContext) const
	{
	try
		{ return metadata.get(Key(keyPrefix + key), transactionContext); }
	catch(ImplementationException& e)
		{ throw DatabaseException("Unexpected metadata.get failure.", e); }
	}

void Metadata::putMetadata(const string& key, const StringVector& values, const bool noOverwrite, TransactionContext& transactionContext)
	{ 
	ostringstream stream;
	copy(values.begin(), values.end(), ostream_iterator<string>(stream, separatorToken));
	putMetadata(key, Data(stream.str()), noOverwrite, transactionContext); 
	}

void Metadata::putMetadata(const std::string& key, const Data& data, const bool noOverwrite, TransactionContext& transactionContext)
	{ 
	try
		{ metadata.put(Key(keyPrefix + key), data, noOverwrite, transactionContext); }
	catch(ImplementationException& e)
		{ throw DatabaseException("Unexpected metadata.put failure.", e); }
	}

StringVector Metadata::getMetadataCollection(const string& key, TransactionContext& transactionContext) const
	{ 
	Data& data = getMetadata(key, transactionContext);
	if(data.getType() == Data::String)
		{
		string collectionString((char*)data.getUntypedValue());
		boost::char_delimiters_separator<char> separator(false, "", separatorToken);
		boost::tokenizer<boost::char_delimiters_separator<char>> tokens(collectionString, separator);
		StringVector result(tokens.begin(), tokens.end());
		
		return result;
		}
	else
		return StringVector();
	}

void Metadata::addToMetadataCollection(const string& key, const string& value, const bool noOverwrite, TransactionFactory& transactionFactory, TransactionContext& transactionContext)
	{
	auto_ptr<Transaction> transaction = transactionFactory.createTransaction(transactionContext);

	StringVector values = getMetadataCollection(key, *transaction);
	values.push_back(value);
	putMetadata(key, values, noOverwrite, *transaction);
	
	transaction->commit();
	}

void Metadata::removeFromMetadataCollection(const string& key, const string& value, TransactionFactory& transactionFactory, TransactionContext& transactionContext)
	{
	auto_ptr<Transaction> transaction = transactionFactory.createTransaction(transactionContext);

	StringVector values = getMetadataCollection(key, *transaction);
	values.erase(remove(values.begin(), values.end(), value), values.end());
	putMetadata(key, values, *transaction);

	transaction->commit();
	}

}
}
}