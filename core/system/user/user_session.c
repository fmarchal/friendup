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

/** @file
 *
 *  User Session
 *
 * file contain all functitons related to user sessions
 *
 *  @author PS (Pawel Stefanski)
 *  @date created 11/2016
 */

#include "user_session.h"
#include <util/string.h>

/**
 * Create new User Session
 *
 * @param sessid sessionID
 * @param devid deviceID
 * @return new UserSession structure when success, otherwise NULL
 */
UserSession *UserSessionNew( char *sessid, char *devid )
{
	UserSession *s;
	if( ( s = FCalloc( 1, sizeof(UserSession) ) ) != NULL )
	{
		s->us_SessionID = StringDuplicate( sessid );
		s->us_DeviceIdentity = StringDuplicate( devid );
		
		INFO("Mutex initialized\n");
		
		pthread_mutex_init( &s->us_WSMutex, NULL );
	}
	return s;
}

/**
 * Delete UserSession
 *
 * @param us pointer to UserSession which will be deleted
 */
void UserSessionDelete( UserSession *us )
{
	if( us != NULL )
	{
		us->us_WSConnections = NULL;
		
		 WebsocketClient *nwsc = us->us_WSConnections;
		 us->us_WSConnections = NULL;
		 int count = 0;
		 
		 // we must wait till all tasks will be finished
		 while( TRUE )
		 {
			 if( us->us_NRConnections <= 0 )
			{
				break;
			}
			else
			{
				DEBUG("number of connections: %d\n", us->us_NRConnections );
				count++;
				if( count > 50 )
				{
					//WorkerManagerDebug( SLIB );
					count = 0;
				}
			}
			usleep( 100000 );
		}
	
		DEBUG("Remove session %p\n", us );
		pthread_mutex_lock( &(us->us_WSMutex) );
		if( nwsc != NULL )
		{
			WebsocketClient *rws = nwsc;
			while( nwsc != NULL )
			{
				rws = nwsc;
				nwsc = (WebsocketClient *)nwsc->node.mln_Succ;
	
				DEBUG("Remove websockets\n");
				FCWSData *data = rws->wc_WebsocketsData;
				if( data != NULL )
				{
					data->fcd_ActiveSession = NULL;
					data->fcd_WSClient = NULL;
				}
				rws->wc_UserSession = NULL;
				FFree( rws );
			}
		}

		pthread_mutex_unlock( &(us->us_WSMutex) );
	
		DEBUG("Session released  sessid: %s device: %s \n", us->us_SessionID, us->us_DeviceIdentity );
	
		if( us->us_DeviceIdentity != NULL )
		{
			FFree( us->us_DeviceIdentity );
		}
	
		if( us->us_SessionID != NULL )
		{
			FFree( us->us_SessionID );
		}
	
		FFree( us );
	}
}
