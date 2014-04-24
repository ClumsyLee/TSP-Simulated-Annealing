#include <fstream>
#include <iostream>
#include "anneal.h"

std::vector<Anneal::City> ReadInputFileOrDie(const char *filename);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input file> <output file>\n";
        std::exit(EXIT_FAILURE);
    }

    std::vector<Anneal::City> cities = ReadInputFileOrDie(argv[1]);

    std::ofstream fout(argv[2]);
    if (!fout.is_open())
    {
        std::cerr << "Cannot open output file: (" << argv[2] << ")\n";
        std::exit(EXIT_FAILURE);
    }

    Anneal solver(cities);
    solver.FindPath(fout);

    return 0;
}

std::vector<Anneal::City> ReadInputFileOrDie(const char *filename)
{
    std::ifstream fin(filename);
    if (!fin.is_open())
    {
        std::cerr << "Cannot open input file: (" << filename << ")\n";
        std::exit(EXIT_FAILURE);
    }

    int city_num;
    fin >> city_num;
    std::vector<Anneal::City> cities(city_num);
    for (Anneal::City &city : cities)
    {
        fin >> city.name >> city.x >> city.y;
    }

  return cities;
}
