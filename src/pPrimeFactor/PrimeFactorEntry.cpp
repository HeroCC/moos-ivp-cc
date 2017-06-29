// Conlan Cesar

#include <cstdlib>
#include <vector>
#include "PrimeFactorEntry.h"

using namespace std;

/*
vector<int> PrimeFactorEntry::m_pf;
bool PrimeFactorEntry::m_is_finished = false;
uint64_t PrimeFactorEntry::m_target;
int PrimeFactorEntry::m_progress;
int PrimeFactorEntry::m_max_calculations;
int PrimeFactorEntry::m_order_number;
double PrimeFactorEntry::m_start_time;
*/

PrimeFactorEntry::PrimeFactorEntry(uint64_t target, int ordernumber, double starttime) {
  m_target = target;
  m_progress = m_target;

  m_order_number = ordernumber;
  m_start_time = starttime;
}

PrimeFactorEntry::~PrimeFactorEntry() {
}

bool PrimeFactorEntry::ContinueCalculations(int loops) {
  int fac = 2;
  int calculations_this_tick = 0;
  while (m_progress > 1) {
    if (calculations_this_tick <= loops) {
      if (m_progress % fac == 0) {
        m_pf.push_back(fac);
        m_progress /= fac;
      } else {
        fac++;
      }
      calculations_this_tick++;
    } else {
      return(false);
    }
  }
  m_is_finished = true;
  return(true);
}

vector<int> PrimeFactorEntry::GetCalculatedPrimeFactors() {
  return(m_pf);
}

bool PrimeFactorEntry::IsFinished() {
  return(m_is_finished);
}

uint64_t PrimeFactorEntry::GetTargetNumber() {
  return(m_target);
}

int PrimeFactorEntry::GetOrderNumber() {
  return(m_order_number);
}

double PrimeFactorEntry::GetStartTime() {
  return(m_start_time);
}
