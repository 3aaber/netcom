

#include "../header/mutex.h"
#include "../header/misc.h"

bool TMUTEX::MUTEX_INIT() {	

#ifdef _WIN32
	if (!InitializeCriticalSectionAndSpinCount(&Lock, 0x80000400) ) {
		TraceErrorHandler(__func__,"Error Creating Mutex",__LINE__,GetLastError());
		return false;	
	}

#else
	int errcode;
//	if( (errcode = pthread_mutexattr_setprotocol(&Lock,PTHREAD_PRIO_NONE)) != 0 )
//	{
//		TraceErrorHandler(__func__,"Error Creating Mutex",__LINE__,errcode);
//		return false;
//	}
	if( (errcode = pthread_mutex_init (&Lock, NULL) ) != 0) {
		TraceErrorHandler(__func__,"Error Creating Mutex",__LINE__,errcode);
		return false;	
	}
#endif
    return true;
}


bool TMUTEX::MUTEX_LOCK() {
#ifdef _WIN32
	EnterCriticalSection(&Lock); 
#else
	int errcode;
	if( (errcode = pthread_mutex_lock( &Lock ) ) != 0 ) {
		TraceErrorHandler(__func__,"Error Locking Mutex",__LINE__,errcode);
		return false;	
	}
#endif
    return true;
}

bool TMUTEX::MUTEX_UNLOCK()
{
#ifdef _WIN32
	LeaveCriticalSection( &Lock );
#else
	int errcode;
	if( (errcode = pthread_mutex_unlock( &Lock )) != 0 ) {
		TraceErrorHandler(__func__,"Error Unlocking Mutex",__LINE__,errcode);
		return false;	
	}
#endif
    return true;
}

bool TMUTEX::MUTEX_ISLOCK()
{
#ifndef _WIN32
	return(pthread_mutex_trylock(&Lock) == EBUSY);
#else
	return !TryEnterCriticalSection(&Lock);
#endif
}


#ifdef _WIN32
int TCONDITIONAL_WAIT::pthread_cond_init (pthread_cond_t *cv, const void *)
{
	// Create an auto-reset event.
	cv->events_[SIGNAL] = CreateEvent (NULL,  // no security
                                     FALSE, // auto-reset event
                                     FALSE, // non-signaled initially
                                     NULL); // unnamed

	// Create a manual-reset event.
	cv->events_[BROADCAST] = CreateEvent (NULL,  // no security
                                        TRUE,  // manual-reset
                                        FALSE, // non-signaled initially
                                        NULL); // unnamed

	if( (cv->events_[SIGNAL] == NULL) || (cv->events_[BROADCAST] == NULL) ) {
		return GetLastError();
	}

	return 0;
}


int TCONDITIONAL_WAIT::pthread_cond_wait (pthread_cond_t *cv, MUTEX *external_mutex)
{
  // Release the <external_mutex> here and wait for either event
  // to become signaled, due to <pthread_cond_signal> being
  // called or <pthread_cond_broadcast> being called.
  LeaveCriticalSection (external_mutex);
  WaitForMultipleObjects (2, // Wait on both <events_>
                          cv->events_,
                          FALSE, // Wait for either event to be signaled
                          INFINITE); // Wait "forever"

  // Reacquire the mutex before returning.
  EnterCriticalSection (external_mutex);
  return 0;
}

int TCONDITIONAL_WAIT::pthread_cond_timedwait (pthread_cond_t *cv, MUTEX *external_mutex,int time_ms)
{
  // Release the <external_mutex> here and wait for either event
  // to become signaled, due to <pthread_cond_signal> being
  // called or <pthread_cond_broadcast> being called.
  LeaveCriticalSection (external_mutex);
  WaitForMultipleObjects (2, // Wait on both <events_>
                          cv->events_,
                          FALSE, // Wait for either event to be signaled
                          time_ms); // Wait "forever"

  // Reacquire the mutex before returning.
  EnterCriticalSection (external_mutex);
  return 0;
}

int TCONDITIONAL_WAIT::pthread_cond_signal (pthread_cond_t *cv)
{
	// Try to release one waiting thread.
	if( !SetEvent (cv->events_[SIGNAL]) ) {
		return GetLastError();
	}
	return 0;

}
#endif 

bool TCONDITIONAL_WAIT::COND_VAR_INIT() {

	int errcode;
	if( (errcode = pthread_cond_init( &Cond, NULL )) != 0 ) {
		TraceErrorHandler(__func__,"Error Initialize Condition",__LINE__,errcode);
		return false;	
	}
	return true;

}


bool TCONDITIONAL_WAIT::CONDITIONAL_WAIT() {

	int errcode;
	MUTEX_LOCK();
	if( (errcode = pthread_cond_wait(&Cond,&Lock) ) != 0 ) {
		TraceErrorHandler(__func__,"Error Conditional Wait",__LINE__,errcode);
		MUTEX_UNLOCK();
		return false;	
	}
	MUTEX_UNLOCK();
	return true;

}

bool TCONDITIONAL_WAIT::CONDITIONAL_TIMEDWAIT(int time_ms) {
	MUTEX_LOCK();
	#ifdef _WIN32

	int errcode;
	if( (errcode = pthread_cond_timedwait(&Cond,&Lock,time_ms) ) != 0 ) {
		TraceErrorHandler(__func__,"Error Conditional Wait",__LINE__,errcode);
		MUTEX_UNLOCK();
		return false;	
	}

#else

	int errcode;
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME,&ts);
	ts.tv_nsec += ( (unsigned long int)time_ms * (1000 * 1000) );
	if( ts.tv_nsec > (1000*1000*1000) ) {
		ts.tv_sec +=1;
		ts.tv_nsec %= (1000*1000*1000);
	}
	errcode = pthread_cond_timedwait(&Cond,&Lock,&ts);
	if( (errcode != 0 ) && (errcode != ETIMEDOUT) ) {
		TraceErrorHandler(__func__,"Error Conditional Wait",__LINE__,errcode);
		MUTEX_UNLOCK();
		return false;	
	}

#endif
	MUTEX_UNLOCK();
	return true;

}

bool TCONDITIONAL_WAIT::CONDITION_SIGNAL() {

	int errcode;
	if( (errcode = pthread_cond_signal(&Cond)) != 0) {
		TraceErrorHandler(__func__,"Error Signalling Condition",__LINE__,errcode);
		return false;
	}

	return true;
}


