#include <cstring>
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
	sess->WndSize( 128, 128 );
	//sess->SetMtu( 1472 );
	sess->SetStreamMode( true );
	//sess->SetDSCP( 46 );

	assert( sess != nullptr );
	ssize_t nsent = 0;
	ssize_t nrecv = 0;
	ssize_t ntotal = 0;
	ssize_t nbuf = 2000;
	char *buf = ( char * ) malloc( nbuf );
	for ( int i = 0; i < 15361; i++ ) {
		memset( buf, 0, nbuf );
		sprintf( buf, "message:%d", i );
		auto sz = strlen( buf );
		sess->Write( buf, sz + 1 );
		sess->Update( iclock() );
		memset( buf, 0, nbuf );
		ssize_t n = 0;
		do {
			n = sess->Read( buf, nbuf );
			if ( n > 0 ) {
				ntotal += n;
				printf( "[%d]Receive %d, Total %d\n", i, n, ntotal );
			}
			///usleep( 200 );
			sess->Update( iclock() );
		} while ( n == 0 );
	}

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


