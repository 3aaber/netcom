#ifndef _MUTEX_H_
#define _MUTEX_H_

#ifdef _WIN32

#include <windows.h>

#else

#include <pthread.h>
#include <stdbool.h>

#endif

class TMUTEX {

protected:
#ifdef _WIN32
	typedef CRITICAL_SECTION MUTEX;
#else
	typedef pthread_mutex_t MUTEX;
#endif

	MUTEX Lock;

private:
	bool MUTEX_INIT();

public:
	TMUTEX() {
		MUTEX_INIT();
	}
	
	bool MUTEX_LOCK();
	bool MUTEX_UNLOCK();
	bool MUTEX_ISLOCK();
};

class TCONDITIONAL_WAIT :protected TMUTEX{

#ifdef _WIN32

	enum THType{
		SIGNAL = 0,
		BROADCAST = 1,
		MAX_EVENTS = 2
	};

	typedef struct {
		HANDLE events_[MAX_EVENTS];
	  // Signal and broadcast event HANDLEs.
	} pthread_cond_t;

#endif

	pthread_cond_t Cond;

#ifdef _WIN32
	int pthread_cond_init (pthread_cond_t *cv, const void *);
	int pthread_cond_wait (pthread_cond_t *cv, MUTEX *external_mutex);
	int pthread_cond_timedwait (pthread_cond_t *cv, MUTEX *external_mutex,int time_ms);
	int pthread_cond_signal (pthread_cond_t *cv);
#endif

	bool COND_VAR_INIT();

public:
	TCONDITIONAL_WAIT() {
		COND_VAR_INIT();
	}

	bool CONDITIONAL_WAIT();
	bool CONDITIONAL_TIMEDWAIT(int time_ms);
	bool CONDITION_SIGNAL();

};

#endif //_MUTEX_H_
