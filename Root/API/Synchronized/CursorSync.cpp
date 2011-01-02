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
#include "../../Support/privateObservable.h"
#include "../DatabaseException.h"

using boost::optional;

namespace BrandonHaynes {
namespace IndexedDB { 

using Implementation::Key;
using Implementation::AbstractDatabaseFactory;
using Implementation::ImplementationException;

namespace API { 

void CursorSync::addLifeCycleObserver( const LifeCycleObserverPtr& observer )
{
    _observable->addLifeCycleObserver(observer);
}

void CursorSync::removeLifeCycleObserver( const LifeCycleObserverPtr& observer )
{
    _observable->removeLifeCycleObserver(observer);
}

CursorSync::CursorSync(FB::BrowserHostPtr host, const ObjectStoreSyncPtr& objectStore, TransactionFactory& transactionFactory, const KeyRangePtr& range, const Cursor::Direction direction)
	: Cursor(direction), 
	  transactionFactory(transactionFactory),
	  readOnly(objectStore->getMode() != Implementation::ObjectStore::READ_WRITE),
	  host(host), range(range),
	  implementation(AbstractDatabaseFactory::getInstance().openCursor(
		objectStore->getImplementation(), 
		range ? Convert::toKey(host, range->getLeft()) : Key::getUndefinedKey(),
		range ? Convert::toKey(host, range->getRight()) : Key::getUndefinedKey(),
		range ? range->getFlags() & KeyRange::LEFT_OPEN : false,
		range ? range->getFlags() & KeyRange::RIGHT_OPEN : false,
		direction == Cursor::PREV || direction == Cursor::PREV_NO_DUPLICATE,
		direction == Cursor::NEXT_NO_DUPLICATE || direction == Cursor::PREV_NO_DUPLICATE, 
		transactionFactory.getTransactionContext()))
	{
	initializeMethods();
	}

CursorSync::CursorSync(FB::BrowserHostPtr host, const IndexSyncPtr& index, TransactionFactory& transactionFactory, const KeyRangePtr& range, const Cursor::Direction direction, const bool returnKeys)
	: Cursor(direction), readOnly(false),
	  transactionFactory(transactionFactory),
	  host(host), range(range),
	  implementation(AbstractDatabaseFactory::getInstance().openCursor(
		*(index->implementation), 
		range ? Convert::toKey(host, range->getLeft()) : Key::getUndefinedKey(),
		range ? Convert::toKey(host, range->getRight()) : Key::getUndefinedKey(),
		range ? range->getFlags() & KeyRange::LEFT_OPEN : false,
		range ? range->getFlags() & KeyRange::RIGHT_OPEN : false,
		direction == Cursor::PREV || direction == Cursor::PREV_NO_DUPLICATE,
		direction == Cursor::NEXT_NO_DUPLICATE || direction == Cursor::PREV_NO_DUPLICATE,
		returnKeys,
		transactionFactory.getTransactionContext()))
	{
	initializeMethods();
	}

CursorSync::~CursorSync()
	{
    this->close();
    FB::ptr_cast<Support::_privateObservable<CursorSync> >(_observable)->invalidate();
    }

void CursorSync::initializeMethods()
	{
    _observable = boost::make_shared<Support::_privateObservable<CursorSync> >(this);
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
	_observable->raiseOnCloseEvent();
	this->implementation->close(); 
	}

}
}
}