/**********************************************************\
Copyright Richard Bateman
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include <boost/type_traits/is_base_of.hpp>

namespace BrandonHaynes {
    namespace IndexedDB { 
        namespace API { 
            namespace Support {
                // Hide implementation details from the class, use forwarding
                template <class T>
                class _privateObservable : public Support::LifeCycleObservable<T> {
                    BOOST_STATIC_ASSERT((boost::is_base_of<FB::JSAPI, T>::value));
                private:
                    T* parent;

                public:
                    _privateObservable(T* parent);
                    ~_privateObservable();
                    void invalidate();

                    virtual boost::shared_ptr<T> get_object() {
                        if (parent) {
                            return FB::ptr_cast<T>(parent->shared_from_this());
                        } else {
                            return boost::shared_ptr<T>();
                        }
                    }

                    virtual void onTransactionCommitted(const TransactionPtr& transaction);
                    virtual void onTransactionAborted(const TransactionPtr& transaction);
                };

                // The idea is that this holds a reference to the parent until the parent goes
                // away, in which case the parent *must* notify this class by calling invalidate.
                template <class T>
                _privateObservable<T>::_privateObservable(T* parent) : parent(parent)
                {}

                template <class T>
                void _privateObservable<T>::invalidate()
                {
                    parent = NULL;
                }

                template <class T>
                _privateObservable<T>::~_privateObservable()
                {}

                template <class T>
                void _privateObservable<T>::onTransactionCommitted( const TransactionPtr& transaction )
                {
                    if (parent)
                        parent->onTransactionCommitted(transaction);
                }

                template <class T>
                void _privateObservable<T>::onTransactionAborted( const TransactionPtr& transaction )
                {
                    if (parent)
                        parent->onTransactionAborted(transaction);
                }
            }
        }
    }
}