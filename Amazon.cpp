#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <ctime>
using std::reverse;
using std::vector;
using std::cout;
using std::endl;
using std::cin;
using std::abs;
using std::swap;

class Address{
private:
  double xcord;
  double ycord;
public:
  Address (double x, double y) {
    xcord = x;
    ycord = y;
    
  } 
  
  double getx(){
    return xcord;
  }
  double gety(){
    return ycord;
  }
  //Using the manhattan distance for a route on a grid
  double ManDistance(Address otheraddress) {
    double x1, x2, y1, y2;
    x1 = xcord;
    y1 = ycord;
    x2 = otheraddress.getx();
    y2 = otheraddress.gety();
    double xdif = x2 - x1;
    double ydif = y2 - y1;
    double distance = abs(xdif) + abs(ydif);
    return distance;
  }
  
};

class AddressList{
protected:
  vector<Address> alladdresses;
  vector<Address> groute;
  vector<Address> opt2route;
  vector<Address> Truck1;    
  vector<Address> Truck2;
  vector<Address> T1notprime;
  vector<Address> T2notprime;
public:
  void adddepot(vector<Address> &vec) {
    //adds ending and starting depot
    Address depot (0,0);
    vec.push_back(depot);
    vec.insert(vec.begin(), depot);
  }
  void rmdepot(vector<Address> &vec) {
    //removes ending and starting depot
    vec.erase(vec.begin());
    vec.erase(vec.end()-1);
  }
  void add_address(Address newaddress){
    alladdresses.push_back(newaddress);
    check_address();
    //inserts address then checks for duplicates
  }
  void check_address(){
    //Function to call to check for duplicates
    int k =0;
    for(int i = 0; i < alladdresses.size(); i++) {
      if(k==1){
	break;
      }
      for(int j =i+1; j < alladdresses.size(); j++) {
	if((alladdresses[i].getx() == alladdresses[j].getx())) {
	  if((alladdresses[i].gety() == alladdresses[j].gety())) {
	    k=1;
	    alladdresses.erase(alladdresses.begin()+j);
	    break;
	  }
	}
      }
    }
  }
  double length(vector<Address> &vec){
    //Calculates length from one address to another address
    double totaldist = 0;
    for (int i=0; i < vec.size(); i++){
      if (i == 0){
	double x = vec[i].getx();
	double y = vec[i].gety();
	totaldist = x + y;
	
      }else{
	double dist = vec[i].ManDistance(vec[i-1]);
	totaldist += dist;
      }
    }
    return totaldist;
  }
  int index_closest_to(Address otheraddress){
    //finds the next index of closest address to previous address to sort addresses
    double mindist, currentdist;
    int closestindex = 0;
    for (int i=0; i < alladdresses.size(); i++){
      if (i == 0){
	mindist = otheraddress.ManDistance(alladdresses[i]);
      }else{
	currentdist = otheraddress.ManDistance(alladdresses[i]);
	if (currentdist < mindist){
	  mindist = currentdist;
	  closestindex = i;
	}
      }
    }
    return closestindex;
  }
  
  vector<Address> greedy_route(){
    //Sorts addresses from closest to depot out to the furthest address then starts to sort back towards depot
    Address depot (0,0);
    // construct new list
    groute.clear();    
    // create address we_are_here; increments through route
    Address we_are_here = depot;
    // run for loop that find address closest to we_are_here
    vector<Address> temp;
    temp = alladdresses;
    for (int i = 0; i < temp.size(); i++){
      int wah_index = index_closest_to(we_are_here);
      we_are_here = alladdresses[wah_index];
      groute.push_back(we_are_here);
      alladdresses.erase(alladdresses.begin()+wah_index);
    }
    alladdresses = temp;
    adddepot(groute);
    return groute;
  }
  vector<Address> opt2_route(vector<Address> vec1){
    //opt2 route heuristic to run through the vector and check if new route length < current route length 
    vector<Address> tour = vec1;
    double new_dist; 
    double min_dist = length(vec1);
    int improve = 0;   
    //    iterate through address list
    while ( improve<20) {
      for (int i = 0; i < tour.size()-1; i++){
	
	for (int j = i+1; j < tour.size(); j++){
	  //           swap points
	  tour = twooptswap(i, j, tour);	     
	  new_dist = length(tour);	
	  // check if length is shorter
	  if (new_dist < min_dist){
	    improve = 0;
	    min_dist = new_dist;
	    vec1 = tour;
	    break;
	  }
	}
      }
      improve++;	   
    }
    vector<Address> route = vec1;
    return route;
  }; 
  void opt2_non_prime(vector<Address> temp1, vector<Address> temp2){
    //Opt 2 route to run for nonprime trucks as to not share addresses between a prime and nonprime truck    
    double l1, l2, len2base, len1base;
    int index;
    if(temp1.size() <= temp2.size()) {
      index = temp1.size();
    }
    else {
      index = temp2.size();
    }
    //    iterate through address list                                                                                            
    //Swaps values between the two trucks
    for(int i =-2; i < index; i++) {
      for(int j = i; j < index; j++) {
	len1base = veccalcnotprime(temp1, i);
	swap(temp1[i], temp2[j]);
	l1 = veccalcnotprime(temp1, i);
	if(l1 <len1base) {	  
	  break;
	}
	else {
	  swap(temp1[i], temp2[j]);
	}
      }
    }
    for(int i =0; i < index; i++) {
      for(int j = i; j < index; j++) {
	len1base = veccalcnotprime(temp1, i);
	swap(temp1[i], temp2[j]);
	l2 = veccalcnotprime(temp2, j);
	if(l2 <len2base) {
	  break;
	}
	else {
	  swap(temp1[i], temp2[j]);
	}
      }
    }
    //           swap points                                                                                           
    T1notprime = opt2_route(temp1); 	 
    T2notprime = opt2_route(temp2); 
  };
  
