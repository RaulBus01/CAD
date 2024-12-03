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

#include "Scheduler.h"
#include <algorithm>
Define_Module(Scheduler);



Scheduler::Scheduler()
{
    selfMsg = nullptr;
}

Scheduler::~Scheduler()
{
    cancelAndDelete(selfMsg);
}


bool sort_functionByUserWeight(const UserInfo &a, const UserInfo &b)
{
    return a.userWeight > b.userWeight;
}


void Scheduler::initialize()
{
    NrUsers = par("gateSize").intValue();
    NrOfChannels = 10;//read from omnetpp.ini
    selfMsg = new cMessage("selfMsg");
    for(int i=0; i<10;i++){
           q[i]=0;
           NrBlocks[i]=0;
      }

    int HIGH_PRIORITY = 3;
    int MEDIUM_PRIORITY = 2;
    int LOW_PRIORITY = 1;

    users.resize(NrUsers);
    for (int i = 0; i < NrUsers; i++) {
        double radioLinkQuality = getParentModule()->getSubmodule("user", i)->par("radioLinkQuality").doubleValue();
        double initialLastTimeServed = -1.0f;
        int initialQueueLength = 0;

        if( i % 3 == 0){
            users[i] = {i, HIGH_PRIORITY, initialQueueLength, radioLinkQuality, initialLastTimeServed};
        }else if( i % 3 == 1){
            users[i] = {i, MEDIUM_PRIORITY, initialQueueLength, radioLinkQuality, initialLastTimeServed};
        }else{
            users[i] = {i, LOW_PRIORITY, initialQueueLength, radioLinkQuality, initialLastTimeServed};
        }

        EV << "User " << i << " has radio link quality " << radioLinkQuality << endl;
        
    }

 
    scheduleAt(simTime(), selfMsg);


}

void Scheduler::handleMessage(cMessage *msg)
{
    int NrBlocks[NrUsers];

    int userWeights[NrUsers];
    for (int i = 0; i < NrUsers; i++) {
        if (msg->arrivedOn("rxInfo", i)) {
            if (msg->hasPar("queueLengthInfo")) {
                q[i] = msg->par("queueLengthInfo");
                EV << "Update: q[" << i << "]= " << q[i] << endl;

                // Obtain the user index from the gate index
                int userIndex = msg->getArrivalGate()->getIndex();
                // Read the radio link quality from the message
                userLinkQualities[userIndex] = msg->par("radioLinkQuality").doubleValue();
                EV << "Radio link quality for user " << userIndex << " is " << userLinkQualities[userIndex] << endl;

                users[userIndex].radioLinkQuality = userLinkQualities[userIndex];
                users[userIndex].queueLength = q[i];
                
                EV << "User " << userIndex << " HAS WEIGHT " << users[userIndex].userWeight << endl;
                delete msg;
            } else {
                EV << "Warning: Received message without queueLengthInfo parameter" << endl;
                delete msg;
            }
        }
    }

    if (msg == selfMsg) {
        int remainingBlocks = NrOfChannels;
        EV << "remainingBlocks = " << remainingBlocks << endl;

        // std::vector<int> userIndexes(NrUsers);
        // for (int i = 0; i < NrUsers; i++) {
        //     userIndexes[i] = i;
        // }

        // Sort the user indexes based on the radio link quality

        // std::sort(userIndexes.begin(), userIndexes.end(), [this](int i1, int i2) {
        //     return userLinkQualities[i1] > userLinkQualities[i2];
        // });

        std::sort(users.begin(), users.end(), sort_functionByUserWeight);

         for (const auto &user : users) {
            EV << "User " << user.index << " has radio link quality " << user.radioLinkQuality << " and queue length " << user.queueLength << endl;
            if (remainingBlocks <= 0) {
                break;
            }
            int nrBlocks = user.queueLength;
            
            if (nrBlocks > 0) {
                if (nrBlocks <= remainingBlocks) {
                    NrBlocks[user.index] = nrBlocks;
                    remainingBlocks -= nrBlocks;
                } else {
                    NrBlocks[user.index] = remainingBlocks;
                    remainingBlocks = 0;
                }
            } else {
                NrBlocks[user.index] = 0;
            }
            EV << "User " << user.index << " has " << NrBlocks[user.index] << " blocks to transmit" << endl;

            // Declare and initialize the cmd variable
            cMessage *cmd = new cMessage("cmd");
            cmd->addPar("nrBlocks");
            cmd->par("nrBlocks").setLongValue(NrBlocks[user.index]);
            send(cmd, "txScheduling", user.index);
        }
        scheduleAt(simTime() + par("schedulingPeriod").doubleValue(), selfMsg);
    

      
    }
}
