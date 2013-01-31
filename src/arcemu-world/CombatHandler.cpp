/*
 * ArcEmu MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (C) 2008 <http://www.ArcEmu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"

void WorldSession::HandleAttackSwingOpcode( WorldPacket & recv_data )
{	/*Apply a bugfix to prevent players from attacking people of the same faction. --Hemi*/
	if(!_player->IsInWorld()) return;
	CHECK_PACKET_SIZE(recv_data, 8);
	uint64 guid;
	recv_data >> guid;

	if(!guid)
	{	// does this mean cancel combat?
		HandleAttackStopOpcode(recv_data);
		return;
	}
	Log.Debug( "WORLD","Recvd CMSG_ATTACKSWING Message" );	// AttackSwing

	if(GetPlayer()->IsPacified() || GetPlayer()->IsStunned() || GetPlayer()->IsFeared())
		return;

	Unit *pEnemy = _player->GetMapMgr()->GetUnit(guid);
	if(!pEnemy)
	{
		sLog.outDebug("WORLD: "I64FMT" does not exist.", guid);
		return;
	}

	if(pEnemy->isDead() || _player->isDead())	// haxors :(
		return;

	bool attackablestatus = isAttackable( GetPlayer(), pEnemy, false );
	if( attackablestatus == false )
	{	//Prevent attacking people of the same faction. --Hemi
		HandleAttackStopOpcode(recv_data);
		return;
	}

	GetPlayer()->smsg_AttackStart(pEnemy);
	GetPlayer()->EventAttackStart();
}

void WorldSession::HandleAttackStopOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
		return;

	uint64 guid = GetPlayer()->GetSelection();
	if(guid)
	{
		Unit *pEnemy = _player->GetMapMgr()->GetUnit(guid);
		if(pEnemy != NULL)
		{
			GetPlayer()->EventAttackStop();
			GetPlayer()->smsg_AttackStop(pEnemy);
		}
	}
}

