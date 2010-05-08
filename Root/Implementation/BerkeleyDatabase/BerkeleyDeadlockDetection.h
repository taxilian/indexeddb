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
	class BerkeleyDeadlockDetection
		{
		public:
			BerkeleyDeadlockDetection(DbEnv& environment, int millisecondsBetweenDetection)
				: isRunning(false), 
				  environment(environment), 
				  millisecondsBetweenDetection(millisecondsBetweenDetection)
				{ }

			~BerkeleyDeadlockDetection()
				{ stop(); }

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
			std::auto_ptr<boost::thread> deadlockDetectionThread;
			boost::mutex synchronized;
			DbEnv& environment;
			volatile bool isRunning;
			const int millisecondsBetweenDetection;
		    
			void detectDeadlocks()
				{
				while(isRunning)
					{
					environment.lock_detect(0, DB_LOCK_DEFAULT, NULL);
					boost::this_thread::sleep(boost::posix_time::milliseconds(millisecondsBetweenDetection));
					}
				}
		};
	}
}
}
}

#endif