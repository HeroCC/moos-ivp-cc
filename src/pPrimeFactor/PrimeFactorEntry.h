#ifndef PrimeFactorEntry_HEADER
#define PrimeFactorEntry_HEADER

class PrimeFactorEntry {
  public:
    PrimeFactorEntry(uint64_t target, int order_number, double moosstarttime);
    ~PrimeFactorEntry();
    bool ContinueCalculations(int tries);
    std::vector<int> GetCalculatedPrimeFactors();
    bool IsFinished();
    uint64_t GetTargetNumber();
    int GetOrderNumber();
    double GetStartTime();

  private:
    std::vector<int> m_pf;
    bool m_is_finished;
    uint64_t m_target;
    int m_progress;
    int m_max_calculations;
    int m_order_number;
    double m_start_time;

};

#endif
