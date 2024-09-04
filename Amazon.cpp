#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <ctime>
#include <cstdlib>
#include <map>

// Constants
const int WORK_DAY_MINUTES = 480; // 9 AM to 5 PM is 480 minutes
const double MAX_COORDINATE = 100.0; // Maximum coordinate value for distance calculation

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
void printPath(const std::vector<Address>& path, int truckNumber, int day) {
    std::cout << "Truck " << truckNumber << " Day " << day << " Path: ";
    for (const auto& addr : path) {
        if (addr.isPrime) {
            std::cout << "[" << addr.x << ", " << addr.y << "] "; // Highlight prime addresses with brackets
        } else {
            std::cout << "(" << addr.x << ", " << addr.y << ") ";
        }
    }
    std::cout << std::endl;
}

// Function to print day-wise routes
void printDayWiseRoutes(const std::map<int, std::vector<Address>>& dayWiseRoutes, int truckNumber) {
    for (const auto& [day, route] : dayWiseRoutes) {
        printPath(route, truckNumber, day);
    }
}

// Function to simulate truck delivery with day tracking
std::map<int, std::vector<Address>> simulateDelivery(std::vector<Address>& addresses, int truckNumber) {
    std::map<int, std::vector<Address>> dayWiseRoutes;
    std::vector<Address> unvisitedAddresses = addresses;

    Address current(0, 0, false); // Start at depot
    int day = 1;
    int elapsedTime = 0; // Time in minutes

    std::vector<Address> currentDayRoute;
    currentDayRoute.push_back(current);

    while (!unvisitedAddresses.empty()) {
        auto nearest = std::min_element(unvisitedAddresses.begin(), unvisitedAddresses.end(),
                                        [&current](const Address& a, const Address& b) {
                                            return current.distanceTo(a) < current.distanceTo(b);
                                        });

        double distanceToNearest = current.distanceTo(*nearest);
        if (elapsedTime + static_cast<int>(distanceToNearest) > WORK_DAY_MINUTES) {
            // Store the route for the current day
            currentDayRoute.push_back(Address(0, 0, false)); // Return to depot
            dayWiseRoutes[day] = currentDayRoute;

            // Prepare for the next day
            day++;
            elapsedTime = 0;
            currentDayRoute.clear();
            currentDayRoute.push_back(Address(0, 0, false)); // Start at depot for the new day

            // Recalculate the nearest address
            nearest = std::min_element(unvisitedAddresses.begin(), unvisitedAddresses.end(),
                                       [&current](const Address& a, const Address& b) {
                                           return current.distanceTo(a) < current.distanceTo(b);
                                       });
        }

        // Visit the nearest address
        currentDayRoute.push_back(*nearest);
        elapsedTime += static_cast<int>(distanceToNearest);
        current = currentDayRoute.back();
        unvisitedAddresses.erase(nearest);

        if (elapsedTime >= WORK_DAY_MINUTES) {
            // Store the route for the current day
            currentDayRoute.push_back(Address(0, 0, false)); // Return to depot
            dayWiseRoutes[day] = currentDayRoute;

            // Prepare for the next day
            day++;
            elapsedTime = 0;
            currentDayRoute.clear();
            currentDayRoute.push_back(Address(0, 0, false)); // Start at depot for the new day
        }
    }

    // Store any remaining addresses for the last day
    if (!currentDayRoute.empty()) {
        currentDayRoute.push_back(Address(0, 0, false)); // Return to depot
        dayWiseRoutes[day] = currentDayRoute;
    }

    return dayWiseRoutes;
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

    // Split the sorted list into two sets for two trucks
    size_t splitIndex = sortedAddresses.size() / 2;
    std::vector<Address> truck1Addresses(sortedAddresses.begin(), sortedAddresses.begin() + splitIndex);
    std::vector<Address> truck2Addresses(sortedAddresses.begin() + splitIndex, sortedAddresses.end());

    // Create instances of route optimizers
    GreedyOptimizer greedyOptimizer;
    
    // Simulate delivery for Truck 1 and Truck 2 with Greedy Algorithm
    std::cout << "Simulating delivery for Truck 1 with Greedy Algorithm:\n";
    auto truck1DayWiseRoutesGreedy = simulateDelivery(truck1Addresses, 1);
    std::cout << "Simulating delivery for Truck 2 with Greedy Algorithm:\n";
    auto truck2DayWiseRoutesGreedy = simulateDelivery(truck2Addresses, 2);

    // Print day-wise routes for Greedy Algorithm
    std::cout << "Greedy Algorithm:\n";
    std::cout << "Truck 1:\n";
    printDayWiseRoutes(truck1DayWiseRoutesGreedy, 1);
    std::cout << "Truck 2:\n";
    printDayWiseRoutes(truck2DayWiseRoutesGreedy, 2);

    // Apply OPT2 Heuristic
    std::cout << "Simulating delivery for Truck 1 with OPT2 Heuristic:\n";
    auto truck1AddressesOpt2 = opt2Heuristic(truck1Addresses);
    auto truck1DayWiseRoutesOpt2 = simulateDelivery(truck1AddressesOpt2, 1);
    std::cout << "Simulating delivery for Truck 2 with OPT2 Heuristic:\n";
    auto truck2AddressesOpt2 = opt2Heuristic(truck2Addresses);
    auto truck2DayWiseRoutesOpt2 = simulateDelivery(truck2AddressesOpt2, 2);

    // Print day-wise routes for OPT2 Heuristic
    std::cout << "OPT2 Heuristic:\n";
    std::cout << "Truck 1:\n";
    printDayWiseRoutes(truck1DayWiseRoutesOpt2, 1);
    std::cout << "Truck 2:\n";
    printDayWiseRoutes(truck2DayWiseRoutesOpt2, 2);

    return 0;
}
