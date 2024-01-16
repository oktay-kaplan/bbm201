#include "Network.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <algorithm>

#include <sstream>

Network::Network() {

}

void Network::process_commands(vector<Client> &clients, vector<string> &commands, int message_limit,
                               const string &sender_port, const string &receiver_port) {
    // TODO: Execute the commands given as a vector of strings while utilizing the remaining arguments.
    /* Don't use any static variables, assume this method will be called over and over during testing.
     Don't forget to update the necessary member variables after processing each command. For example,
     after the MESSAGE command, the outgoing queue of the sender must have the expected frames ready to send. */


    for (const string &command : commands) {
        stringstream ss(command);
        string cmdType;
        ss >> cmdType;

        for (int i = 0; i < command.size()+9; ++i) {
            cout << "-";
        }
        std::cout<<endl;

        if (cmdType == "MESSAGE") {

            std::cout << "Command: " << command << endl;
            for (int i = 0; i < command.size()+9; ++i) {
                cout << "-";
            }
            std::cout<<endl;

            string senderID, receiverID, message;
            ss >> senderID >> receiverID;

            char firstChar;
            ss >> firstChar;

            getline(ss, message, '#');  // Read the message until the closing hashtag

            // Find the sender client in the clients vector
            auto senderClientIt = find_if(clients.begin(), clients.end(), [senderID](const Client& client) {

                return client.client_id == senderID;
            });


            // Find the sender client in the clients vector
            auto receiverClientIt = find_if(clients.begin(), clients.end(), [receiverID](const Client& client) {
                return client.client_id == receiverID;
            });

            if (senderClientIt != clients.end()) {

                string assembledMessage ="";


                cout << "Message to be sent: \"" << message << "\"\n" << std::endl;
                Client& senderClient = *senderClientIt;


                string carrierID = senderClient.routing_table[receiverID];

                // Find the sender client in the clients vector
                auto carrierClientIt = find_if(clients.begin(), clients.end(), [carrierID](const Client& client) {
                    return client.client_id == carrierID;

                });


                // Split the message into chunks and create frames
                vector<string> messageChunks = split_message(message, message_limit);
                int numChunks = messageChunks.size();


                // Output information for each frame
                for (int i = 0; i < numChunks; ++i) {
                    stack<Packet*> frame;

                    // Create Application Layer Packet on the heap
                    ApplicationLayerPacket* appPacket = new ApplicationLayerPacket(0, senderID, receiverID, messageChunks[i]);

                    appPacket->current_sender_ID = senderID;

                    // Create Transport Layer Packet on the heap
                    TransportLayerPacket* transportPacket = new TransportLayerPacket(1, sender_port, receiver_port);

                    // Create Network Layer Packet on the heap
                    NetworkLayerPacket* networkPacket = new NetworkLayerPacket(2, senderClient.client_ip, receiverClientIt->client_ip);

                    // Create Physical Layer Packet on the heap
                    PhysicalLayerPacket* physicalPacket = new PhysicalLayerPacket(3, senderClient.client_mac, carrierClientIt->client_mac);

                    frame.push(appPacket);
                    frame.push(transportPacket);
                    frame.push(networkPacket);
                    frame.push(physicalPacket);
                    frame.top()->frame_ID = i+1;
                    frame.top()->number_of_hops = 0;


                    cout << "Frame #" << frame.top()->frame_ID << endl;
                    stack<Packet*> tempStack = frame;

                    // Print information for each layer in the frame
                    while (!tempStack.empty()) {
                        if (ApplicationLayerPacket* appLayerPacket = dynamic_cast<ApplicationLayerPacket*>(tempStack.top())) {
                            assembledMessage += appLayerPacket->message_data;
                        }
                        tempStack.top()->print();
                        tempStack.pop();
                    }

                    cout << "Number of hops so far: "<<frame.top()->number_of_hops << endl;
                    cout << "--------" << endl;

                    senderClient.outgoing_queue.push(frame);
                }

                if(assembledMessage.size()>0)
                    senderClient.log_entries.emplace_back(get_timestamp(), assembledMessage, numChunks, 0, senderID, receiverID, true, ActivityType::MESSAGE_SENT);

            }

            // TODO: Implement logic to create and queue message frames in the network

        }else if (cmdType == "SHOW_FRAME_INFO") {
            std::cout << "Command: " << command << std::endl;
            for (int i = 0; i < command.size() + 9; ++i) {
                std::cout << "-";
            }
            std::cout << std::endl;

            // Process SHOW_FRAME_INFO command
            string clientID, queueSelection;
            int frameNumber;
            ss >> clientID >> queueSelection >> frameNumber;

            // Find the sender client in the clients vector
            auto it = find_if(clients.begin(), clients.end(), [clientID](const Client& client) {
                return client.client_id == clientID;
            });

            // ...

            if (it != clients.end()) {
                // Check if queueSelection is "in" (incoming) or "out" (outgoing)
                if (queueSelection == "in") {
                    // TODO: Implement logic to display frame information from the incoming queue.

                    // Check if the specified frameNumber is within the range of the incoming queue
                    if (frameNumber >= 1 && frameNumber <= it->incoming_queue.size()) {
                        stack<Packet*> tempStack;

                        queue<stack<Packet*>> originalQueue = it->incoming_queue;

                        for (int i = 1; i < frameNumber; ++i) {
                            originalQueue.pop();
                        }

                        tempStack = originalQueue.front();



                        std::cout << "Current Frame #" << frameNumber << " on the incoming queue of client " << clientID << std::endl;

                        while (!tempStack.empty()) {
                            std::cout << "Layer " << tempStack.top()->layer_ID << " info: ";

                            tempStack.top()->print();
                            tempStack.pop();
                        }

                        cout << "Number of hops so far: "<<originalQueue.front().top()->number_of_hops << endl;
                    } else {
                        std::cout << "No such frame." << std::endl;
                    }
                }
                else if (queueSelection == "out") {
                    // Check if the specified frameNumber is within the range of the outgoing queue
                    if (frameNumber >= 1 && frameNumber <= it->outgoing_queue.size()) {
                        stack<Packet*> tempStack;

                        queue<stack<Packet*>> originalQueue = it->outgoing_queue;

                        for (int i = 1; i < frameNumber; ++i) {
                            originalQueue.pop();
                        }

                        tempStack = originalQueue.front();

                        std::cout << "Current Frame #" << frameNumber << " on the outgoing queue of client " << clientID << std::endl;

                        while (!tempStack.empty()) {
                            std::cout << "Layer " << tempStack.top()->layer_ID << " info: ";

                            tempStack.top()->print();
                            tempStack.pop();
                        }

                        cout << "Number of hops so far: "<<originalQueue.front().top()->number_of_hops << endl;
                    } else {
                        std::cout << "No such frame."<< std::endl;
                    }
                } else {
                    std::cout << "Error: Invalid queue selection - " << queueSelection << std::endl;
                }
            } else {
                std::cout << "Error: Client " << clientID << " not found." << std::endl;
            }
        }
        else if (cmdType == "SHOW_Q_INFO") {
            std::cout << "Command: " << command << std::endl;
            for (int i = 0; i < command.size() + 9; ++i) {
                std::cout << "-";
            }
            std::cout << std::endl;

            // Process SHOW_Q_INFO command
            string clientID, queueSelection;
            ss >> clientID >> queueSelection;

            // Find the client in the vector
            auto it = std::find_if(clients.begin(), clients.end(), [clientID](const Client& client) {
                return client.client_id == clientID;
            });

            if (it != clients.end()) {
                if (queueSelection == "in") {
                    std::cout << "Client " << clientID << " Incoming Queue Status" << std::endl;
                    std::cout << "Current total number of frames: " << it->incoming_queue.size() << std::endl;
                } else if (queueSelection == "out") {
                    std::cout << "Client " << clientID << " Outgoing Queue Status" << std::endl;
                    std::cout << "Current total number of frames: " << it->outgoing_queue.size() << std::endl;
                } else {
                    std::cout << "Error: Invalid queue selection - " << queueSelection << std::endl;
                }
            } else {
                std::cout << "Error: Client " << clientID << " not found." << std::endl;
            }
        }
        else if (cmdType == "SEND") {
            std::cout << "Command: " << command << std::endl;
            for (int i = 0; i < command.size() + 9; ++i) {
                std::cout << "-";
            }
            std::cout << std::endl;

            // Iterate through all clients in the network
            for (Client& senderClient : clients) {
                string assembledMessage ="";
                int number_control = 0;
                while (!senderClient.outgoing_queue.empty()) {

                    number_control ++;
                    stack<Packet*> frame = senderClient.outgoing_queue.front();

                    senderClient.outgoing_queue.pop();

                    string receiverID;
                    string senderID;

                    stack<Packet*> tempStack1 = frame;

                    while (!tempStack1.empty()) {
                        if (ApplicationLayerPacket* appLayerPacket = dynamic_cast<ApplicationLayerPacket*>(tempStack1.top())) {
                            // It's an ApplicationLayerPacket, you can access its properties
                            receiverID = appLayerPacket->receiver_ID;
                            senderID = appLayerPacket->sender_ID;


                        }
                        tempStack1.pop();
                    }


                    // Find the next hop client ID based on the routing table
                    string nextHopID = senderClient.routing_table[receiverID];

                    // Find the next hop client in the clients vector
                    auto nextHopClientIt = find_if(clients.begin(), clients.end(), [nextHopID](const Client& client) {
                        return client.client_id == nextHopID;
                    });

                    // Check if the next hop client exists
                    if ((nextHopClientIt != clients.end())) {



                        if(frame.top()->frame_ID ==1 && number_control>0 && assembledMessage.size()>0){
                            if(senderID != senderClient.client_id){
                                senderClient.log_entries.emplace_back(get_timestamp(), assembledMessage, nextHopClientIt->incoming_queue.size(), frame.top()->number_of_hops, senderID, receiverID, true, ActivityType::MESSAGE_FORWARDED);
                                number_control --;

                            }


                        }


                        nextHopClientIt->incoming_queue.push(frame);

                        if(frame.top()->frame_ID ==1 )
                            assembledMessage = "";


                        stack<Packet*> tempStack2 = frame;


                        std::cout << "Client " << senderClient.client_id << " sending frame #"<< tempStack2.top()->frame_ID<< " to client " << nextHopClientIt->client_id << std::endl;


                        // Print information for each layer in the frame
                        while (!tempStack2.empty()) {
                            if (ApplicationLayerPacket* appLayerPacket = dynamic_cast<ApplicationLayerPacket*>(tempStack2.top())) {
                                assembledMessage += appLayerPacket->message_data;
                            }
                            tempStack2.top()->print();
                            tempStack2.pop();
                        }
                        if(senderClient.outgoing_queue.empty() && nextHopClientIt->incoming_queue.size()-number_control != 0){
                            if(senderID != senderClient.client_id){
                                senderClient.log_entries.emplace_back(get_timestamp(), assembledMessage, nextHopClientIt->incoming_queue.size()-number_control, frame.top()->number_of_hops, senderID, receiverID, true, ActivityType::MESSAGE_FORWARDED);

                                assembledMessage = "";

                            }

                        }
                        if(senderClient.outgoing_queue.empty() && nextHopClientIt->incoming_queue.size()-number_control == 0){
                            if(senderID != senderClient.client_id){
                                senderClient.log_entries.emplace_back(get_timestamp(), assembledMessage, nextHopClientIt->incoming_queue.size(), frame.top()->number_of_hops, senderID, receiverID, true, ActivityType::MESSAGE_FORWARDED);

                                assembledMessage = "";

                            }

                        }
                        frame.top()->number_of_hops++;

                        cout << "Number of hops so far: "<<frame.top()->number_of_hops << endl;

                        std::cout<<"--------"<<endl;

                    } else {
                        // Log the error if the next hop client is unreachable
                        senderClient.log_entries.emplace_back(get_timestamp(), "Unreachable Destination", 1, frame.top()->number_of_hops, senderID, receiverID, false, ActivityType::MESSAGE_DROPPED);
                        std::cout << "Error: Unreachable destination. Packets are dropped after "<<frame.top()->number_of_hops<<" hops!" << std::endl;

                        stack<Packet*> tempStack = frame;



                        while (!tempStack.empty()) {


                            delete tempStack.top();
                            tempStack.pop();
                        }
                    }

                }
            }
        }
        else if (cmdType == "RECEIVE") {
            std::cout << "Command: " << command << endl;
            for (int i = 0; i < command.size() + 9; ++i) {
                cout << "-";
            }
            std::cout << endl;

            // Process RECEIVE command
            for (Client& receiverClient : clients) {
                int number_control =0;
                int number_control2 =0;
                string assembledMessage;

                while (!receiverClient.incoming_queue.empty()) {

                    stack<Packet*> frame = receiverClient.incoming_queue.front();
                    receiverClient.incoming_queue.pop();


                    string receiverID;
                    string senderID;
                    string current_senderID;


                    stack<Packet*> tempStack = frame;

                    while (!tempStack.empty()) {
                        if (ApplicationLayerPacket* appLayerPacket = dynamic_cast<ApplicationLayerPacket*>(tempStack.top())) {
                            receiverID = appLayerPacket->receiver_ID;
                            senderID = appLayerPacket->sender_ID;
                            current_senderID = appLayerPacket->current_sender_ID;
                        }
                        tempStack.pop();
                    }

                    if(frame.top()->frame_ID ==1 && number_control>0 && assembledMessage.size()>0){
                        cout << "Client " << receiverClient.client_id << " received the message \"" << assembledMessage << "\" from client "<<senderID<<"." << endl;
                        std::cout<< "--------"<<endl;
                        receiverClient.log_entries.emplace_back(get_timestamp(), assembledMessage, number_control, frame.top()->number_of_hops, senderID, receiverID, true, ActivityType::MESSAGE_RECEIVED);

                        number_control = 0;

                        assembledMessage = "";


                    }
                    else if(frame.top()->frame_ID ==1 && number_control>0){
                        number_control2 = 0;

                        std::cout<< "--------"<<endl;
                    }
                    number_control++;
                    number_control2++;


                    // Check if the receiver ID matches the client's ID
                    if (receiverID == receiverClient.client_id) {
                        // The frame has reached its final destination

                        stack<Packet*> tempStack2 = frame;


                        std::cout << "Client " << receiverClient.client_id << " receiving frame #"<< tempStack2.top()->frame_ID<<
                                  " from client " << current_senderID << ", originating from client " << senderID
                                  << std::endl;





                        // Print information for each layer in the frame
                        while (!tempStack2.empty()) {

                            if (ApplicationLayerPacket* appLayerPacket = dynamic_cast<ApplicationLayerPacket*>(tempStack2.top())) {
                                // It's an ApplicationLayerPacket, you can access its properties
                                assembledMessage += appLayerPacket->message_data;
                            }
                            tempStack2.top()->print();
                            tempStack2.pop();
                        }


                        cout << "Number of hops so far: "<<frame.top()->number_of_hops << endl;



                        if(receiverClient.incoming_queue.empty()){
                            std::cout<<"--------"<<endl;
                            receiverClient.log_entries.emplace_back(get_timestamp(), assembledMessage, number_control, frame.top()->number_of_hops, senderID, receiverID, true, ActivityType::MESSAGE_RECEIVED);


                            cout << "Client " << receiverClient.client_id << " received the message \"" << assembledMessage << "\" from client "<<senderID<<"." << endl;


                            assembledMessage = "";

                        }
                        std::cout<<"--------"<<endl;


                        stack<Packet*> tempStack = frame;



                        while (!tempStack.empty()) {

                            delete tempStack.top();
                            tempStack.pop();
                        }



                    } else {
                        // Find the next hop client ID based on the routing table
                        string nextHopID = receiverClient.routing_table[receiverID];

                        // Find the next hop client in the clients vector
                        auto nextHopClientIt = find_if(clients.begin(), clients.end(), [nextHopID](const Client& client) {
                            return client.client_id == nextHopID;
                        });

                        string receive_ID = receiverClient.client_id;

                        // Find the next hop client in the clients vector
                        auto current = find_if(clients.begin(), clients.end(), [receive_ID](const Client& client) {
                            return client.client_id == receive_ID;
                        });

                        // Check if the next hop client exists
                        if ((nextHopClientIt != clients.end())) {




                            stack<Packet*> tempStack = frame;
                            if(tempStack.top()->frame_ID ==1){
                                std::cout << "Client " << receiverClient.client_id << " receiving a message "<<
                                          "from client " <<current_senderID  << ", but intended for client " << receiverID << ". Forwarding..."
                                          << std::endl;

                            }

                            // Print information for each layer in the frame
                            std::cout << "Frame #" <<tempStack.top()->frame_ID<< " MAC address change: New sender MAC " <<receiverClient.client_mac << ", new receiver MAC  "<< nextHopClientIt->client_mac<<endl;


                            // Iterate through each layer in the frame and update MAC addresses
                            while (!tempStack.empty()) {
                                // Assuming Packet class has getters and setters for sender and receiver MAC addresses
                                if (PhysicalLayerPacket* physicalLayerPacket = dynamic_cast<PhysicalLayerPacket*>(tempStack.top())) {
                                    physicalLayerPacket->setSenderMAC(receiverClient.client_mac);
                                    physicalLayerPacket->setReceiverMAC(nextHopClientIt->client_mac);
                                } // Assuming Packet class has getters and setters for sender and receiver MAC addresses
                                if (ApplicationLayerPacket* applicationLayerPacket = dynamic_cast<ApplicationLayerPacket*>(tempStack.top())) {
                                    applicationLayerPacket->setCurrentSenderID(receiverClient.client_id);

                                }


                                tempStack.pop();
                            }

                            if(receiverClient.incoming_queue.empty()){
                                std::cout<<"--------"<<endl;


                            }

                            current->outgoing_queue.push(frame);

                        } else {
                            std::cout << "Client " << receiverClient.client_id << " receiving frame #"<<frame.top()->frame_ID<<
                                      " from client " <<current_senderID  << ", but intended for client " << receiverID << ". Forwarding..."
                                      << std::endl;
                            if(number_control2 == 1 && number_control>1 ){
                                receiverClient.log_entries.emplace_back(get_timestamp(), "Unreachable Destination", number_control-number_control2, frame.top()->number_of_hops, senderID, receiverID, false, ActivityType::MESSAGE_DROPPED);


                            }
                            std::cout << "Error: Unreachable destination. Packets are dropped after "<<frame.top()->number_of_hops<<" hops!" << std::endl;


                            if(receiverClient.incoming_queue.empty()){
                                receiverClient.log_entries.emplace_back(get_timestamp(), "Unreachable Destination", number_control2, frame.top()->number_of_hops, senderID, receiverID, false, ActivityType::MESSAGE_DROPPED);
                                std::cout<<"--------"<<endl;



                            }
                            stack<Packet*> tempStack = frame;



                            while (!tempStack.empty()) {


                                delete tempStack.top();
                                tempStack.pop();
                            }


                        }

                    }
                }

                if(assembledMessage.size()>0){
                    cout << "Client " << receiverClient.client_id << " received the message \"" << assembledMessage << "\" from client "<<"D"<<"." << endl;


                    std::cout<< "--------"<<endl;
                    assembledMessage = "";

                }
            }
        }
        else if (cmdType == "PRINT_LOG") {
            std::cout << "Command: " << command << std::endl;
            for (int i = 0; i < command.size() + 9; ++i) {
                std::cout << "-";
            }
            std::cout << std::endl;

            // Process PRINT_LOG command
            std::string clientID;
            ss >> clientID;

            // Find the client in the vector
            auto it = std::find_if(clients.begin(), clients.end(), [clientID](const Client& client) {
                return client.client_id == clientID;
            });

            if (it != clients.end()) {

                // Check if the client has any log entries
                if (!it->log_entries.empty()) {

                    std::cout << "Client " << clientID << " Logs:" << std::endl;
                    int number_entry = 1;



                    // Iterate through each log entry and print the information
                    for (const Log& logEntry : it->log_entries) {
                        std::cout << "--------------" << std::endl;
                        std::cout << "Log Entry #" << number_entry << ":" << std::endl;
                        std::cout << "Activity: " << activityTypeToString(logEntry.activity_type) << std::endl;
                        std::cout << "Timestamp: " << logEntry.timestamp << std::endl;
                        std::cout << "Number of frames: " << logEntry.number_of_frames << std::endl;
                        std::cout << "Number of hops: " << logEntry.number_of_hops << std::endl;
                        std::cout << "Sender ID: " << logEntry.sender_id << std::endl;
                        std::cout << "Receiver ID: " << logEntry.receiver_id << std::endl;
                        std::cout << "Success: " << (logEntry.success_status ? "Yes" : "No") << std::endl;
                        number_entry++;

                        // Print additional message if available (for MESSAGE activity type)
                        if (logEntry.activity_type == ActivityType::MESSAGE_SENT || logEntry.activity_type == ActivityType::MESSAGE_RECEIVED ) {
                            std::cout << "Message: \"" << logEntry.message_content << "\"" << std::endl;
                        }

                    }
                } else {
                }
            } else {
                std::cout << "Error: Client " << clientID << " not found." << std::endl;
            }
        }
        else {
            // Invalid command
            std::cout << "Command: " << command << endl;
            for (int i = 0; i < command.size()+9; ++i) {
                cout << "-";
            }
            std::cout<<endl<<"Invalid command."<<endl;
        }
    }
}

