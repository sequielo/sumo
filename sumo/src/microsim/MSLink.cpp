/****************************************************************************/
/// @file    MSLink.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Sept 2002
/// @version $Id$
///
// A connnection between lanes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utils/iodevices/OutputDevice.h>
#include "MSNet.h"
#include "MSLink.h"
#include "MSLane.h"
#include "MSGlobals.h"
#include "MSVehicle.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
SUMOTime MSLink::myLookaheadTime = TIME2STEPS(1);


// ===========================================================================
// member method definitions
// ===========================================================================
#ifndef HAVE_INTERNAL_LANES
MSLink::MSLink(MSLane* succLane,
               LinkDirection dir, LinkState state,
               SUMOReal length)
    :
    myLane(succLane),
    myRequestIdx(0), myRespondIdx(0),
    myState(state), myDirection(dir),  myLength(length) {}
#else
MSLink::MSLink(MSLane* succLane, MSLane* via,
               LinkDirection dir, LinkState state, SUMOReal length)
    :
    myLane(succLane),
    myRequestIdx(0), myRespondIdx(0),
    myState(state), myDirection(dir), myLength(length),
    myJunctionInlane(via) {}
#endif


MSLink::~MSLink() {}


void
MSLink::setRequestInformation(unsigned int requestIdx, unsigned int respondIdx, bool isCrossing, bool isCont,
                              const std::vector<MSLink*>& foeLinks,
                              const std::vector<MSLane*>& foeLanes) {
    myRequestIdx = requestIdx;
    myRespondIdx = respondIdx;
    myIsCrossing = isCrossing;
    myAmCont = isCont;
    myFoeLinks = foeLinks;
    myFoeLanes = foeLanes;
}


void
MSLink::setApproaching(const SUMOVehicle* approaching, const SUMOTime arrivalTime, const SUMOReal arrivalSpeed, const SUMOReal leaveSpeed, const bool setRequest) {
    const SUMOTime leaveTime = getLeaveTime(arrivalTime, arrivalSpeed, leaveSpeed, approaching->getVehicleType().getLengthWithGap());
    myApproachingVehicles.insert(std::make_pair(approaching, ApproachingVehicleInformation(arrivalTime, leaveTime, arrivalSpeed, leaveSpeed, setRequest)));
}


void
MSLink::addBlockedLink(MSLink* link) {
    myBlockedFoeLinks.insert(link);
}



bool
MSLink::willHaveBlockedFoe() const {
    for (std::set<MSLink*>::const_iterator i = myBlockedFoeLinks.begin(); i != myBlockedFoeLinks.end(); ++i) {
        if ((*i)->isBlockingAnyone()) {
            return true;
        }
    }
    return false;
}


void
MSLink::removeApproaching(const SUMOVehicle* veh) {
    myApproachingVehicles.erase(veh);
}


MSLink::ApproachingVehicleInformation
MSLink::getApproaching(const SUMOVehicle* veh) const {
    std::map<const SUMOVehicle*, ApproachingVehicleInformation>::const_iterator i = myApproachingVehicles.find(veh);
    if (i != myApproachingVehicles.end()) {
        return i->second;
    } else {
        return ApproachingVehicleInformation(-1000, -1000, 0, 0, false);
    }
}


SUMOTime
MSLink::getLeaveTime(SUMOTime arrivalTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed, SUMOReal vehicleLength) const {
    return arrivalTime + TIME2STEPS((getLength() + vehicleLength) / (0.5 * (arrivalSpeed + leaveSpeed)));
}


bool
MSLink::opened(SUMOTime arrivalTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed, SUMOReal vehicleLength) const {
    if (myState == LINKSTATE_TL_RED) {
        return false;
    }
    if (myAmCont && MSGlobals::gUsingInternalLanes) {
        return true;
    }
    const SUMOTime leaveTime = getLeaveTime(arrivalTime, arrivalSpeed, leaveSpeed, vehicleLength);
    for (std::vector<MSLink*>::const_iterator i = myFoeLinks.begin(); i != myFoeLinks.end(); ++i) {
#ifdef HAVE_INTERNAL
        if (MSGlobals::gUseMesoSim) {
            if ((*i)->getState() == LINKSTATE_TL_RED) {
                continue;
            }
        }
#endif
        if ((*i)->blockedAtTime(arrivalTime, leaveTime, arrivalSpeed, leaveSpeed, myLane == (*i)->getLane())) {
            return false;
        }
    }
    return true;
}


