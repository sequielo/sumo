#ifndef help_h
#define help_h
//---------------------------------------------------------------------------//
//                        jp_router_help.h -
//      Help-screen of the junction percentage router module
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 20 Jan 2004
//  subproject           : junction percentage router
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.8  2006/01/09 13:33:30  dkrajzew
// debugging error handling
//
// Revision 1.7  2005/10/07 11:48:00  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/15 12:27:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2004/12/20 15:00:36  dkrajzew
// version patched
//
// Revision 1.4  2004/12/20 14:05:15  dkrajzew
// version patched
//
// Revision 1.3  2004/12/20 13:15:58  dkrajzew
// options output corrected
//
// Revision 1.2  2004/12/20 10:48:35  dkrajzew
// net-files changed to net-file
//
// Revision 1.1  2004/08/02 13:03:19  dkrajzew
// applied better names
//
// Revision 1.1  2004/02/06 08:56:44  dkrajzew
// _INC_MALLOC definition removed (does not work on MSVC7.0)
//
// Revision 1.1  2004/01/26 07:12:12  dkrajzew
// now two routers are available - the dua- and the jp-router
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H


 /* =========================================================================
 * definitions
 * ======================================================================= */
/**
 * The list of help strings for the sumo-junction percentage router module
 */
char *help[] = {
    " Routes vehicles by turning percentages on junctions.",
    " ",
    "Usage: (sumo-)jtrrouter(.exe) [OPTION]*",
    " ",
    "Examples: ",
    "  sumo-jtrrouter -c routes_munich.cfg",
    " ",
    "Options:",
    " Input files:",
    "  Configuration",
    "   -c, --configuration FILE       FILE will be used as configuration",
    "                                    1. Default: ./sumo-netcovert.cfg",
    "  Network description:",
    "   -n, --net-file FILE           FILE containing the SUMO-net",
    " ",
    "  Route descriptions:",
    "   -s, --sumo-input FILE          FILE containing SUMO-routes",
    "   -t, --trip-defs FILE           FILE containing trip definitions",
    "   -a, --alternatives FILE        FILE containing route alternatives",
    "   -f, --flow-definitions FILE    FILE containing flow definitions",
    "   --cell-input FILE              FILE containing cell routes",
    "   --artemis-input FILE           FILE containing cell routes",
    " ",
    " Output files:",
    "   -o, --output-file FILE         Generated routes will be written to FILE",
    "                                   Additionally, a file with route",
    "                                   alternatives will be generated as",
    "                                   'FILE.alt'",
    " ",
    " Deafults:",
    "   -V, --krauss-vmax FLOAT    Default value for krauss' vmax",
    "   -A, --krauss-a FLOAT       Default value for krauss' max. accel",
    "   -B, --krauss-b FLOAT       Default value for krauss' max. decel",
    "   -L, --krauss-length FLOAT  Default value for krauss' vehicle length",
    "   -E, --krauss-eps FLOAT     Default value for krauss' driver imperf.",
    " ",
    " Process Options:",
    "   -b, --begin INT                The begin time of routing",
    "   -e, --end INT                  The end time of routing",
    "   -R, --random-per-second FLOAT  Builds (additional) random routes",
    "   --gBeta FLOAT                  Gawron-Beta for DUE",
    "   --gA FLOAT                     Gawron-A for DUE",
    "   --unsorted                     The routes are assumed to be unsorted",
    "   --save-cell-rindex             The index to cell-routes will be saved",
    "   --intel-cell                   Set, when reading cell-routes computed",
    "                                   on an Intel-machine",
    "   --no-last-cell                 Use best, not the last cell-route",
    "   --continue-on-unbuild          Continue if a route could not be build",
    "   --move-on-short                Moves vehicles edges further if the edge",
    "                                   is too short for vehicle emission",
    " ",
    " Random Number Options:",
    "  --srand INT                     Initialises the random number generator",
    "                                   with the given value",
    "  --abs-rand                      Set this when the current time shall be",
    "                                   used for random number initialisation",
    " ",
    " Report options:",
    "   -v, --verbose                   SUMO-ROUTER will report what it does",
    "   -W, --suppress-warnings         No warnings will be printed",
    "   -l, --log-file FILE             Writes all messages to the file",
    "   -p, --print-options             Prints option values before processing",
    "   -?, --help                      This screen",
    "   --stats-period INT          Step number to pass between status output",
    0
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
//
