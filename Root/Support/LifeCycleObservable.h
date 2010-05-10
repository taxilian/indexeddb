/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_SUPPORT_LIFECYCLEOBSERVABLE_H
#define BRANDONHAYNES_INDEXEDDB_SUPPORT_LIFECYCLEOBSERVABLE_H

#include <list>
#include "LifeCycleObserver.h"
#include "Container.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

class Transaction;

namespace Support {

///<summary>
/// This class represents an entity that implements the observer pattern; other classes may observe
/// it to determine if it contains a transaction that has been committed or aborted.  Observers
/// are also notified when this object is closed.
/// TODO: Pretty sure we can refactor out the transactional methods here; there's no need to chain these
///</summary>
template<class T>
class LifeCycleObservable
	{
	public:
		// Add a new observer to this observable class
		void addLifeCycleObserver(LifeCycleObserver<T>* const observer)
			{ observers.push_back(observer); }

		// Remove an observer from this observable class
		void removeLifeCycleObserver(LifeCycleObserver<T>* const observer)
			{ observers.remove(observer);}

	protected:
		// These methods are executed when the derived class wishes to fire the transaction event
		virtual void onTransactionCommitted(const Transaction& transaction) = 0;
		virtual void onTransactionAborted(const Transaction& transaction) = 0;

		// This method is called when the derived class is about to close; it notifies all 
		// observers of this fact
		void raiseOnCloseEvent()
			{ 
			for_each(observers.begin(), observers.end(), ExecuteCloseFunctor(static_cast<T*>(this))); 
			observers.clear();
			}

	private:
		// A list of our current observers
		// Would have prefered for these to be weak pointers, but there's no FireBreath compatible option here
		std::list<LifeCycleObserver<T>*> observers;

		// Functor that calls the onClose event on a given observer
		struct ExecuteCloseFunctor
			{
			T* const observable;
			ExecuteCloseFunctor(T* const observable) 
				: observable(observable) { }

			void operator ()(LifeCycleObserver<T>* const observer) 
				{ observer->onClose(observable); }
			};

		friend class Support::Container<T>;
	};

}
}
}
}

#endif