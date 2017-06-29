/************************************************************/
/*    NAME: Conlan Cesar                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef PrimeFactor_HEADER
#define PrimeFactor_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "PrimeFactorEntry.h"

class PrimeFactor : public CMOOSApp
{
 public:
   PrimeFactor();
   ~PrimeFactor();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected:
   void RegisterVariables();
   std::vector<int> GetAllPrimes(uint64_t max);
   void ProcessPrimeRequest(uint64_t number);
   std::string GetFormattedString(PrimeFactorEntry primeEntry, std::string username);

 private: // Configuration variables

 private: // State variables
};

#endif 
