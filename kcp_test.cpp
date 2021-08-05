#include <cstring>
#include <cstdio>
#include "sess.h"

IUINT32 iclock();

int main() {
	long s, u;
	itimeofday( &s, &u );
	srand( ( s * 1000 ) + ( u / 1000 ) );

	UDPSession *sess = UDPSession::DialWithOptions( "127.0.0.1", 12345, 10, 3 );
	sess->NoDelay( 1, 20, 2, 1 );
	sess->WndSize( 128, 128 );
	sess->SetMtu( 1400 );
	sess->SetStreamMode( true );
	sess->SetDSCP( 46 );

	assert( sess != nullptr );
	ssize_t nsent = 0;
	ssize_t nrecv = 0;
	char *buf = ( char * ) malloc( 128 );

	for ( int i = 0; i < 10; i++ ) {
		sprintf( buf, "message:%d", i );
		auto sz = strlen( buf );
		sess->Write( buf, sz );
		sess->Update( iclock() );
		memset( buf, 0, 128 );
		ssize_t n = 0;
		do {
			n = sess->Read( buf, 128 );
			if ( n > 0 ) {
				printf( "%s\n", buf );
			}
			usleep( 1000 );
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


