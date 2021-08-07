#include <cstring>
#include <iostream>
#include <cstdio>
#include "sess.h"
#include "cominclude.h"

IUINT32 iclock();

int main() {
	long s, u;
	itimeofday( &s, &u );
	srand( ( s * 1000 ) + ( u / 1000 ) );

	UDPSession *sess = UDPSession::DialWithOptions( "127.0.0.1", 9999, 10, 3 );
	sess->NoDelay( 1, 10, 2, 1 );
	sess->WndSize( 1024, 1024 );
	//sess->SetMtu( 1472 );
	sess->SetStreamMode( true );
	//sess->SetDSCP( 46 );

	assert( sess != nullptr );
	ssize_t nsent = 0;
	ssize_t nrecv = 0;
	ssize_t nbuf = 4096;
	ssize_t ntotal = 0;
	char *buf = ( char * ) malloc( nbuf );
	sprintf( buf, "Hello" );
	auto sz = strlen( buf );
	sess->Write( buf, sz );
	auto clkBegin = clock();
	int i = 0;
	while ( true ) {
		sess->Update( iclock() );
		memset( buf, 0, nbuf );
		int n = sess->Read( buf, nbuf );
		if ( n > 0 ) {
			ntotal += n;
			printf( "[%d]Receive %d, Total %d\n", i, n, ntotal );
			if ( strcmp( buf, "finish" ) == 0 ) {
				break;
			}
		}
		i++;
	}
	auto clkEnd = clock();
	std::cout << "Cost Time " << clkEnd - clkBegin << " ms" << std::endl;
	UDPSession::Destroy( sess );
}

IUINT64 iclock64( void ) {
	long s, u;
	IUINT64 value;
	itimeofday( &s, &u );
	value = ( ( IUINT64 ) s ) * 1000 + ( u / 1000 );
	return value;
}

IUINT32 iclock() {
	return ( IUINT32 ) ( iclock64() & 0xfffffffful );
}


