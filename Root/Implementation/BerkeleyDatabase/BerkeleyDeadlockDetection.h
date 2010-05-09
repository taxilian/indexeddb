/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYDEADLOCKDETECTION_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYDEADLOCKDETECTION_H

#include <db_cxx.h>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
namespace BerkeleyDB
	{
	///<summary>
	/// This class is spawned on a per-BerkeleyDatabase basis; it runs the Berkeley DB timeout detection method
	/// on an interval.  This class is fully managed by BerkeleyDatabase, including initiation and termination.
	/// (There is no reason that an external entity could not also utilize this class.)  This class is RAII.
	///</summary>
	class BerkeleyDeadlockDetection
		{
		public:
			// Create a deadlock detection thread for the given environment.  Timeouts are detected at the given interval.
			BerkeleyDeadlockDetection(DbEnv& environment, int millisecondsBetweenDetection)
				: isRunning(false), 
				  environment(environment), 
				  millisecondsBetweenDetection(millisecondsBetweenDetection)
				{ }

			~BerkeleyDeadlockDetection()
				{ stop(); }

			// Start deadlock detection on this thread
			void start()
				{ 
				boost::lock_guard<boost::mutex> guard(synchronized);
				if(!isRunning)
					{
					isRunning = true; 
					deadlockDetectionThread = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(
						&BerkeleyDeadlockDetection::detectDeadlocks, this)));
					}
				}

			// Conclude deadlock detection on this thread
			void stop()
				{
				boost::lock_guard<boost::mutex> guard(synchronized);
				if(isRunning)
					{
					isRunning = false;
					deadlockDetectionThread->join();
					}
				}

		private:
			// The thead associated with this class
			std::auto_ptr<boost::thread> deadlockDetectionThread;
			// Several operations need to be synchronized; we use a Boost mutex for this purpose
			boost::mutex synchronized;
			DbEnv& environment;
			// Indicates whether our thread is currently running
			volatile bool isRunning;
			const int millisecondsBetweenDetection;
		    
			// Method fired once every interval; detects timed out transactions and locks
			void detectDeadlocks()
				{
				// Automatically terminate whenever the isRunning flag is cleared
				while(isRunning)
					{
					// Execute our deadlock detection routine
					environment.lock_detect(0, DB_LOCK_DEFAULT, NULL);
					// Sleep for our interval (I believe this is interruptable, but should probably check)
					boost::this_thread::sleep(boost::posix_time::milliseconds(millisecondsBetweenDetection));
					}
				}
		};
	}
}
}
}

#endif