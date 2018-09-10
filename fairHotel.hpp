//
//  fairHotel.hpp
//  as5 pet hotel
//
//  Created by Junchen Zhang on 3/27/18.
//  Copyright © 2018 Junchen Zhang. All rights reserved.
//

#ifndef fairHotel_h
#define fairHotel_h

#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <cassert>

using namespace std;

class FairHotel {
    int birdCount = 0;
    int catCount = 0;
    int dogCount = 0;
    mutex mut;
    condition_variable catBegin;
    condition_variable birdBegin;
    condition_variable dogBegin;
    int catCall = 0;
    int birdCall = 0;
    int dogCall = 0;
    
public:
    void bird() {
        
        {
            unique_lock<mutex> locker(mut);
            while (catCount != 0 || birdCall > catCall || birdCall > dogCall) {
                birdBegin.wait(locker);
            }
            assert(catCount == 0);
            birdCount++;
            if (catCount == 0 && dogCall <= birdCall && dogCall <= catCall) {
                dogBegin.notify_all();
            }
        }
        play();
        {
            unique_lock<mutex> locker(mut);
            birdCount--;
            birdCall++;
            if (birdCount == 0 && dogCount == 0 && catCall <= birdCall && catCall <= dogCall) {
                catBegin.notify_all();
            }
            if (catCount == 0 && dogCall <= birdCall && dogCall <= catCall) {
                dogBegin.notify_all();
            }
        }
    }
    
    void cat() {
        {
            unique_lock<mutex> locker(mut);
            while (birdCount != 0 || dogCount != 0 || catCall > birdCall || catCall > dogCall) {
                catBegin.wait(locker);
            }
            assert(birdCount == 0 && dogCount == 0);
            catCount++;
        }
        play();
        {
            unique_lock<mutex> locker(mut);
            catCount--;
            catCall++;
            if (catCount == 0 && birdCall <= catCall && birdCall <= dogCall) {
                birdBegin.notify_all();
            }
            if (catCount == 0 && dogCall <= birdCall && dogCall <= catCall) {
                dogBegin.notify_all();
            }
        }
    }
    
    void dog() {
        {
            unique_lock<mutex> locker(mut);
            while (catCount != 0 || dogCall > birdCall || dogCall > catCall) {
                dogBegin.wait(locker);
            }
            assert(catCount == 0);
            dogCount++;
            if (catCount == 0 && birdCall <= catCall && birdCall <= dogCall) {
                birdBegin.notify_all();
            }
        }
        play();
        {
            unique_lock<mutex> locker(mut);
            dogCount--;
            dogCall++;
            if (catCount == 0 && birdCall <= catCall && birdCall <= dogCall) {
                birdBegin.notify_all();
            }
            if (birdCount == 0 && dogCount == 0 && catCall <= birdCall && catCall <= dogCall) {
                catBegin.notify_all();
            }
        }
    }
    
    void play() const;
};


#endif /* fairHotel_h */
