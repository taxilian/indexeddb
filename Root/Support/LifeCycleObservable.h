/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
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

template<class T>
class LifeCycleObservable
	{
	public:
		void addLifeCycleObserver(LifeCycleObserver<T>* const observer)
			{ observers.push_back(observer); }

		void removeLifeCycleObserver(LifeCycleObserver<T>* const observer)
			{ observers.remove(observer);}

	protected:
		virtual void onTransactionCommitted(const Transaction& transaction) = 0;
		virtual void onTransactionAborted(const Transaction& transaction) = 0;

		void raiseOnCloseEvent()
			{ 
			for_each(observers.begin(), observers.end(), ExecuteCloseFunctor(static_cast<T*>(this))); 
			observers.clear();
			}

	private:
		std::list<LifeCycleObserver<T>*> observers;

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