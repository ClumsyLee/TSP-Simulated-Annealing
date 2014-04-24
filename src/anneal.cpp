#include <cmath>
#include <algorithm>
#include <iostream>
#include <random>
#include "anneal.h"

Anneal::Anneal(const std::vector<City> &cities)
        : cities_(cities),
          distances_(),
          generator_(),
          iterations_this_temperature_(0),
          t_(0.0),
          temperature_drop_times_(0),
          value_now_(0.0)
{
    int city_count = cities_.size();
    // resize the array to size() rows
    distances_.resize(city_count);

    // calculate the distance between each city
    for (int i = 0; i < city_count; i++)
        for (int j = 0; j < city_count; j++)
        {
            double x_differ = cities_[i].x - cities_[j].x;
            double y_differ = cities_[i].y - cities_[j].y;
            distances_[i].push_back(std::sqrt(x_differ * x_differ +
                                              y_differ * y_differ));
        }

    // initialize random number generator
    std::random_device rd;
    generator_.seed(rd());
}

Anneal::Path Anneal::FindPath(std::ostream &os)
{
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    InitializeTemperature();
    temperature_drop_times_ = 0;

    path_now_ = GenerateRandomPath();
    value_now_ = Evaluate(path_now_);

    while (!ShouldTerminate())
    {
        // a new t_
        iterations_this_temperature_ = 0;
        while (!BalanceReached())
        {
            iterations_this_temperature_++;

            Path neighbor = GenerateRandomNeighbor(path_now_);

            double new_value = Evaluate(neighbor);
            if (new_value <= value_now_ ||
                distribution(generator_) <
                    TransitionProbability(value_now_, new_value))
            {
                path_now_ = neighbor;
                value_now_ = new_value;
            }
        }
        // balance reached at this temperature
        // print the path now
        PrintPath(std::cout);
        PrintPath(os);
        DropTemperature();  // counter updated inside
    }
    return path_now_;
}

void Anneal::PrintPath(std::ostream &os) const
{
    for (int city_index : path_now_)
    {
        os << cities_[city_index].name << ' ';
    }
    os << value_now_ << std::endl;
}


// Private functions
double Anneal::AcceptRatioNow(int sequence_size) const
{
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    int accept_state_num = 0;
    int total_state_num = 0;

    std::vector<Path> state_sequence(sequence_size);
    std::vector<double> state_value(sequence_size);
    for (int i = 0; i < sequence_size; i++)
    {
        state_sequence[i] = std::move(GenerateRandomPath());
        state_value[i] = Evaluate(state_sequence[i]);
    }

    for (int i = 0; i < sequence_size; i++)
    {
        for (int j = 0; j < sequence_size; j++)
        {
            total_state_num++;
            if (state_value[j] <= state_value[i] ||
                distribution(generator_) <
                    TransitionProbability(state_value[i], state_value[j]))
                    accept_state_num++;
        }
    }
    return static_cast<double>(accept_state_num) /
           static_cast<double>(total_state_num);
}

double Anneal::Evaluate(const Path &path) const
{
    double length = 0.0;
    int last_city = path.back();
    for (int new_city : path)
    {
        length += distances_[last_city][new_city];
        last_city = new_city;
    }
    return length;
}

Anneal::Path Anneal::GenerateRandomNeighbor(const Path &path_before) const
{
    int size = path_before.size();
    // ((size - 1) * (size - 2) / 2) represents the total number of intervals
    // where only the first city will never be changed.
    // For example, when size = 3:
    //
    //  * * *
    //  * *
    //  *
    //
    //  There are 6 intervals in total
    int random_index = generator_() % ((size - 1) * (size - 2) / 2);
    int i;
    for (i = 1; i < size - 1; i++)
    {
        int j = size - 1 - i;
        if (random_index >= j)
            random_index -= j;
        else
            break;
    }

    Path neighbor(path_before);
    std::reverse(neighbor.begin() + i, neighbor.begin() + i + 2 + random_index);

    return neighbor;
}

Anneal::Path Anneal::GenerateRandomPath() const
{
    int city_number = cities_.size();
    Path path(city_number);
    for (int i = 0; i < city_number; i++)
        path[i] = i;
    // The first city is fixed, so only shuffle the other cities
    std::shuffle(path.begin() + 1, path.end(), generator_);
    return path;
}

// Increase the temperature, until the accept ratio is no less than 0.9
void Anneal::InitializeTemperature()
{
    t_ = 1;
    while (AcceptRatioNow(100) < 0.9)
    {
        t_ *= 1.05;
    }
    // Print the initial temperature only to stdout
    // (for the convenience of testing)
    std::cout << "t = " << t_ << std::endl;
}
