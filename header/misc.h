#ifndef _MISC_H_
#define _MISC_H_

#include <sys/timeb.h>
#include <time.h>
#include <stdio.h>

#define DBG_LEVEL 1

#ifdef _WIN32

#include <windows.h>
#include <process.h>
#include <sys/types.h>
#define __func__ __FUNCTION__
#define usleep(x); Sleep((x)/1000);
//typedef BOOL bool;
typedef LPTHREAD_START_ROUTINE TThreadFunc;
typedef HANDLE TThreadID;

#else
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/time.h>
#define GetLastError() (errno)
typedef pthread_t TThreadID;
typedef void*(*TThreadFunc)(void*) ;

#endif

#ifdef _WIN32
#define swap_l(X) ntohl(X)
#define swap_s(X) ntohs(X)
#else
#define swap_l(X) __bswap_32(X) 
#define swap_s(X) __bswap_16(X)
#endif
#define swap_ll(x)  ((uint64_t)((((uint64_t)(x) & 0x00000000000000ffULL) << 56) | \
	(((uint64_t)(x) & 0x000000000000ff00ULL) << 40) | \
	(((uint64_t)(x) & 0x0000000000ff0000ULL) << 24) | \
	(((uint64_t)(x) & 0x00000000ff000000ULL) <<  8) | \
	(((uint64_t)(x) & 0x000000ff00000000ULL) >>  8) | \
	(((uint64_t)(x) & 0x0000ff0000000000ULL) >> 24) | \
	(((uint64_t)(x) & 0x00ff000000000000ULL) >> 40) | \
	(((uint64_t)(x) & 0xff00000000000000ULL) >> 56)))

#define KNRM  "\x1B[0m" 	//reset; clears all colors and styles (to white on black)
#define KBLD  "\x1B[1m" 	//bold on (see below)
#define KITL  "\x1B[3m" 	//italics on
#define KUND  "\x1B[4m" 	//underline on
#define KBLINK  "\x1B[5m" 	//blink on
#define KINV  "\x1B[7m" 	//inverse on; reverses foreground & background colors
#define KSTR  "\x1B[9m" 	//strike through on
#define KBLDO  "\x1B[22m" 	//bold off (see below)
#define KITLO  "\x1B[23m" 	//italics off
#define KUNDO  "\x1B[24m" 	//underline off
#define KBLINKO  "\x1B[25m" 	//blink off
#define KINVO  "\x1B[27m" 	//inverse off
#define KSTRO  "\x1B[29m" 	//strike through off
#define KBLK  "\x1B[30m" 	//set foreground color to black
#define KRED  "\x1B[31m"	//set foreground color to red
#define KGRN  "\x1B[32m"	//set foreground color to green
#define KYEL  "\x1B[33m"	//set foreground color to yellow
#define KBLU  "\x1B[34m"	//set foreground color to blue
#define KMAG  "\x1B[35m"	//set foreground color to magenta
#define KCYN  "\x1B[36m"	//set foreground color to cyan
#define KWHT  "\x1B[37m"	//set foreground color to white
#define KDEF  "\x1B[39m" 	//set foreground color to default (white)
#define KBGBLK  "\x1B[40m" 	//set background color to black
#define KBGRED  "\x1B[41m"	//set background color to red
#define KBGGRN  "\x1B[42m" 	//set background color to green
#define KBGYEL  "\x1B[43m" 	//set background color to yellow
#define KBGBLU  "\x1B[44m" 	//set background color to blue
#define KBGMAG  "\x1B[45m" 	//set background color to magenta (purple)
#define KBGCYN  "\x1B[46m" 	//set background color to cyan
#define KBGWHT  "\x1B[47m" 	//set background color to white
#define KBGDEF  "\x1B[49m" 	//set background color to default (black)

#if DBG_LEVEL == 0
#	define printdbg(x,...) 			do { } while (0)
#	define printdbg2(x,...) 		do { } while (0)
#	define printdbg3(x,...) 		do { } while (0)
#elif DBG_LEVEL == 1
#	define printdbg(x,...) 			printf(x,##__VA_ARGS__)
#	define printdbg2(x,...) 		do { } while (0)
#	define printdbg3(x,...) 		do { } while (0)
#elif DBG_LEVEL == 2
#	define printdbg(x,...) 			printf(x,##__VA_ARGS__)
#	define printdbg2(x,...) 		printf(x,##__VA_ARGS__)
#	define printdbg3(x,...) 		do { } while (0)
#elif DBG_LEVEL == 3
#	define printdbg(x,...) 			printf(x,##__VA_ARGS__)
#	define printdbg2(x,...) 		printf(x,##__VA_ARGS__)
#	define printdbg3(x,...) 		printf(x,##__VA_ARGS__)
#endif