  double veccalcnotprime(vector<Address>tour, int i) {
    //Function to calc distance between two addresses in a tour
    double len;
    vector<Address> L1;
    L1.clear();
    L1.insert(L1.begin(), tour[i-1]);
    L1.insert(L1.begin()+1, tour[i]);
    len = length(L1);
    return len;
    
  }
  double veccalc(vector<Address>tour, int i) {
    //Function to calc distance between two addresses in a tour
    double len;
    Address depot(0,0);
    vector<Address> L1;
    L1.clear();
    L1.insert(L1.begin(), depot);
    L1.insert(L1.begin()+1, tour[i]);
    len = length(L1);
    return len;
    
  }
  vector<Address> twooptswap(int &i, int &k, vector<Address> tour) {
    //Swapping mechanism for opt2 heuristic
    double l1, l2;
    int dec =0;
    //take route[0] to route[i-1] and add them in order to new_route
    for ( int c = 0; c <= i - 1; ++c )
      {
	
	l1 = veccalc(tour, i-1);
	l2 = veccalc(tour, 0);
	//checks lengths and sorts      
	if(l2 < l1) {
	  tour.insert(tour.begin()+(i-1), tour[c]);
	  tour.erase(tour.begin()+(c));
	}
	else {
	  tour.insert(tour.begin()+(i), tour[c]);
	  tour.erase(tour.begin()+(c));
	}
      }
    //take route[i] to route[k] and add them in reverse order to new_route
    for ( int c = i; c <= k; ++c )
      {
	l1 = veccalc(tour, k);
	l2 = veccalc(tour, i);
	if(l2 > l1) {
	  tour.insert(tour.begin()+(k), tour[c]);
	  tour.erase(tour.begin()+(c));
	}
	else {
	  tour.insert(tour.begin()+(k), tour[c]);
	  tour.erase(tour.begin()+(c));
	}
	
      }
    //take route[k+1] to end and add them in order to new_route
    for ( int c = k + 1; c < tour.size(); ++c )
      {
	l1 = veccalc(tour, tour.size());
	l2 = veccalc(tour, k+1);
	if(l2 < l1) {
	  tour.insert(tour.end()-1, tour[c]);
	  tour.erase(tour.begin()+(c));
	}
	else {
	  tour.insert(tour.end()-1, tour[c]);
	  tour.erase(tour.begin()+(c));
	}
	
	
      }	 
    return tour;
  }
  void multtrucks() {
    //Allows for the creation of two trucks
    int randindex;
    Truck1.clear(); 
    vector<Address> temp = alladdresses;            
    //store those values in Truck1
    Truck2 =temp;
    for (int i=0; i < temp.size(); i++) {
      randindex = randomgen(Truck2.size());
      // erases value from Truck 2
      Truck1.push_back(temp[randindex]);
      Truck2.erase(Truck2.begin()+randindex);
      temp.erase(temp.begin()+randindex);
      
    }
    
    opt2_non_prime(Truck1, Truck2);
    Truck1 = opt2_route(Truck1);
    Truck2 = opt2_route(Truck2);
    adddepot(Truck1); 
    adddepot(Truck2);
  }
  
