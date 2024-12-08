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

#include "Sink.h"
#include <numeric>
#include <fstream>
Define_Module(Sink);

void Sink::initialize()
{
    // lifetimeSignal = registerSignal("lifetime");
    // delaySignalHighPriority = registerSignal("delayHighPriority");
    // delaySignalMediumPriority = registerSignal("delayMediumPriority");
    // delaySignalLowPriority = registerSignal("delayLowPriority");
    csvFile.open("delays.csv");
    csvFile << "Time,UserIndex,Priority,Delay\n"; // Header

}
void Sink::writeToCSV(int userIndex, int priority, simtime_t delay) {
    csvFile << simTime() << "," 
            << userIndex << "," 
            << priority << "," 
            << delay << "\n";
}

void Sink::handleMessage(cMessage *msg)
{
    simtime_t lifetime = simTime() - msg->getCreationTime();
    //  emit(lifetimeSignal, lifetime);
     if(msg->arrivedOn("rxPackets"))
     {
        EV << "Received " << msg->getName() << ", lifetime: " << lifetime << "s" << endl;
        int userPriority = (int)msg->par("userPriorityType");
        int userIndex = (int)msg->par("userIndex");
        writeToCSV(userIndex,userPriority, lifetime);
        // switch(userPriority){
        //     case 3:
        //         delayHighPriority.push_back(lifetime);
        //         emit(delaySignalHighPriority, lifetime);
        //         EV << "Priority 3 packet delay: " << lifetime << "s" << endl;
        //         break;
        //     case 2:
        //         delayMediumPriority.push_back(lifetime);
        //         emit(delaySignalMediumPriority, lifetime);
        //         EV << "Priority 2 packet delay: " << lifetime << "s" << endl;
        //         break;
        //     case 1:
        //         delayLowPriority.push_back(lifetime);
        //         emit(delaySignalLowPriority, lifetime);
        //         EV << "Priority 1 packet delay: " << lifetime << "s" << endl;
        //         break;
        // }
     }
     
    
      delete msg;
}
void Sink::finish()
{
    
    csvFile.close();
    
    // Calculate and record final statistics
   
}