#ifndef MSActuatedTrafficLightLogic_h
#define MSActuatedTrafficLightLogic_h
//---------------------------------------------------------------------------//
//                        MSActuatedTrafficLightLogic.h -
//  The basic traffic light logic
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.4  2003/05/20 09:31:45  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output debugged; setting and retrieval of some parameter added
//
// Revision 1.3  2003/04/02 11:44:03  dkrajzew
// continuation of implementation of actuated traffic lights
//
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <utility>
#include <vector>
#include <bitset>
#include <map>
#include "MSEventControl.h"
#include "MSNet.h"
#include "MSTrafficLightLogic.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */

/**
 * The definition of a single phase.
 * We moved it out of the main class to allow later template
 * parametrisation.
 */
class ActuatedPhaseDefinition {
public:
    /// the duration of the phase
    size_t          duration;

    /// the mask which links are allowed to drive within this phase (green light)
    std::bitset<64>  driveMask;

    /// the mask which vehicles must not drive within this phase (red light)
    std::bitset<64>  breakMask;

    /// The minimum duration of the pahse
    size_t          minDuration;

    /// The maximum duration of the pahse
    size_t          maxDuration;

    /// stores the timestep of the last on-switched of the phase
    MSNet::Time _lastSwitch;


    /// constructor
    ActuatedPhaseDefinition(size_t durationArg,
        const std::bitset<64> &driveMaskArg, const std::bitset<64> &breakMaskArg,
        size_t minDurationArg, size_t maxDurationArg)
    : duration(durationArg), driveMask(driveMaskArg),
    breakMask(breakMaskArg), minDuration(minDurationArg),
    maxDuration(maxDurationArg), _lastSwitch(0)
    {
    minDuration = 5;
    maxDuration = 30;
    }

    /// destructor
    ~ActuatedPhaseDefinition() { }

private:
    /// invalidated standard constructor
    ActuatedPhaseDefinition();

};


/// definition of a list of phases, being the junction logic
typedef std::vector<ActuatedPhaseDefinition > ActuatedPhases;


/**
 * @class MSActuatedTrafficLightLogic
 * The implementation of a simple traffic light which only switches between
 * it's phases and sets the lights to red in between.
 * Some functions are called with an information about the current step. This
 * is needed as a single logic may be used by many junctions and so the current
 * step is stored within them, not within the logic.
 */
template< class _TInductLoop, class _TLaneState >
class MSActuatedTrafficLightLogic :
        public MSTrafficLightLogic
{
public:
    /// Definition of a map from lanes to induct loops lying on them
    typedef std::map<MSLane*, _TInductLoop*> InductLoopMap;

    /// Definition of a map from lanes to lane state detectors lying on them
    typedef std::map<MSLane*, _TLaneState*> LaneStateMap;

public:
    /// constructor
    MSActuatedTrafficLightLogic(const std::string &id, const ActuatedPhases &phases,
        size_t step, const std::vector<MSLane*> &lanes);

    /// destructor
    ~MSActuatedTrafficLightLogic();

    /** @brief Switches to the next phase
        Returns the time of the next switch */
    virtual MSNet::Time nextPhase(MSLogicJunction::InLaneCont &inLanes);

    /// Returns the duration of the given step
    virtual MSNet::Time duration() const;

    /** @brief Applies the right-of-way rules of the phase specified by the second argument to the first argument
        Requests of vehicles which are not allowed to drive as they
        have red light are masked out from the given request */
    virtual void applyPhase(MSLogicJunction::Request &request) const;

    /** Returns the link priorities for the given phase */
    virtual const std::bitset<64> &linkPriorities() const;

    /// Returns the index of the phase next to the given phase
    /// and stores the duration of the phase, which was just sent
    /// or stores the activation-time in _lastphase of the phase next
    virtual size_t nextStep();

    /// Desides, whether a phase should be continued by checking the gaps of vehicles having green
    virtual bool gapControl();

    /// Returns a vector of build detectors
    MSNet::DetectorCont getDetectorList() const;

	/// returns the current step
	size_t step() const { return _step; }

protected:
    /// Builds the detectors
    virtual void sproutDetectors(const std::vector<MSLane*> &lanes);

protected:
    /// A map from lanes to induct loops lying on them
    InductLoopMap myInductLoops;

    /// A map from lanes to lane states lying on them
    LaneStateMap myLaneStates;

    /// information whether the current phase is the dead-phase
    bool _allRed;

    /// information whether the current phase should be lenghtend
    bool _continue;



    /// The step within the cycla
    size_t _step;

    /// The phase definitions
    ActuatedPhases _phases;

    /// static container for all lights being set to red
    static std::bitset<64> _allClear;



};

#ifndef EXTERNAL_TEMPLATE_DEFINITION
#ifndef MSVC
#include "MSActuatedTrafficLightLogic.cpp"
#endif
#endif // EXTERNAL_TEMPLATE_DEFINITION


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "MSActuatedTrafficLightLogic.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

