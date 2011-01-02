/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_SUPPORT_LIFECYCLEOBSERVER_H
#define BRANDONHAYNES_INDEXEDDB_SUPPORT_LIFECYCLEOBSERVER_H

#include <boost/enable_shared_from_this.hpp>

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
class LifeCycleObserver : public boost::enable_shared_from_this<LifeCycleObserver<T> >
	{
	public:
		virtual void onClose(const boost::weak_ptr<T>& entity) = 0;

	protected:
		LifeCycleObserver() {}
	};

}
}
}
}

#endif