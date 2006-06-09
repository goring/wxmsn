/*  Copyright (c) 2005 Neil "Superna" ARMSTRONG
    This file is part of wxMSN.

    wxMSN is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    wxMSN is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with wxMSN; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/** 	\file 	defines.h
	\author Neil "Superna" ARMSTRONG
	\date 	28/12/2005
    $Id$
*/

// Include Protection

#ifndef _DEFINES_H_
#define _DEFINES_H_

// Includes
#include <wx/wx.h>

//Non namespaced Enums

#define RC_PATH nsUtils::CPrefs::GetPath()
//wxString(wxPathOnly(wxTheApp->argv[0]) + wxT("/"))
#define RC_LOCATE(a) nsUtils::CPrefs::LocateRSC(_T(a))
//RC_LOCATE(a) RC_PATH+wxT("resources/")+wxT(a)

// MSN Statuses (CContact)
enum ONLINE_STATUS
{
    // NLN - Available
    ONLINE=0,
    // BSY - Busy
    BUSY,
    // IDL - Idle
    IDLE,
    // BRB - Be Right Back
    BE_RIGHT_BACK,
    // AWY - Away
    AWAY,
    // PHN - On the Phone
    PHONE,
    // LUN - Out to Lunch
    OUT_TO_LUNCH,
    //HDN
    APPEAR_OFFLINE,
    // Custom Status Enums
    OFFLINE,
};


// ContactList Sorting Styles (CContactList)
enum CONTACT_SORTING_STYLE
{
    BY_GROUP,
    BY_STATUS,
    MIXED
};

// CContactList Events IDs
enum CONTACTLIST_EVENTID
{
    HOVER_TIMER = wxID_HIGHEST
};

// CPopup Events IDs
enum POPUP_EVENTID
{
    DISMISS_TIMER = wxID_HIGHEST
};


namespace nsChat
{
    //enums for CChat Windows
    enum CEntryIds
    {
        AddTextEvent = wxID_HIGHEST,
		TabChangeEvent,
        SendButton,
        EntryTextControl,
		
    };
	
    enum TabStatus
    {
        TabStIdle = 0,
        TabStError,
        TabStConnected,
        TabStNewMessage,
        TabStWriting,
        
    };
} // nsChat

namespace nsContact
{
    // Enums for the CContactFrame window
    enum
    { 
        ImageMenu = wxID_HIGHEST,
        FNameMenu,
        PStatusMenu,
        DisconnectMenu,
        SetBusyMenu,
        SetAvailableMenu,
		AboutMenu = wxID_ABOUT,
        ExitMenu = wxID_EXIT,
        PrefMenu = wxID_PREFERENCES,
        
        FileMenuID = 0,
        AccountMenuID,
        ToolsMenuID,
        HelpMenuID,
		
		// CONTACT LIST DEBUG
		ByGroup,
		ByStatus,
		Mixed,
    };
    
    #define HelpMenuName _("&Help")
    #define FileMenuName _("&File")
    #define ToolsMenuName _("&Tools")
    #define AccountMenuName _("&Account")
    
	// Enums for the CContactList control
	enum
	{
		ChangeFNameMenu = wxID_HIGHEST,
		ChangePMessageMenu,
		OnlineStatusMenu,
		BusyStatusMenu,
		IdleStatusMenu,
		BrbStatusMenu,
		AwayStatusMenu,
		PhoneStatusMenu,
		LunchStatusMenu,
	    AppearOfflineStatusMenu,
		CustomStatusMenu,
        
        //Group Menu
        GroupBlockMenu,
        GroupChatMenu,
        GroupRemoveMenu,
        GroupRenameMenu,
        
        //Contact Menu
        ContactChatMenu,
        ContactInfoMenu,
        ContactGroupRemoveMenu,
        ContactGroupCopyMenu,
        ContactGroupMoveMenu,
        ContactRemoveMenu,
	};
	
	// Enums for the CConnectPanel control
	enum
	{
		ConnectButton = wxID_HIGHEST,
		NewProfileButton,
		DeleteProfileButton,
		ModifyProfileButton,
	};
	
}//nsContact

namespace nsConsole
{
    // Enums for the CConsole window
    enum
    {
        wxButtonConnect = wxID_HIGHEST,
        wxButtonDisconnect,
        wxButtonFName,
        wxButtonPStatus,
        wxButtonClear,
        wxTimerID,
    };
} //nsConsole

namespace nsCore
{
    enum
    {
        wxTimerID,
        SongTimer,
        IdleTimer,
    };
    
    #define IDLETIME 600000
    #define SONGFILE _("songplayed")
    
    enum SongStatus
    {
        PrefsNoNewSong,
        PrefsNoSong,
        PrefsNewSong,
        PrefsSongError,
    };
}

namespace nsEvents
{
    enum MsnEventTypes
    {
        wxMsnEventRefreshList = 0,
        wxMsnEventConnectionLog,
        wxMsnEventConnectionFailed,
        wxMsnEventConnectionP1, //Status before Authentification
        wxMsnEventConnectionP2, //Successful Authentification
        wxMsnEventConnectionP3, //Status after Authentification
        wxMsnEventConnectionP4, //Status after Sync
        wxMsnEventConnected, //Status after NLN
        wxMsnEventDisconnected,
        wxMsnEventPersoInfo,
        wxMsnEventPersoPStatus,
        wxMsnEventNewContact,
        wxMsnEventNewGroup,
        wxMsnEventAssignGroup,
        wxMsnEventContactConnected,
        wxMsnEventContactDisconnected,
        wxMsnEventContactStatus,
        wxMsnEventContactPStatus,
        wxMsnEventChatInvitation,
        wxMsnEventNewSbSession,
    };
    
    enum ListEventTypes
    {
        wxListEventConnect = 0,
        wxListEventDisconnect,
        wxListEventDblClick,
        wxListEventBlock,
        wxListEventGroupBlock,
        wxListEventGroupChat,
        wxListEventGroupRemove,
        wxListEventGroupRename,
        wxListEventContactGroupRemove,
        wxListEventContactGroupCopy,
        wxListEventContactGroupMove,
        wxListEventContactRemove,
        wxListEventContactInfo,
        wxListEventAdd,
        wxListEventNewFName,
        wxListEventNewStatus,
        wxListEventNewPStatus,
    };
    
    enum ChatEventTypes
    {
        wxChatEventAddText = 0, //When text is needed to be sent
        wxChatEventCloseChat, //When tab is closed
        wxChatEventInviteContact, //When contact must be invited to a switchboard session
        wxChatEventCreateSwitchboard, //When a new sb session must be opened (Handler -> MsnCon)
    };
}

#endif
