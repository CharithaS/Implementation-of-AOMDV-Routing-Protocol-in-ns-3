/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 IITP RAS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Based on
 *      NS-2 AOMDV model developed by the CMU/MONARCH group and optimized and
 *      tuned by Samir Das and Mahesh Marina, University of Cincinnati;
 *
 *      AOMDV-UU implementation by Erik Nordström of Uppsala University
 *      http://core.it.uu.se/core/index.php/AOMDV-UU
 *
 * Authors: Elena Buchatskaia <borovkovaes@iitp.ru>
 *          Pavel Boyko <boyko@iitp.ru>
 */
#include "aomdv-id-cache.h"
#include <algorithm>

namespace ns3
{
namespace aomdv
{
bool
IdCache::IsDuplicate (Ipv4Address addr, uint32_t id)
{
  Purge ();
  for (std::vector<UniqueId>::const_iterator i = m_idCache.begin ();
       i != m_idCache.end (); ++i)
    if (i->m_context == addr && i->m_id == id)
      return true;
  struct UniqueId uniqueId =
  { addr, id, m_lifetime + Simulator::Now (), 0 };
  m_idCache.push_back (uniqueId);
  return false;
}

struct IdCache::UniqueId*
IdCache::GetId (Ipv4Address addr, uint32_t id)
{
  Purge ();
  for (std::vector<UniqueId>::iterator i = m_idCache.begin ();
       i != m_idCache.end (); ++i)
    {
      if (i->m_context == addr && i->m_id == id)
        {
          UniqueId *uid=&(*i);
          return uid;
        }
    }
  return NULL;
}

void
IdCache::InsertId (Ipv4Address addr, uint32_t id)
{
  struct UniqueId uniqueId =
  { addr, id, m_lifetime + Simulator::Now (), 0 };
  m_idCache.push_back (uniqueId);
}

void
IdCache::Purge ()
{
  m_idCache.erase (remove_if (m_idCache.begin (), m_idCache.end (),
                              IsExpired ()), m_idCache.end ());
}

uint32_t
IdCache::GetSize ()
{
  Purge ();
  return m_idCache.size ();
}

void
IdCache::UniqueId::ReversePathInsert (Ipv4Address nextHop, Ipv4Address lastHop)
{
  AOMDVRoute* route = new AOMDVRoute (nextHop, lastHop); 
  m_reversePathList.push_back (*route);
}

bool 
IdCache::UniqueId::ReversePathLookup (Ipv4Address nextHop, AOMDVRoute & rt, Ipv4Address lastHop)
{
  for (std::vector<AOMDVRoute>::iterator i = m_reversePathList.begin(); i != m_reversePathList.end(); ++i) 
    {
      if ( ( i->GetNextHop () == nextHop) && ( i->GetLastHop () == lastHop) )
        {
	  rt = *i;
          return true;     
        }
    }
    return false;
}
				
void 
IdCache::UniqueId::ForwardPathInsert (Ipv4Address nextHop, Ipv4Address lastHop)
{
  AOMDVRoute* route = new AOMDVRoute (nextHop, lastHop); 
  m_forwardPathList.push_back (*route);
}

bool
IdCache::UniqueId::ForwardPathLookup (Ipv4Address nextHop, AOMDVRoute & rt, Ipv4Address lastHop)
{
  for (std::vector<AOMDVRoute>::iterator i = m_forwardPathList.begin(); i != m_forwardPathList.end(); ++i) 
    {
      if ( ( i->GetNextHop () == nextHop) && ( i->GetLastHop () == lastHop) )
        {
	  rt = *i;
          return true;      
        }
    }
  return false;
}

}
}
