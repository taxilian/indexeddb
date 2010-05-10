/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_SUPPORT_LIFECYCLEOBSERVER_H
#define BRANDONHAYNES_INDEXEDDB_SUPPORT_LIFECYCLEOBSERVER_H

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 
namespace Support {

template<class T>
class LifeCycleObservable;

///<summary>
/// This interface represents a class capable of observing a LifeCycleObservable instance.  It requires
/// implementation of the onClose handler.
///</summary>
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