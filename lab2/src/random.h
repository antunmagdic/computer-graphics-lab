#ifndef RANDOM_H
#define RANDOM_H

#include <random>

class Random {
public:

    Random(double u=0, double s=1) :unif_ {0, 1}, normal_ {u, s} {
        re_.seed(std::chrono::system_clock::now().time_since_epoch().count());
        srand(time(0));
    }

    double next_double() { return unif_(re_); };
    int next_int() { return rand(); }
    double next_gaussian() { return normal_(re_); }
    std::default_random_engine get_engine() { return re_; }

private:

    std::default_random_engine re_;
    std::uniform_real_distribution<double> unif_;
    std::normal_distribution<double> normal_ {0.0, 1.0};

};

#endif // RANDOM_H
