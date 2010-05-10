/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include <BrowserObjectAPI.h>
#include "CursorSync.h"
#include "ObjectStoreSync.h"
#include "../../Implementation/Cursor.h"
#include "../../Implementation/AbstractDatabaseFactory.h"
#include "../../Support/Convert.h"
#include "../DatabaseException.h"

using boost::optional;

namespace BrandonHaynes {
namespace IndexedDB { 

using Implementation::Key;
using Implementation::AbstractDatabaseFactory;
using Implementation::ImplementationException;

namespace API { 

CursorSync::CursorSync(FB::BrowserHost host, ObjectStoreSync& objectStore, TransactionFactory& transactionFactory, const optional<KeyRange>& range, const Cursor::Direction direction)
	: Cursor(direction), 
	  transactionFactory(transactionFactory),
	  readOnly(objectStore.getMode() != Implementation::ObjectStore::READ_WRITE),
	  host(host), range(range),
	  implementation(AbstractDatabaseFactory::getInstance().openCursor(
		objectStore.getImplementation(), 
		range.is_initialized() ? Convert::toKey(host, range->getLeft()) : Key::getUndefinedKey(),
		range.is_initialized() ? Convert::toKey(host, range->getRight()) : Key::getUndefinedKey(),
		range.is_initialized() ? range->getFlags() & KeyRange::LEFT_OPEN : false,
		range.is_initialized() ? range->getFlags() & KeyRange::RIGHT_OPEN : false,
		direction == Cursor::PREV || direction == Cursor::PREV_NO_DUPLICATE,
		direction == Cursor::NEXT_NO_DUPLICATE || direction == Cursor::PREV_NO_DUPLICATE, 
		transactionFactory.getTransactionContext()))
	{
	initializeMethods();
	}

CursorSync::CursorSync(FB::BrowserHost host, IndexSync& index, TransactionFactory& transactionFactory, const optional<KeyRange>& range, const Cursor::Direction direction, const bool returnKeys)
	: Cursor(direction), readOnly(false),
	  transactionFactory(transactionFactory),
	  host(host), range(range),
	  implementation(AbstractDatabaseFactory::getInstance().openCursor(
		*index.implementation, 
		range.is_initialized() ? Convert::toKey(host, range->getLeft()) : Key::getUndefinedKey(),
		range.is_initialized() ? Convert::toKey(host, range->getRight()) : Key::getUndefinedKey(),
		range.is_initialized() ? range->getFlags() & KeyRange::LEFT_OPEN : false,
		range.is_initialized() ? range->getFlags() & KeyRange::RIGHT_OPEN : false,
		direction == Cursor::PREV || direction == Cursor::PREV_NO_DUPLICATE,
		direction == Cursor::NEXT_NO_DUPLICATE || direction == Cursor::PREV_NO_DUPLICATE,
		returnKeys,
		transactionFactory.getTransactionContext()))
	{
	initializeMethods();
	}

CursorSync::~CursorSync()
	{ this->close(); }

void CursorSync::initializeMethods()
	{
	registerProperty("key", make_property(this, &CursorSync::getKey));
	registerProperty("value", make_property(this, &CursorSync::getValue));
	registerProperty("count", make_property(this, &CursorSync::getCount));
	registerMethod("continue", make_method(this, &CursorSync::next));
	registerMethod("remove", make_method(this, &CursorSync::remove));
	registerMethod("close", make_method(this, &CursorSync::close));
	}

FB::variant CursorSync::getKey()
	{ 
	try
		{ return Convert::toVariant(host, implementation->getKey()); }
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}

FB::variant CursorSync::getValue()
	{ 
	try
		{ return Convert::toVariant(host, implementation->getData(transactionFactory.getTransactionContext())); }
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}

int CursorSync::getCount()
	{ 
	try
		{ return implementation->getCount(transactionFactory.getTransactionContext()); }
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}

bool CursorSync::next(const FB::CatchAll& args)
	{ 
	const FB::VariantList& values = args.value;

	try
		{
		if(values.size() > 1)
			throw FB::invalid_arguments();
		else if(values.size() == 1)
			return implementation->next(Convert::toKey(host, values[0]));
		else
			return implementation->next(transactionFactory.getTransactionContext());
		}
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}

void CursorSync::remove()
	{ 
	if(!readOnly)
		this->implementation->remove();
	else
		throw DatabaseException("NOT_ALLOWED_ERR", DatabaseException::NOT_ALLOWED_ERR);
	}

void CursorSync::close()
	{
	this->raiseOnCloseEvent();
	this->implementation->close(); 
	}

}
}
}