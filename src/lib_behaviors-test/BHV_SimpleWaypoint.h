/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J. Leonard                    */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_SimpleWaypoint.ch                                */
/*    DATE: July 1st 2008  (For purposes of simple illustration) */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/
 
#ifndef BHV_SIMPLE_WAYPOINT_HEADER
#define BHV_SIMPLE_WAYPOINT_HEADER

#include <string>
#include "IvPBehavior.h"
#include "XYPoint.h"

class BHV_SimpleWaypoint : public IvPBehavior {
public:
  BHV_SimpleWaypoint(IvPDomain);
  ~BHV_SimpleWaypoint() {};
  
  bool         setParam(std::string, std::string);
  void         onIdleState();
  IvPFunction* onRunState();

protected:
  void         postViewPoint(bool viewable=true);
  IvPFunction* buildFunctionWithZAIC();
  IvPFunction* buildFunctionWithReflector();

protected: // Configuration parameters
  double       m_arrival_radius;
  double       m_desired_speed;
  XYPoint      m_nextpt;
  std::string  m_ipf_type;

protected: // State variables
  double   m_osx;
  double   m_osy;
};

#ifdef WIN32
	// Windows needs to explicitly specify functions to export from a dll
   #define IVP_EXPORT_FUNCTION __declspec(dllexport) 
#else
   #define IVP_EXPORT_FUNCTION
#endif

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_SimpleWaypoint(domain);}
}
#endif









