/*
* (C) 2014 Mike Tzou
*
* This file is part of "Simple http server".
*
* "Mask calcuator" is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* "Mask calcuator" is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*/

///// headers  /////
#include <cstdio>
#include <string>
#include <cstdlib>
#include <cstring>

#include <errno.h>
#include <arpa/inet.h>

#include "microhttpd.h"


///// global variables  /////
static const char IF_IP[] = "192.168.123.141";
static const unsigned int PORT = 3123;
static const unsigned int FLAGS = MHD_USE_EPOLL_INTERNALLY_LINUX_ONLY;  // MHD_USE_DUAL_STACK

static const unsigned int IP_LIMIT = 5;
static const unsigned int TIMEOUT = 30;
static const unsigned int CON_LIMIT = 60;

static const std::string FILE_PATH = "./";
static const std::string FILE_NAME = "wpad.dat";


///// function prototypes  /////
static void readStaticFile( const char *path , char **data );
static int myHandler( void *arg, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **ptr );

static void prepareStaticResp( const char *path , struct MHD_Response **r );


///// functions  /////
int main()
{
	// prepare static data
	struct MHD_Response *r;
	prepareStaticResp( ( FILE_PATH + FILE_NAME ).c_str() , &r );

	// create IPv4 socket
	struct sockaddr_in serv_addr = {0};                            // Init & clear struct
	serv_addr.sin_family = AF_INET;                                // Internet/IP
	int err = inet_pton( AF_INET , IF_IP , &serv_addr.sin_addr );  // Incoming addr
	if( err < 0 )
	{
		printf( "inet_pton(), %d error: %d, %s\n" , err , errno , strerror( errno ) );
		exit( EXIT_FAILURE );
	}
	serv_addr.sin_port = htons( PORT );                            // server port

	// start daemon
	struct MHD_Daemon *d = MHD_start_daemon( FLAGS , PORT , \
						   NULL , NULL , \
						   &myHandler , ( void * ) r , \
						   MHD_OPTION_CONNECTION_LIMIT , CON_LIMIT , \
						   MHD_OPTION_CONNECTION_TIMEOUT , TIMEOUT , \
						   MHD_OPTION_PER_IP_CONNECTION_LIMIT , IP_LIMIT , \
						   MHD_OPTION_SOCK_ADDR , &serv_addr , \
						   MHD_OPTION_END );
	if( d == NULL )
	{
		printf( "MHD_start_daemon(), error\n" );
		exit( EXIT_FAILURE );
	}

	// main thread sleep forever
	err = pause();
	if( err == -1 )
	{
		printf( "pause(), %d error: %d, %s\n" , err , errno , strerror( errno ) );
	}

	// shutdown
	MHD_destroy_response( r );
	MHD_stop_daemon( d );
	printf( "main thread exit\n" );
	return 0;
}


static int myHandler( void *arg, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **ptr )
{
	// only handle 'GET' request
	if( strncmp( method, "GET" , 3 ) != 0 )
	{
		printf( "request is not GET, is: %s\n" , method );
		return MHD_NO;
	}

	// strict url matching
	std::string str( "/" );
	str.append( FILE_NAME );
	const size_t max_len = ( strlen( url ) > str.length() ) ? strlen( url ) : str.length();
	if( strncmp( url , str.c_str() , max_len ) != 0 )
	{
		printf( "unexpected url: %s\n" , url );
		return MHD_NO;
	}

	// request is OK, handle it
	const int ret = MHD_queue_response( connection, MHD_HTTP_OK, ( struct MHD_Response * ) arg );
	return ret;
}


static void prepareStaticResp( const char *path , struct MHD_Response **r )
{
	static char *static_data;
	readStaticFile( path , &static_data );

	*r = MHD_create_response_from_buffer( strlen( static_data ), static_data , MHD_RESPMEM_MUST_FREE );
	if( *r == NULL )
	{
		printf( "MHD_create_response_from_buffer(), error\n" );
		exit( EXIT_FAILURE );
	}

	const int err = MHD_add_response_header( *r , "Content-Type" , "application/x-ns-proxy-autoconfig" );
	if( err == MHD_NO )
	{
		printf( "MHD_add_response_header(), error\n" );
		exit( EXIT_FAILURE );
	}
	return;
}


static void readStaticFile( const char *path , char **data )
{
	// open file
	FILE *f = fopen( path , "rb" );
	if( f == NULL )  // error occurs
	{
		printf( "fopen(), %s error: %d, %s\n" , path , errno , strerror( errno ) );
		exit( EXIT_FAILURE );
	}

	// get file length
	fseek( f , 0 , SEEK_END );
	const long int len = ftell( f ) + 1;  // +1 for null token
	fseek( f , 0 , SEEK_SET );

	// allocate space
	*data = ( char * )malloc( len );

	// read data
	fread( *data , 1 , len , f );
	const int err = feof( f );
	fclose( f );
	if( err == 0 )
	{
		printf( "fread(), error: didn't read until eof\n" );
		exit( EXIT_FAILURE );
	}

	return;
}