void writeToIODevice(FILE* device, const char* message);


/*!
* Handles an error. Could include some trace & debug helpful routines.
* 
* @param[in] function Pointer to the function name string.
* @param[in] message Pointer to the message string.
* @param[in] line Line number whitch error has occured in
* @param[in] errcode Error code 
*/
void TraceErrorHandler(char const*function,char const*message ,int line,int errcode);

/*!
* /brief Handles a trace message. Could be controlled by defining trace level macros.
* 
* /param[in] function Pointer to the function name string.
* /param[in] message Tointer to the message string.
* /param[in] line Line number whitch error has occured in
*/
void TraceMessageHandler(char const*function,int line,char *message );

/*!
* Creates a thread and runs the function pointed by func in it.
* 
* @param[out] thread The thread handle or ID if the thread started successfully.
* @param[in] func Pointer to the function whitch should be run.
* @param[in] args parameters which will be passed to the invoking function
* 
* \return false in case of any error
*/
bool ThreadStart(TThreadID *thread,TThreadFunc func,void *args);

/*!
* Template Class For Easily Making Threads From Class Member Functions in Linux and Windows 
* 
*/
template<class thclass>
class TThread
{


#ifdef _WIN32
	typedef	DWORD (WINAPI thclass::* TPThreadMember)(LPVOID);	
#else
	typedef void* (thclass::* TPThreadMember)(void*);
#endif

	typedef struct {
		thclass *objptr;
		TPThreadMember memberfunptr;
		void *params;
	}TThreadParamHelper;

	typedef enum {
		BelowNormal,
		Normal,
		UpperNormal
	}TThreadPriority;

#ifdef _WIN32
	static 
		DWORD WINAPI ThreadRun(LPVOID member){
			DWORD result;
#else 
	static 
		void *ThreadRun(void *member){
			void *result;
#endif		
			TThreadParamHelper *helperptr = (TThreadParamHelper*)member;
			result =  ((*(helperptr->objptr)).*(helperptr->memberfunptr))(helperptr->params);
			delete helperptr;
			return result;
	}

public:

	/*!
	* Starts a specified method from the specified object in an other thread
	* The thread method must be in the same interface with the TPThreadMember
	* 
	* @param[in] obj pointer to the object which the thread method is in.
	* @param[in] thmethod pointer to the method which will be run in a thread.
	* 
	* \return Handle to the created thread
	*/
	static TThreadID ThreadStart(thclass *obj,TPThreadMember thmethod,void* params = NULL,TThreadPriority prio = Normal)
	{
		TThreadID thid;
		TThreadParamHelper *pthparam = new TThreadParamHelper;
		pthparam->objptr = obj;
		pthparam->memberfunptr = thmethod;
		pthparam->params = params;

		::ThreadStart(&thid,ThreadRun,(void*)pthparam);

//		sched_param schedparams;
//		int priority;
//		switch(prio)
//		{
//		case Normal:
//			priority = sched_get_priority_max(SCHED_FIFO);
//			priority = ( priority + sched_get_priority_min(SCHED_FIFO) ) /2;
//			schedparams.sched_priority = priority;
//			pthread_setschedparam(thid, SCHED_FIFO, &schedparams);
//			break;
//		case BelowNormal:
//			priority = sched_get_priority_min(SCHED_RR);
//			schedparams.sched_priority = priority;
//			pthread_setschedparam(thid, SCHED_RR, &schedparams);
//
//			break;
//		case UpperNormal:
//			break;
//		}

		return thid;
	}
};

unsigned long long int GetTimeInMs();

unsigned long long int GetTimeInUs();

void set_realtime_priority(char const*);

void CalculateMD5(char* filename,unsigned char* md5buff);

void print_md5_sum(unsigned char* md);

void PrintProcessID(const char *ProccessName, int ProccessID);


#endif //_MISC_H_