vector<Client> Network::read_clients(const string &filename) {
    // TODO: Read clients from the given input file and return a vector of Client instances.

    vector<Client> clients;
    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return clients;
    }

    int numClients;
    inputFile >> numClients; // Read the number of clients

    for (int i = 0; i < numClients; ++i) {
        string id, ip, mac;
        inputFile >> id >> ip >> mac;
        Client client(id, ip, mac);
        clients.push_back(client);
    }

    inputFile.close();

    return clients;
}

void Network::read_routing_tables(vector<Client> &clients, const string &filename) {
    // TODO: Read the routing tables from the given input file and populate the clients' routing_table member variable.
    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    size_t clientIndex = 0;

    while (getline(inputFile, line)) {
        if (line == "-") {
            ++clientIndex;
            continue;
        }

        istringstream iss(line);
        string destination, nextHop;

        if (!(iss >> destination >> nextHop)) {
            cerr << "Error parsing line: " << line << endl;
            continue;
        }

        if (clientIndex < clients.size()) {
            // Check if the client exists in the vector
            clients[clientIndex].routing_table[destination] = nextHop;
        } else {
            cerr << "Invalid client index: " << clientIndex << endl;
        }
    }

    inputFile.close();
}

// Returns a list of token lists for each command
vector<string> Network::read_commands(const string &filename) {
    // TODO: Read commands from the given input file and return them as a vector of strings.

    vector<string> commands;

    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return commands;
    }

    int numCommands;
    inputFile >> numCommands; // Read the number of commands

    string line;
    getline(inputFile, line);

    for (int i = 0; i < numCommands; ++i) {
        getline(inputFile, line);
        commands.push_back(line);
    }

    inputFile.close();

    return commands;
}

Network::~Network() {
    // TODO: Free any dynamically allocated memory if necessary.
}

std::vector<std::string> Network:: split_message(const std::string& message, int chunk_size) {
    std::vector<std::string> chunks;
    for (size_t i = 0; i < message.length(); i += chunk_size) {
        chunks.push_back(message.substr(i, chunk_size));
    }
    return chunks;
}
std::string Network:: get_timestamp() {
    auto now = std::chrono::system_clock::now();

    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm local_tm = *std::localtime(&time);

    std::stringstream timestamp;
    timestamp << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");

    return timestamp.str();
}
std::string Network::activityTypeToString(ActivityType type) {
    switch (type) {
        case ActivityType::MESSAGE_RECEIVED:
            return "Message Received";
        case ActivityType::MESSAGE_FORWARDED:
            return "Message Forwarded";
        case ActivityType::MESSAGE_SENT:
            return "Message Sent";
        case ActivityType::MESSAGE_DROPPED:
            return "Message Dropped";
        default:
            return "UNKNOWN_ACTIVITY_TYPE";
    }
}