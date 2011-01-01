/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_SUPPORT_CONTAINER_H
#define BRANDONHAYNES_INDEXEDDB_SUPPORT_CONTAINER_H

#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
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
                    typedef boost::shared_ptr<LifeCycleObserver<T> > LifeCycleObserverPtr;
                    void add(const boost::weak_ptr<T>& child) 
                    { 
                        lock_guard<mutex> guard(synchronization);
                        children.push_back(child); 
                        child->addLifeCycleObserver(shared_from_this());
                    }

                    template<class Predicate>
                    void remove(const Predicate& predicate) 
                    { 
                        lock_guard<mutex> guard(synchronization);
                        children.remove_if(predicate); 
                    }

                    void remove(const boost::weak_ptr<T>& element) 
                    {
                        lock_guard<mutex> guard(synchronization);
                        entity->removeLifeCycleObserver(shared_from_this);
                        entity->close();
                        children.remove(element); 
                    }

                    void remove(const std::string& name) 
                    { remove(RemovalFunctor(shared_from_this(), name)); }

                    void release()
                    { 
                        lock_guard<mutex> guard(synchronization);
                        for_each(children.begin(), children.end(), CloseFunctor(shared_from_this()));
                        children.clear(); 
                    }

                    virtual void raiseTransactionCommitted(const TransactionPtr& transaction)
                    { for_each(children.begin(), children.end(), CommitFunctor(transaction)); }
                    virtual void raiseTransactionAborted(const TransactionPtr& transaction)
                    { for_each(children.begin(), children.end(), AbortFunctor(transaction)); }

                private:
                    std::list<boost::weak_ptr<T> > children;

                    struct removeWeakPtr : public std::unary_function<boost::weak_ptr<T>, bool>
                    {
                        removeWeakPtr(const boost::weak_ptr<T>& ptr) : inner(ptr) { }

                        boost::weak_ptr<T> inner;
                        bool operator()(const boost::weak_ptr<T>& b) {
                            return inner.lock() == b.lock();
                        }
                    };

                    virtual void onClose(const boost::weak_ptr<T>& entity)
                    { 
                        children.remove_if(removeWeakPtr(entity)); 
                    }

                    struct RemovalFunctor : public std::unary_function<boost::weak_ptr<T>, bool>
                    {
                        const std::string& name;
                        LifeCycleObserverPtr observer;

                        RemovalFunctor(const LifeCycleObserverPtr& observer, const std::string& name): 
                        observer(observer), name(name) { }

                        bool operator ()(const boost::weak_ptr<T>& en)
                        {
                            boost::shared_ptr entity(en.lock());
                            if(entity && name == entity->getName()) { 
                                entity->removeLifeCycleObserver(observer);
                                entity->close(); 
                                return true; 
                            } else if (!entity) {
                                return true;
                            } else {
                                return false;
                            }
                        }
                    };

                    struct CloseFunctor
                    {
                        LifeCycleObserverPtr const observer;

                        CloseFunctor(const LifeCycleObserverPtr& observer) 
                            : observer(observer) { }

                        void operator ()(const LifeCycleObserverPtr& entity) 
                        { 
                            entity->removeLifeCycleObserver(observer);
                            entity->close(); 
                        }
                    };

                    struct CommitFunctor
                    {
                        const TransactionPtr transaction;

                        CommitFunctor(const TransactionPtr& transaction) 
                            : transaction(transaction) { }

                        void operator ()(const LifeCycleObserverPtr& const entity) 
                        { entity->onTransactionCommitted(transaction); }
                    };

                    struct AbortFunctor
                    {
                        const TransactionPtr transaction;

                        AbortFunctor(const TransactionPtr& transaction) 
                            : transaction(transaction) { }

                        void operator ()(const LifeCycleObserverPtr& entity) 
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