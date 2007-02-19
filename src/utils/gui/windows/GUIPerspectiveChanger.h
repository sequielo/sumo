/****************************************************************************/
/// @file    GUIPerspectiveChanger.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A virtual class that allows to steer the visual output in dependence to
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIPerspectiveChanger_h
#define GUIPerspectiveChanger_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include "GUISUMOAbstractView.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOAbstractView;
class Position2D;
class Boundary;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIPerspectiveChanger
 * This is the interface for implementation of own classes that handle the
 * interaction between the user and a display field.
 * While most of our (IVF) interfaces allow zooming by choosing the rectangle
 * to show, other types of interaction are possible and have been implemented.
 * To differ between the behaviours, all types of interaction between the
 * user and the canvas are send to this class: mouse moving, mouse button
 * pressing and releasing.
 */
class GUIPerspectiveChanger
{
public:
    enum MouseState {
        MOUSEBTN_NONE = 0,
        MOUSEBTN_LEFT = 1,
        MOUSEBTN_RIGHT = 2,
        MOUSEBTN_MIDDLE = 4
    };

    /// Constructor
    GUIPerspectiveChanger(GUISUMOAbstractView &callBack);

    /// Destructor
    virtual ~GUIPerspectiveChanger();

    virtual void onLeftBtnPress(void *data);
    virtual void onLeftBtnRelease(void *data);
    virtual void onRightBtnPress(void *data);
    virtual bool onRightBtnRelease(void *data);
    virtual void onMouseMove(void *data);

    /// Returns the rotation of the canvas stored in this changer
    virtual SUMOReal getRotation() const = 0;

    /// Returns the x-offset of the field to show stored in this changer
    virtual SUMOReal getXPos() const = 0;

    /// Returns the y-offset of the field to show stored in this changer
    virtual SUMOReal getYPos() const = 0;

    /// Returns the zoom factor computed stored in this changer
    virtual SUMOReal getZoom() const = 0;

    /// Returns the information whether the view has chnaged
    bool changed() const;

    /** @brief Informs the changer about other chnages (window scaling etc.)
        This only resets the information that "change" has to return true */
    void otherChange();

    /// recenters the view to display the whole network
    virtual void recenterView() = 0;

    /// Informs the changer that the view has beend adapted to changes
    void applied();

    /** @brief Centers the view to the given position,
        setting it to a size that covers the radius.
        Used for: Centering of vehicles and junctions */
    virtual void centerTo(const Boundary &netBoundary,
                          const Position2D &pos, SUMOReal radius, bool applyZoom=true) = 0;

    /** @brief Centers the view to show the given boundary
        Used for: Centering of lanes */
    virtual void centerTo(const Boundary &netBoundary,
                          Boundary bound, bool applyZoom=true) = 0;

    /** @brief Sets the viewport
        Used for: Adapting a new viewport */
    virtual void setViewport(SUMOReal zoom, SUMOReal xPos, SUMOReal yPos) = 0;

    /// Returns the last mouse x-position an event occured at
    int getMouseXPosition() const;

    /// Returns the last mouse y-position an event occured at
    int getMouseYPosition() const;

    /// Sets the sizes of the network
    void setNetSizes(size_t width, size_t height);

    /// Informs the changer aboud the size of the canvas
    void applyCanvasSize(size_t width, size_t height);

protected:
    /// The parent window (canvas to scale)
    GUISUMOAbstractView &myCallback;

    /// Information whether the view has changed
    bool myHaveChanged;

    /// The sizes of the network
    size_t myNetWidth, myNetHeight;

    /// The sizes of the canvas
    size_t myCanvasWidth, myCanvasHeight;

    /// the current mouse position
    int myMouseXPosition, myMouseYPosition;

};


#endif

/****************************************************************************/

