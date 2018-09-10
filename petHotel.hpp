#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <cassert>

using namespace std;

/*
  Since the test starts with bird, so most time cats are not allowed in but dogs are allowed in. And the rule determines that birds and dogs are more likely to play than cats because less restriction. Only when number of cats is much bigger than dogs and birds, its total count is bigger than dog's and bird's. However, when number of cats is almost same as or less than or a little bigger than dogs and birds, its total count is much less than dog's and bird's.
*/

class PetHotel {
    int birdCount = 0;
    int catCount = 0;
    int dogCount = 0;
    mutex mut;
    condition_variable noCat;
    condition_variable noDogBird;
    
public:
    void bird() {
        {
            unique_lock<mutex> locker(mut);
            while (catCount != 0) {
                noCat.wait(locker);
            }
            assert(catCount == 0);
            birdCount++;
        }
        play();
        {
            unique_lock<mutex> locker(mut);
            birdCount--;
            if (birdCount == 0 && dogCount == 0) {
                noDogBird.notify_all();
            }
        }
    }
    
    void cat() {
        {
            unique_lock<mutex> locker(mut);
            while (birdCount != 0 || dogCount != 0) {
                noDogBird.wait(locker);
            }
            assert(birdCount == 0 && dogCount == 0);
            catCount++;
        }
        play();
        {
            unique_lock<mutex> locker(mut);
            catCount--;
            if (catCount == 0) {
                noCat.notify_all();
            }
        }
    }
    
    void dog() {
        {
            unique_lock<mutex> locker(mut);
            while (catCount != 0) {
                noCat.wait(locker);
            }
            assert(catCount == 0);
            dogCount++;
        }
        play();
        {
            unique_lock<mutex> locker(mut);
            dogCount--;
            if (dogCount == 0 && birdCount == 0) {
                noDogBird.notify_all();
            }
        }
    }
    
    void play() const;
};