bool
MSLink::blockedAtTime(SUMOTime arrivalTime, SUMOTime leaveTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed,
                      bool sameTargetLane) const {
    for (std::map<const SUMOVehicle*, ApproachingVehicleInformation>::const_iterator i = myApproachingVehicles.begin(); i != myApproachingVehicles.end(); ++i) {
        if (!i->second.willPass) {
            continue;
        }
        if (i->second.leavingTime < arrivalTime) {
            // ego wants to be follower
            if (sameTargetLane && unsafeHeadwayTime(arrivalTime - i->second.leavingTime, i->second.leaveSpeed, arrivalSpeed)) {
                return true;
            }
        } else if (i->second.arrivalTime > leaveTime) {
            // ego wants to be leader
            if (sameTargetLane && unsafeHeadwayTime(i->second.arrivalTime - leaveTime, leaveSpeed, i->second.arrivalSpeed)) {
                return true;
            }
        } else {
            // even without considering safeHeadwayTime there is already a conflict
            return true;
        }
    }
    return false;
}


std::vector<const SUMOVehicle*> 
MSLink::getBlockingFoes(SUMOTime arrivalTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed, SUMOReal vehicleLength) const {
    std::vector<const SUMOVehicle*> result;
    if (myState == LINKSTATE_TL_RED) {
        return result;
    }
    if (myAmCont && MSGlobals::gUsingInternalLanes) {
        return result;
    }
    const SUMOTime leaveTime = getLeaveTime(arrivalTime, arrivalSpeed, leaveSpeed, vehicleLength);
    for (std::vector<MSLink*>::const_iterator i = myFoeLinks.begin(); i != myFoeLinks.end(); ++i) {
#ifdef HAVE_INTERNAL
        if (MSGlobals::gUseMesoSim) {
            if ((*i)->getState() == LINKSTATE_TL_RED) {
                continue;
            }
        }
#endif
        std::vector<const SUMOVehicle*> foes = (*i)->blockedAtTimeFoes(arrivalTime, leaveTime, arrivalSpeed, leaveSpeed, myLane == (*i)->getLane());
        result.insert(result.end(), foes.begin(), foes.end());
    }
    return result;
}


std::vector<const SUMOVehicle*>
MSLink::blockedAtTimeFoes(SUMOTime arrivalTime, SUMOTime leaveTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed,
                      bool sameTargetLane) const {
    std::vector<const SUMOVehicle*> result;
    for (std::map<const SUMOVehicle*, ApproachingVehicleInformation>::const_iterator i = myApproachingVehicles.begin(); i != myApproachingVehicles.end(); ++i) {
        if (!i->second.willPass) {
            continue;
        }
        if (i->second.leavingTime < arrivalTime) {
            // ego wants to be follower
            if (sameTargetLane && unsafeHeadwayTime(arrivalTime - i->second.leavingTime, i->second.leaveSpeed, arrivalSpeed)) {
                result.push_back(i->first);
            }
        } else if (i->second.arrivalTime > leaveTime) {
            // ego wants to be leader
            if (sameTargetLane && unsafeHeadwayTime(i->second.arrivalTime - leaveTime, leaveSpeed, i->second.arrivalSpeed)) {
                result.push_back(i->first);
            }
        } else {
            // even without considering safeHeadwayTime there is already a conflict
            result.push_back(i->first);
        }
    }
    return result;
}


SUMOTime
MSLink::unsafeHeadwayTime(SUMOTime headwayTime, SUMOReal leaderSpeed, SUMOReal followerSpeed) {
    if (headwayTime < myLookaheadTime) {
        return true;
    }
    // headwayTime is the expected time difference between the leaders rear and the followers front + safeGap
    if (leaderSpeed < DEFAULT_VEH_DECEL) {
        // leader may break in one timestep
        leaderSpeed = 0;
    }
    // this formula is conservative since the headway time increases as soon as
    // the follower starts to break
    // on the other hand, vehicles turning onto a higher-priority road usually
    // don't want to make other people break
    return ((followerSpeed - leaderSpeed) / DEFAULT_VEH_DECEL) > STEPS2TIME(headwayTime);
}


bool
MSLink::maybeOccupied(MSLane* lane) {
    MSVehicle* veh = lane->getLastVehicle();
    SUMOReal distLeft = 0;
    if (veh == 0) {
        veh = lane->getPartialOccupator();
        distLeft = lane->getLength() - lane->getPartialOccupatorEnd();
    } else {
        distLeft = lane->getLength() - veh->getPositionOnLane() + veh->getVehicleType().getLength();
    }
    if (veh == 0) {
        return false;
    } else {
        assert(distLeft > 0);
        // can we be sure that the vehicle leaves this lane in the next step?
        bool result = distLeft > (veh->getSpeed() - veh->getCarFollowModel().getMaxDecel());
        return result;
    }
}


