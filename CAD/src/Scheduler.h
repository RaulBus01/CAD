//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __SCHED_TST_SCHEDULER_H_
#define __SCHED_TST_SCHEDULER_H_

#include <omnetpp.h>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class User{
public:
    User(int index, int weight)
    {
        this->index = index;
        this->weight = weight;
        this->lastTimeServed = 0;
        this->queueLength = 0;
    }

    int addQueueLength()
    {
        this->queueLength++;
        return this->queueLength;
    }

    int decrementQueueLength(int subtrahend)
    {
        this->queueLength -= subtrahend;
        return this->queueLength;
    }

    int getQueueLength()
    {
        return this->queueLength;
    }

    double updateLastTimeServed(double currentSimTime)
    {
        this->lastTimeServed = currentSimTime;
        return this->lastTimeServed;
    }

    double getUserPriority(double currentSimTime)
    {
        if(this->lastTimeServed < 0)
        {
            return this->weight;
        }
        return this->weight*(currentSimTime - this->lastTimeServed);
    }

    int getUserIndex()
    {
        return this->index;
    }

    int getUserWeight()
        {
            return this->weight;
        }
    void updateDelayStats(double delay) {
        totalDelay += delay;
        packetCount++;
    }
    
    double getAverageDelay() const {
        return packetCount > 0 ? totalDelay / packetCount : 0.0;
    }
    
    int getPacketCount() const {
        return packetCount;
    }
private:
    User();
    int index;
    int weight;
    double totalDelay;
    int packetCount;
    int queueLength;
    double lastTimeServed;
};


class Scheduler : public cSimpleModule
{
public:
    Scheduler();
    ~Scheduler();
private:
    cMessage *selfMsg;
    int NrUsers;
    std::vector<User> users;
    int NrOfChannels;
     std::vector<simsignal_t> userDelaySignals;
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
