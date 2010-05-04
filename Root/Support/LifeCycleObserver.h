/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_SUPPORT_LIFECYCLEOBSERVER_H
#define BRANDONHAYNES_INDEXEDDB_SUPPORT_LIFECYCLEOBSERVER_H

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 
namespace Support {

template<class T>
class LifeCycleObservable;

template<class T>
class LifeCycleObserver
	{
	public:
		virtual void onClose(T* const entity) = 0;

	protected:
		LifeCycleObserver() {}
	};

}
}
}
}

#endif