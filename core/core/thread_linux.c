/*©mit**************************************************************************
*                                                                              *
* This file is part of FRIEND UNIFYING PLATFORM.                               *
* Copyright 2014-2017 Friend Software Labs AS                                  *
*                                                                              *
* Permission is hereby granted, free of charge, to any person obtaining a copy *
* of this software and associated documentation files (the "Software"), to     *
* deal in the Software without restriction, including without limitation the   *
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or  *
* sell copies of the Software, and to permit persons to whom the Software is   *
* furnished to do so, subject to the following conditions:                     *
*                                                                              *
* The above copyright notice and this permission notice shall be included in   *
* all copies or substantial portions of the Software.                          *
*                                                                              *
* This program is distributed in the hope that it will be useful,              *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 *
* MIT License for more details.                                                *
*                                                                              *
*****************************************************************************©*/


#include <core/thread.h>
#include <util/log/log.h>
#include <pthread.h>

//
// create new thread
//

FThread *ThreadNew( void *func, void *data, FBOOL autos, pthread_attr_t *attr )
{
	if( !func || !data ) return NULL;
	
	FThread *nt = (FThread *)FCalloc( 1, sizeof( FThread ) );
	if( nt == NULL )
	{
		FERROR("[ThreadNew] Cannot allocate memory for Thread\n");
		return NULL;
	}
	int error = 0;

	nt->t_Function = func;
	nt->t_Data = data;
	nt->t_Launched = FALSE;
	
	//uuid_generate( nt->t_uuid );
	nt->t_pid = (FUQUAD)nt;
	
	//DEBUG("ThreadNew create thread func ptr %x\n", func );

	if( autos == TRUE )
	{
		nt->t_Quit = FALSE;
		
		if( ( error = pthread_create( &(nt->t_Thread), attr, func, nt ) ) == 0 )
		{
			nt->t_Launched = TRUE;
			// WE ALWAYS PASS POINTER TO THREAD AND ALLOW DEVELOPER TO HANDLE  quit
			//DEBUG("[ThreadNew] STARTED\n" );
		}
		else
		{
			FFree( nt );
			DEBUG("[ThreadNew] error: %d\n", error );
			return NULL;
		}
	}
	else
	{
		Log( FLOG_INFO, "Thread start delayed\n");
	}

	return nt;
}

//
// start thread
//

FThread *ThreadStart( FThread *ft )
{
	if( ft != NULL && ft->t_Launched == FALSE )
	{
		int error;
		
		ft->t_Quit = FALSE;
		
		if( ( error = pthread_create( &(ft->t_Thread), NULL, ft->t_Function, ft->t_Data ) ) == 0 )
		{
			ft->t_Launched = TRUE;
		}
		else
		{
			//free( ft );
			DEBUG("[ThreadNew] error: %d\n", error );
			return NULL;
		}
	}
	return ft;
}

//
// Stop currently working thread
//

void ThreadCancel( FThread *ft, FBOOL wait )
{
	pthread_cancel( ft->t_Thread );
	
	if( wait == TRUE && ft->t_Launched != FALSE )
	{
		pthread_join( ft->t_Thread, NULL);
	}
}

//
// example
//
/*
	void f( void *args )
	{
	struct FThread *ft = (FThread *)args;
	while( ft->quit != TRUE )
	{
		//...do something
	}
	
	ft = t_Ended = TRUE;

	}
*/


//
// remove thread
//

void ThreadDelete( FThread *t )
{
	if( t->t_Thread )
	{
		t->t_Quit = TRUE;
		
		DEBUG("[ThreadDelete] Asking thread %p to quit.\n", t );
		
		//if( t->t_Launched == TRUE )
		{
			pthread_join( t->t_Thread, NULL );
		}
		
		DEBUG("[ThreadDelete] Thread finished work (%p)..\n", t );
		
		FFree( t );
	}
}

//#endif // __LINUX__
