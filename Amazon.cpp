#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <ctime>
#include <cstdlib>

// Address class to store coordinates
class Address {
public:
    int x, y;
    bool isPrime; // Track if the address is prime or not
    
    Address(int x, int y, bool isPrime) : x(x), y(y), isPrime(isPrime) {}
    
    double distanceTo(const Address& other) const {
        return std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2));
    }
};

// Base class for route optimization
class RouteOptimizer {
public:
    virtual std::vector<Address> optimize(const std::vector<Address>& addresses) = 0;
};

// Greedy Route Optimizer
class GreedyOptimizer : public RouteOptimizer {
public:
    std::vector<Address> optimize(const std::vector<Address>& addresses) override {
        std::vector<Address> path;
        std::vector<Address> remaining = addresses;
        
        Address current(0, 0, false); // Starting at depot (0, 0) which is not prime
        path.push_back(current); // Start path with depot

        while (!remaining.empty()) {
            auto nearest = std::min_element(remaining.begin(), remaining.end(),
                                            [&current](const Address& a, const Address& b) {
                                                return current.distanceTo(a) < current.distanceTo(b);
                                            });
            path.push_back(*nearest);
            remaining.erase(nearest);
            current = path.back();
        }
        // Ensure the path returns to the depot
        path.push_back(Address(0, 0, false));
        return path;
    }
};

// Utility function to calculate total distance of a path
double calculateTotalDistance(const std::vector<Address>& path) {
    double totalDistance = 0.0;
    for (size_t i = 1; i < path.size(); ++i) {
        totalDistance += path[i - 1].distanceTo(path[i]);
    }
    return totalDistance;
}

// OPT2 Heuristic function (simplified version)
std::vector<Address> opt2Heuristic(std::vector<Address> path) {
    auto twoOptSwap = [&path](int i, int k) {
        std::reverse(path.begin() + i, path.begin() + k + 1);
    };

    bool improvement = true;
    while (improvement) {
        improvement = false;
        for (size_t i = 1; i < path.size() - 2; ++i) {
            for (size_t k = i + 1; k < path.size() - 1; ++k) {
                double currentDist = path[i - 1].distanceTo(path[i]) + path[k].distanceTo(path[k + 1]);
                double newDist = path[i - 1].distanceTo(path[k]) + path[i].distanceTo(path[k + 1]);
                if (newDist < currentDist) {
                    twoOptSwap(i, k);
                    improvement = true;
                }
            }
        }
    }
    // Ensure the path starts and ends at the depot
    if (!path.empty() && (path.front().x != 0 || path.front().y != 0)) {
        path.insert(path.begin(), Address(0, 0, false));
    }
    if (!path.empty() && (path.back().x != 0 || path.back().y != 0)) {
        path.push_back(Address(0, 0, false));
    }
    return path;
}

// Function to print the path with prime addresses highlighted
void printPath(const std::vector<Address>& path, int truckNumber) {
    std::cout << "Truck " << truckNumber << " Path: ";
    for (const auto& addr : path) {
        if (addr.isPrime) {
            std::cout << "[" << addr.x << ", " << addr.y << "] "; // Highlight prime addresses with brackets
        } else {
            std::cout << "(" << addr.x << ", " << addr.y << ") ";
        }
    }
    std::cout << std::endl;
}

// Main function
int main() {
    std::vector<Address> addresses;
    int numAddresses;

    std::cout << "Enter the number of addresses: ";
    std::cin >> numAddresses;

    // Initialize random seed
    std::srand(std::time(0));

    // Generate random addresses
    for (int i = 0; i < numAddresses; ++i) {
        int x = std::rand() % 100; // Random x coordinate between 0 and 99
        int y = std::rand() % 100; // Random y coordinate between 0 and 99
        bool isPrime = std::rand() % 2; // Randomly assign prime status (0 or 1)
        addresses.emplace_back(x, y, isPrime);
    }

    // Separate prime and non-prime addresses
    std::vector<Address> primeAddresses;
    std::vector<Address> nonPrimeAddresses;

    for (const auto& addr : addresses) {
        if (addr.isPrime) {
            primeAddresses.push_back(addr);
        } else {
            nonPrimeAddresses.push_back(addr);
        }
    }

    // Combine prime and non-prime addresses
    std::vector<Address> sortedAddresses;
    sortedAddresses.insert(sortedAddresses.end(), primeAddresses.begin(), primeAddresses.end());
    sortedAddresses.insert(sortedAddresses.end(), nonPrimeAddresses.begin(), nonPrimeAddresses.end());

    // Split the sorted list into two sets for two trucks, ensuring primes are distributed correctly
    size_t splitIndex = sortedAddresses.size() / 2;
    std::vector<Address> truck1Addresses(sortedAddresses.begin(), sortedAddresses.begin() + splitIndex);
    std::vector<Address> truck2Addresses(sortedAddresses.begin() + splitIndex, sortedAddresses.end());

    // Print and optimize paths for Truck 1
    std::cout << "Greedy optimization for Truck 1:\n";
    GreedyOptimizer greedyOptimizer;
    std::vector<Address> truck1Path = greedyOptimizer.optimize(truck1Addresses);
    double truck1Distance = calculateTotalDistance(truck1Path);
    std::cout << "Truck 1 Greedy Path Distance: " << truck1Distance << std::endl;
    printPath(truck1Path, 1);

    // Print and optimize paths for Truck 2
    std::cout << "Greedy optimization for Truck 2:\n";
    std::vector<Address> truck2Path = greedyOptimizer.optimize(truck2Addresses);
    double truck2Distance = calculateTotalDistance(truck2Path);
    std::cout << "Truck 2 Greedy Path Distance: " << truck2Distance << std::endl;
    printPath(truck2Path, 2);

    // Print and optimize paths with OPT2 heuristic for Truck 1
    std::cout << "OPT2 heuristic for Truck 1:\n";
    std::vector<Address> truck1Opt2Path = opt2Heuristic(truck1Addresses);
    double truck1Opt2Distance = calculateTotalDistance(truck1Opt2Path);
    std::cout << "Truck 1 OPT2 Path Distance: " << truck1Opt2Distance << std::endl;
    printPath(truck1Opt2Path, 1);

    // Print and optimize paths with OPT2 heuristic for Truck 2
    std::cout << "OPT2 heuristic for Truck 2:\n";
    std::vector<Address> truck2Opt2Path = opt2Heuristic(truck2Addresses);
    double truck2Opt2Distance = calculateTotalDistance(truck2Opt2Path);
    std::cout << "Truck 2 OPT2 Path Distance: " << truck2Opt2Distance << std::endl;
    printPath(truck2Opt2Path, 2);

    return 0;
}
