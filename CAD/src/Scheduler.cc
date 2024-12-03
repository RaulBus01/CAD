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


void Scheduler::initialize()
{
    NrUsers = par("gateSize").intValue();
    NrOfChannels = 10;//read from omnetpp.ini
    selfMsg = new cMessage("selfMsg");
    for(int i=0; i<10;i++){
           q[i]=0;
           NrBlocks[i]=0;
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

        std::vector<int> userIndexes(NrUsers);
        for (int i = 0; i < NrUsers; i++) {
            userIndexes[i] = i;
        }

        // Sort the user indexes based on the radio link quality

        std::sort(userIndexes.begin(), userIndexes.end(), [this](int i1, int i2) {
            return userLinkQualities[i1] > userLinkQualities[i2];
        });

        for (int i = 0; i < NrUsers; i++) {
            int userIndex = userIndexes[i];
            int nrBlocks = q[userIndex];

            if (nrBlocks > 0) {
                if (nrBlocks <= remainingBlocks) {
                    EV << "User " << userIndex << " gets " << nrBlocks << " blocks" << endl;
                    NrBlocks[userIndex] = nrBlocks;
                    remainingBlocks -= nrBlocks;
                } else {
                    EV << "User " << userIndex << " gets " << remainingBlocks << " blocks" << endl;
                    NrBlocks[userIndex] = remainingBlocks;
                    remainingBlocks = 0;
                }
            } else {
                NrBlocks[userIndex] = 0;
            }

            cMessage *cmd = new cMessage("cmd");
            cmd->addPar("nrBlocks");
            cmd->par("nrBlocks").setLongValue(NrBlocks[userIndex]);
            send(cmd, "txScheduling", userIndex);
            
        }
        scheduleAt(simTime() + par("schedulingPeriod").doubleValue(), selfMsg);
    }
}