  int randomgen(int n) {  // Random number generator
    static std::default_random_engine gen(time(NULL));
    std::uniform_int_distribution<> dist(1,n);
    return dist(gen);
  }
  void print() {
    //The most disgusting print statement you've seen
    double t1len, t2len;
    double  multtot;
    opt2route = alladdresses;
    adddepot(alladdresses);
    //AllAddresses Vector
    double routelength = length(alladdresses);
    cout<< "Route from depot to depot: " << endl; 
    for(int i =0; i < alladdresses.size(); i++) {
      
      cout << "(" << alladdresses[i].getx() << ", " << alladdresses[i].gety() << ")";
    }  
    cout << endl;
    cout<< "Distance: " <<  routelength << endl; //depot to depot length
    
    //Greedy Route Vector
    cout<< "Greedy Route: " << endl;
    for(int i =0; i < groute.size(); i++) {
      
      cout << "(" << groute[i].getx() << ", " << groute[i].gety() << ")";
    }  
    cout << endl; 
    routelength = length(groute); //Greedy route length
    cout<< "Distance: " <<  routelength << endl;
    cout<< endl; 
    //Opt 2 vector
    opt2route = opt2_route(opt2route);
    adddepot(opt2route);
    cout<< "Opt2 Route: " << endl;
    for(int i =0; i < opt2route.size(); i++) {
      
      cout << "(" << opt2route[i].getx() << ", " << opt2route[i].gety() << ")";
    }
    cout << endl;
    routelength = length(opt2route); //opt2 optimized length
    cout<< "Distance: " <<  routelength << endl;
    cout << endl;
    //Truck1 prime vector
    for(int i =0; i < Truck1.size(); i++) {
      
      cout << "(" << Truck1[i].getx() << ", " << Truck1[i].gety() << ")";
    }
    cout << endl;
    t1len = length(Truck1); //opt2 optimized length Truck1                                                           
    cout<< "Distance: " <<  t1len << endl;
    
    //Truck2 prime vector
    for(int i =0; i < Truck2.size(); i++) {
      
      cout << "(" << Truck2[i].getx() << ", " << Truck2[i].gety() << ")";
    }
    cout << endl;
    t2len = length(Truck2); //opt2 optimized length Truck 2
    cout<< "Distance: " <<  t2len << endl;
    multtot =t1len+t2len;
    cout<< "Total 2 Truck Distance with prime: " <<  multtot << endl;
    cout<< endl;
    //Truck1 non prime vector
    adddepot(T1notprime);
    for(int i =0; i < T1notprime.size(); i++) {
      
      cout << "(" << T1notprime[i].getx() << ", " << T1notprime[i].gety() << ")";
    }
    cout << endl;
    t1len = length(T1notprime); //opt2 optimized length Truck1                                                                                                                                                    
    cout<< "Distance: " <<  t1len << endl;
    //Truck2 non prime vector
    adddepot(T2notprime);
    for(int i =0; i < T2notprime.size(); i++) {
      
      cout << "(" << T2notprime[i].getx() << ", " << T2notprime[i].gety() << ")";
    }
    cout << endl;
    t2len = length(T2notprime); //opt2 optimized length Truck 2                                                                                                                                                   
    cout<< "Distance: " <<  t2len << endl;
    multtot =t1len+t2len;
    cout<< "Total 2 Truck Distance without prime: " <<  multtot << endl;
    //Removes depots to run the next day's addresses
    rmdepot(alladdresses);
    rmdepot(opt2route);
    rmdepot(groute);
  }
};

class Route : public AddressList{
public:
  void set_addresses(int routesize){
    // create new addresses and place them in alladdresses
    int x, y;
    int gridsize = 30; // size to represent mapping
    for (int i=0; i < routesize; i++) {
      x = randomgen(gridsize);
      y = randomgen(gridsize);
      Address addy (x,y); // does not consider minimum days from shipping
      add_address(addy);
    }
  }
  void deliveries_done(){
    //on beginning of new day delete addresses already delivered and add more addresses   
    int rand = randomgen((alladdresses.size()/2));
    for (int i = 0; i < alladdresses.size(); i++) {
      alladdresses.erase(alladdresses.begin() + i);
      if (i == rand) {
	break;
      }
    }
  }
};

int main(){
  int num_addresses = 3;
  
  Route listofadds;
  for(int day = 1; day < 3; day++) {
    cout << endl;
    if(day > 1) {
      cout<< "Deleting deliveried addresses and adding: " << num_addresses << endl;
      cout << "Day #: " << day << endl;
    }
    else {
      cout << "Day #: " << day << " with " << num_addresses << " Addresses " << endl;
    } 
    //Calls all functions from route class
    listofadds.set_addresses(num_addresses);
    listofadds.greedy_route();
    listofadds.multtrucks();
    listofadds.print();
    listofadds.deliveries_done();
  }
  
  
  return 0;
}
