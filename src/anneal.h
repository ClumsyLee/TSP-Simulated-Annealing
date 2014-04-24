#ifndef AI_HOMEWORK3_SRC_ANNEAL_H_
#define AI_HOMEWORK3_SRC_ANNEAL_H_

#include <ostream>
#include <random>
#include <vector>

// Class of the sim-anneal algorithm.
// An object corresponds to a certain problem, i.e. the infomation of cities
// will not change once the object is created.
// For the implement of a certain function, see below or anneal.cpp.
class Anneal
{
 public:
    typedef std::vector<int> Path;

    struct City
    {
        char name;
        double x, y;
    };

    explicit Anneal(const std::vector<City> &cities);

    // Try to find the best path, print infomations to std::cout and os
    Path FindPath(std::ostream &os);

    // Print the path to os, in the format of "A B C ... Z <total length>".
    void PrintPath(std::ostream &os) const;

 private:
    double AcceptRatioNow(int sequence_size) const;
    bool BalanceReached() const;
    void DropTemperature();
    double Evaluate(const Path &path) const;
    Path GenerateRandomNeighbor(const Path &path_before) const;
    Path GenerateRandomPath() const;
    void InitializeTemperature();
    bool ShouldTerminate() const;
    double TransitionProbability(double value_before,
                                 double value_after) const;

    // problem related vaiables
    const std::vector<City> cities_;
    std::vector<std::vector<double>> distances_;

    // random number generator
    mutable std::mt19937 generator_;

    // status variables
    int iterations_this_temperature_;
    Path path_now_;
    double t_;
    int temperature_drop_times_;
    double value_now_;
};

// Inline functions

// iterate 100 * city_number times for a temperature
inline bool Anneal::BalanceReached() const
{
    return iterations_this_temperature_ >=
               100 * static_cast<int>(cities_.size());
}

// drop the temperature by a fixed ratio (0.98)
inline void Anneal::DropTemperature()
{
    t_ *= 0.98;
    iterations_this_temperature_ = 0;
}

// Terminate the algorithm when the temperature is low enough
inline bool Anneal::ShouldTerminate() const
{
    return t_ < 1e-3;
}

// Assue value_before < value_after
inline double Anneal::TransitionProbability(double value_before,
                                            double value_after) const
{
    return std::exp(-(value_after - value_before) / t_);
}

#endif  // AI_HOMEWORK3_SRC_ANNEAL_H_
