/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_SUPPORT_CONTAINER_H
#define BRANDONHAYNES_INDEXEDDB_SUPPORT_CONTAINER_H

#include <list>
#include <boost/thread/mutex.hpp>
#include "LifeCycleObserver.h"

using boost::mutex;
using boost::lock_guard;

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

class Transaction;

namespace Support {

template<class T>
class Container : LifeCycleObserver<T>
	{
	public:
			void add(T* const child) 
				{ 
				lock_guard<mutex> guard(synchronization);
				children.push_back(child); 
				child->addLifeCycleObserver(this);
				}

			template<class Predicate>
			void remove(Predicate& predicate) 
				{ 
				lock_guard<mutex> guard(synchronization);
				children.remove_if(predicate); 
				}
			
			void remove(T* const element) 
				{
				lock_guard<mutex> guard(synchronization);
				entity->removeLifeCycleObserver(this);
				entity->close();
				children.remove(element); 
				}

			void remove(const std::string& name) 
				{ remove(RemovalFunctor(this, name)); }

			void release()
				{ 
				lock_guard<mutex> guard(synchronization);
				for_each(children.begin(), children.end(), CloseFunctor(this));
				children.clear(); 
				}

			virtual void raiseTransactionCommitted(const Transaction& transaction)
				{ for_each(children.begin(), children.end(), CommitFunctor(transaction)); }
			virtual void raiseTransactionAborted(const Transaction& transaction)
				{ for_each(children.begin(), children.end(), AbortFunctor(transaction)); }

	private:
		std::list<T* const> children;

		virtual void onClose(T* const entity)
			{ 
			children.remove(entity); 
			}

		struct RemovalFunctor : public std::unary_function<T* const, bool>
			{
			const std::string& name;
			LifeCycleObserver<T>* const observer;

			RemovalFunctor(LifeCycleObserver<T>* const observer, const std::string& name): 
				observer(observer), name(name) { }

			bool operator ()(T* const entity)
				{ if(name == entity->getName()) { 
					entity->removeLifeCycleObserver(observer);
					entity->close(); 
					return true; 
				}
				  else return false; }
			};

		struct CloseFunctor
			{
			LifeCycleObserver<T>* const observer;

			CloseFunctor(LifeCycleObserver<T>* const observer) 
				: observer(observer) { }
			
			void operator ()(T* const entity) 
				{ 
				entity->removeLifeCycleObserver(observer);
				entity->close(); 
				}
			};

		struct CommitFunctor
			{
			const Transaction& transaction;

			CommitFunctor(const Transaction& transaction) 
				: transaction(transaction) { }
			
			void operator ()(LifeCycleObservable<T>* const entity) 
				{ entity->onTransactionCommitted(transaction); }
			};

		struct AbortFunctor
			{
			const Transaction& transaction;

			AbortFunctor(const Transaction& transaction) 
				: transaction(transaction) { }
			
			void operator ()(LifeCycleObservable<T>* const entity) 
				{ entity->onTransactionAborted(transaction); }
			};

	private:
		boost::mutex synchronization;
	};

}
}
}
}

#endif