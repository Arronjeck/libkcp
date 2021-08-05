#ifndef __KCP_CPP_H__
#define __KCP_CPP_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <array>
#include <memory>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <stdexcept>
#include <string>
#include <utility>
//#include <thread>

#include "ikcp.h"

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
using ssize_t = size_t;
using ioctl_setting = u_long;
static inline int gettimeofday( struct timeval
								*tp, void *tzp ) {
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime( &wtm );
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime( &tm );
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;
	return ( 0 );
}

static inline void itimeofday( long *sec, long *usec ) {
	static long mode = 0, addsec = 0;
	BOOL retval;
	static IINT64 freq = 1;
	IINT64 qpc;
	if ( mode == 0 ) {
		retval = QueryPerformanceFrequency( ( LARGE_INTEGER * )&freq );
		freq = ( freq == 0 ) ? 1 : freq;
		retval = QueryPerformanceCounter( ( LARGE_INTEGER * )&qpc );
		addsec = ( long )time( NULL );
		addsec = addsec - ( long )( ( qpc / freq ) & 0x7fffffff );
		mode = 1;
	}
	retval = QueryPerformanceCounter( ( LARGE_INTEGER * )&qpc );
	retval = retval * 2;
	if ( sec ) {
		*sec = ( long )( qpc / freq ) + addsec;
	}
	if ( usec ) {
		*usec = ( long )( ( qpc % freq ) * 1000000 / freq );
	}
}

static inline void usleep( int iMilliSec )
{
	Sleep( iMilliSec );
	//std::this_thread::sleep_for( std::chrono::milliseconds( iMilliSec ) );
}

// taken from: https://github.com/rxi/dyad/blob/915ae4939529b9aaaf6ebfd2f65c6cff45fc0eac/src/dyad.c#L58
inline const char *inet_ntop( int af,
							  const void *src, char *dst, socklen_t size )
{
	union {
		struct sockaddr sa;
		struct sockaddr_in sai;
		struct sockaddr_in6 sai6;
	} addr;
	int res;
	memset( &addr, 0, sizeof( addr ) );
	addr.sa.sa_family = af;
	if ( af == AF_INET6 )
	{
		memcpy( &addr.sai6.sin6_addr, src,
				sizeof( addr.sai6.sin6_addr ) );
	}
	else
	{
		memcpy( &addr.sai.sin_addr, src,
				sizeof( addr.sai.sin_addr ) );
	}
	res = WSAAddressToStringA( &addr.sa,
							   sizeof( addr ), 0, dst,
							   reinterpret_cast<LPDWORD>( &size ) );
	if ( res != 0 ) {
		return NULL;
	}
	return dst;
}

class WSANet
{
private:
	///Prevent copy construct
	WSANet( const WSANet & ) = delete;
	///Prevent copy assignment
	WSANet &operator=( const WSANet & ) = delete;
	///Prevent moving
	WSANet( WSANet && ) = delete;
	///Prevent move assignment
	WSANet &operator=( WSANet && ) = delete;
public:
	WSANet():
		wsa_data()
	{
		const auto status = WSAStartup( MAKEWORD( 2, 2 ), &wsa_data );
		if ( status != 0 )
		{
			std::string error_message;
			switch ( status ) // https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup#return-value
			{
				default:
					error_message = "Unknown error happened.";
					break;
				case WSASYSNOTREADY:
					error_message =
						"The underlying network subsystem is not ready for network communication.";
					break;
				case WSAVERNOTSUPPORTED: //unlikely, we specify 2.2!
					error_message =
						" The version of Windows Sockets support requested "
						"(2.2)" //we know here the version was 2.2, add that to the error message copied from MSDN
						" is not provided by this particular Windows Sockets implementation. ";
					break;
				case WSAEINPROGRESS:
					error_message =
						"A blocking Windows Sockets 1.1 operation is in progress.";
					break;
				case WSAEPROCLIM:
					error_message =
						"A limit on the number of tasks supported by the Windows Sockets implementation has been reached.";
					break;
				case WSAEFAULT: //unlikely, if this ctor is running, wsa_data is part of this object's "stack" data
					error_message =
						"The lpWSAData parameter is not a valid pointer.";
					break;
			}
		}
	}

	~WSANet()
	{
		WSACleanup();
	}
private:
	WSADATA wsa_data;
};

static WSANet instance;

#elif !defined(__unix)
#define __unix
#endif

#ifdef __unix
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
inline void itimeofday( long *sec, long *usec ) {
	struct timeval time;
	gettimeofday( &time, NULL );
	if ( sec ) {
		*sec = time.tv_sec;
	}
	if ( usec ) {
		*usec = time.tv_usec;
	}
}
#endif

#endif //__KCP_CPP_H__