bool
MSLink::hasApproachingFoe(SUMOTime arrivalTime, SUMOTime leaveTime, SUMOReal speed) const {
    for (std::vector<MSLink*>::const_iterator i = myFoeLinks.begin(); i != myFoeLinks.end(); ++i) {
        if ((*i)->blockedAtTime(arrivalTime, leaveTime, speed, speed, myLane == (*i)->getLane())) {
            return true;
        }
    }
    for (std::vector<MSLane*>::const_iterator i = myFoeLanes.begin(); i != myFoeLanes.end(); ++i) {
        if ((*i)->getVehicleNumber() > 0 || (*i)->getPartialOccupator() != 0) {
            return true;
        }
    }
    return false;
}


LinkDirection
MSLink::getDirection() const {
    return myDirection;
}


void
MSLink::setTLState(LinkState state, SUMOTime /*t*/) {
    myState = state;
}


MSLane*
MSLink::getLane() const {
    return myLane;
}


void 
MSLink::writeApproaching(OutputDevice& od, const std::string fromLaneID) const {
    if (myApproachingVehicles.size() > 0) {
        od.openTag("link");
        od.writeAttr(SUMO_ATTR_FROM, fromLaneID);
#ifdef HAVE_INTERNAL_LANES
        const std::string via = getViaLane() == 0 ? "" : getViaLane()->getID();
#else
        const std::string via = "";
#endif
        od.writeAttr(SUMO_ATTR_VIA, via);
        od.writeAttr(SUMO_ATTR_TO, getLane()==0 ? "" : getLane()->getID());
        std::vector<std::pair<SUMOTime, const SUMOVehicle*> > toSort; // stabilize output
        for (std::map<const SUMOVehicle*, ApproachingVehicleInformation>::const_iterator it = myApproachingVehicles.begin(); it != myApproachingVehicles.end(); ++it) {
            toSort.push_back(std::make_pair(it->second.arrivalTime, it->first));
        }
        std::sort(toSort.begin(), toSort.end());
        for (std::vector<std::pair<SUMOTime, const SUMOVehicle*> >::const_iterator it = toSort.begin(); it != toSort.end(); ++it) {
            od.openTag("approaching");
            const ApproachingVehicleInformation& avi = myApproachingVehicles.find(it->second)->second;
            od.writeAttr(SUMO_ATTR_ID, it->second->getID());
            od.writeAttr("arrivalTime", time2string(avi.arrivalTime));
            od.writeAttr("leaveTime", time2string(avi.leavingTime));
            od.writeAttr("arrivalSpeed", toString(avi.arrivalSpeed));
            od.writeAttr("leaveSpeed", toString(avi.leaveSpeed));
            od.writeAttr("willPass", toString(avi.willPass));
            od.closeTag();
        }
        od.closeTag();
    }
}


#ifdef HAVE_INTERNAL_LANES
MSLane*
MSLink::getViaLane() const {
    return myJunctionInlane;
}


MSLink::LinkLeaders
MSLink::getLeaderInfo(SUMOReal dist) const {
    LinkLeaders result;
    if (MSGlobals::gUsingInternalLanes && myJunctionInlane == 0) {
        // this is an exit link

        for (std::vector<MSLane*>::const_iterator it_lane = myFoeLanes.begin(); it_lane != myFoeLanes.end(); ++it_lane) {
            // it is not sufficient to return the last vehicle on the foeLane because ego might be its leader
            // therefore we return all vehicles on the lane
            const MSLane::VehCont& vehicles = (*it_lane)->getVehiclesSecure();
            (*it_lane)->releaseVehicles();
            for (MSLane::VehCont::const_iterator it_veh = vehicles.begin(); it_veh != vehicles.end(); ++it_veh) {
                MSVehicle* leader = *it_veh;
                // XXX apply viaLane/foeLane specific distance offset 
                // to account for the fact that the crossing point has different distances from the lane ends
                result.push_back(std::make_pair(leader,
                            dist - ((*it_lane)->getLength() - leader->getPositionOnLane()) - leader->getVehicleType().getLength()));

            }
            // XXX partial occupates should be ignored if they do not extend past the crossing point
            MSVehicle* leader = (*it_lane)->getPartialOccupator();
            if (leader != 0) {
                result.push_back(std::make_pair(leader, dist - ((*it_lane)->getLength() - (*it_lane)->getPartialOccupatorEnd())));
            }
        }
    }
    return result;
}
#endif


MSLane*
MSLink::getViaLaneOrLane() const {
#ifdef HAVE_INTERNAL_LANES
    if (myJunctionInlane != 0) {
        return myJunctionInlane;
    }
#endif
    return myLane;
}


unsigned int
MSLink::getRespondIndex() const {
    return myRespondIdx;
}



/****************************************************************************/
