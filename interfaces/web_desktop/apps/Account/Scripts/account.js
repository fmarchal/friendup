/*©agpl*************************************************************************
*                                                                              *
* This file is part of FRIEND UNIFYING PLATFORM.                               *
*                                                                              *
* This program is free software: you can redistribute it and/or modify         *
* it under the terms of the GNU Affero General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* This program is distributed in the hope that it will be useful,              *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 *
* GNU Affero General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Affero General Public License     *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.        *
*                                                                              *
*****************************************************************************©*/

Application.run = function( msg, iface )
{
	var v = new View( {
		title: i18n( 'i18n_account' ),
		width: 700,
		height: 490
	} );
	
	v.onClose = function()
	{
		Application.quit();
	}
	
	this.mainView = v;
	
	var m = new Module( 'system' );
	m.onExecuted = function( e, d )
	{
		var s = JSON.parse( d );
		var f = new File( 'Progdir:Templates/main.html' );
		//f.replacements = {
		//	'username' : s.Name,
		//	'fullname' : s.FullName,
		//	'email'    : s.Email
		//};
		f.i18n();
		f.onLoad = function( data )
		{
			v.setContent( data );
			s.command = 'userinfo';
			v.sendMessage( s );
			
			Authenticate.load( 'publickey', displayPublicKey );
		}
		f.load();
	}
	m.execute( 'userinfoget', { id: msg.userId } );	
}

function displayPublicKey( data )
{
	Application.mainView.sendMessage( { command : 'setkey', data : data } );
}

Application.receiveMessage = function( msg )
{
	if( !msg.command ) return;
	
	if( msg.command == 'saveresult' )
	{
		this.mainView.setFlag( 'title', i18n( 'i18n_account' ) + ( msg.result == 'ok' ? ' (saved)' : ' (failed to save)' ) );
		setTimeout( function()
		{
			Application.mainView.setFlag( 'title', i18n( 'i18n_account' ) );
		}, 1000 );
		
		// Update login in Workspace!
		if( msg.result == 'ok' )
		{
			Notify({'title':i18n('i18_account2'),'text':i18n('i18n_settings_saved')});
			Application.sendMessage( {
				type: 'system',
				command: 'updatelogin',
				username: msg.data.Name,
				password: msg.data.Password
			} );
		}
	}
	
	switch( msg.command )
	{
		case 'encrypt':
			if( msg.key )
			{
				console.log( 'encrypt msg: ', msg );
				
				Authenticate.encrypt( {
					
					destinationViewId: msg.viewId,
					data: msg.key 
					
				}, function( item ){
					
					console.log( 'encrypt item: ', item );
					
					Application.sendMessage( {
						
						command: 'updatekey', 
						destinationViewId: item.destinationViewId, 
						data: item.data 
						
					} );
					
				} );
			}
			break;
		
		case 'decrypt':
			if( msg.key )
			{
				console.log( 'decrypt msg: ', msg );
				
				Authenticate.decrypt( {
					
					destinationViewId: msg.viewId,
					data: msg.key 
					
				}, function( item ){
					
					console.log( 'decrypt item: ', item );
					
					Application.sendMessage( {
						
						command: 'updatekey', 
						destinationViewId: item.destinationViewId, 
						data: item.data 
						
					} );
					
				} );
			}
			break;
		
		default:
			return;
	}
}
