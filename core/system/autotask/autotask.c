/*©mit**************************************************************************
 *                                                                              *
 * Friend Unifying Platform                                                     *
 * ------------------------                                                     *
 *                                                                              * 
 * Copyright 2014-2016 Friend Software Labs AS, all rights reserved.            *
 * Hillevaagsveien 14, 4016 Stavanger, Norway                                   *
 * Tel.: (+47) 40 72 96 56                                                      *
 * Mail: info@friendos.com                                                      *
 *                                                                              *
 *****************************************************************************©*/
/** @file
 * 
 *  Auto task
 *
 * Threads started with FC
 *
 *  @author PS (Pawel Stefanski)
 *  @date created 11/2016
 */
#include "autotask.h"
#include <system/systembase.h>

/**
 * Create new Autotask
 *
 * @param command command which will be executed
 * @param arguments used by command 
 * @return new Autotask structure when success, otherwise NULL
 */
Autotask *AutotaskNew( char *command, char *arguments )
{
	Autotask *at;
	if( ( at = FCalloc( 1, sizeof( Autotask ) ) ) != NULL )
	{
		at->at_Command = StringDuplicate( command );
		at->at_Arguments = StringDuplicate( arguments );
		
		char *args[ 3 ];
		args[ 0 ] = at->at_Command;
		args[ 1 ] = at->at_Arguments;
		args[ 2 ] = NULL;
		
		int pid = fork();
		if( pid == 0 )
		{
			at->at_Launched = TRUE;
			int val = execv( at->at_Command, args );
			DEBUG("Autostart command returned %d\n", val );
			
			at->at_Launched = FALSE;
		}
		else if( pid < 0 )
		{
			FERROR("Cannot launch script: %s %s\n", at->at_Command, at->at_Arguments );
		}
		else
		{
			INFO("Command launched: %s %s: pid %d\n", at->at_Command, at->at_Arguments, pid );
		}
	}
	else
	{
		FERROR("Cannot allocate memory for user\n");
	}
	
	return at;
}

/**
 * Delete Autotask
 *
 * @return new User structure when success, otherwise NULL
 */
void AutotaskDelete( Autotask *at )
{
	if( at != NULL )
	{
		if( at->at_Command != NULL )
		{
			FFree( at->at_Command );
		}
		
		if( at->at_Arguments != NULL )
		{
			FFree( at->at_Arguments );
		}
		
		if( at->at_Name != NULL )
		{
			FFree( at->at_Name );
		}
		
		if( at->at_PID >= 0 )
		{
			while( at->at_Launched != FALSE )
			{
				kill( at->at_PID, SIGTERM );
			}
		}
		
		FFree( at );
	}
}