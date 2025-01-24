#include "Network.h"

Network::Network() {

}

void Network::process_commands(vector<Client> &clients, vector<string> &commands, int message_limit,
                               const string &sender_port, const string &receiver_port) {
    // TODO: Execute the commands given as a vector of strings while utilizing the remaining arguments.
    /* Don't use any static variables, assume this method will be called over and over during testing.
     Don't forget to update the necessary member variables after processing each command. For example,
     after the MESSAGE command, the outgoing queue of the sender must have the expected frames ready to send. */
    for(const string& commandString : commands) {

        print_dash_line(commandString.length()+9);
        cout << "Command: " << commandString << endl;
        print_dash_line(commandString.length()+9);

        istringstream iss(commandString);
        string command;
        iss >> command;
        if (command == "MESSAGE") {
            string sender_ID, receiver_ID, message;
            iss >> sender_ID >> receiver_ID;
            getline(iss >> std::ws, message);
            message = message.substr(1, message.length()-2);
            cout << "Message to be sent: \"" << message << "\"\n" << endl;
            Client *sender = nullptr, *receiver = nullptr;
            for (Client &client : clients) {
                if (client.client_id == sender_ID) {
                    sender = &client;
                }
                if (client.client_id == receiver_ID) {
                    receiver = &client;
                }
            }
            if (sender != nullptr && receiver != nullptr) {
                int numOfFrames = message.length()/message_limit;
                if (message.length() % message_limit != 0) {
                    ++numOfFrames;
                }
                for (int frame = 1; frame<=numOfFrames; ++frame) {
                    int start = (frame-1) * message_limit;
                    int chunkSize = min(message_limit, static_cast<int>(message.length()) - start);
                    string messageChunk = message.substr(start, chunkSize);

                    Client *nextHop = nullptr;
                    for (Client &client : clients) {
                        if (client.client_id == sender->routing_table[receiver->client_id]) {
                            nextHop = &client;
                        }
                    }
                    auto* physical = new PhysicalLayerPacket(3, sender->client_mac, nextHop->client_mac);
                    auto* network = new NetworkLayerPacket(2, sender->client_ip, receiver->client_ip);
                    auto* transport = new TransportLayerPacket(1, sender_port, receiver_port);
                    auto* application = new ApplicationLayerPacket(0, sender->client_id, receiver->client_id, messageChunk);

                    physical->numOfFrame = frame;

                    cout << "Frame #" << physical->numOfFrame << endl;
                    physical->print();
                    network->print();
                    transport->print();
                    application->print();
                    cout << "Message chunk carried: \"" << messageChunk << "\"" << endl;
                    cout << "Number of hops so far: " << application->numOfHops << endl;
                    print_dash_line(8);
                    stack<Packet*> packets;
                    packets.push(application);
                    packets.push(transport);
                    packets.push(network);
                    packets.push(physical);
                    sender->outgoing_queue.push(packets);
                    if (frame == numOfFrames) {
                        Log log_entry(get_timestamp(), message, numOfFrames, application->numOfHops, sender->client_id, receiver->client_id, false, ActivityType::MESSAGE_SENT);
                        sender->log_entries.push_back(log_entry);
                    }
                }
            }
        } else if (command == "SHOW_FRAME_INFO") {
            string client_ID, in_or_out;
            int frame;
            iss >> client_ID >> in_or_out >> frame;

            Client *selected_client = nullptr;
            for (Client &client : clients) {
                if (client.client_id == client_ID) {
                    selected_client = &client;
                    break;
                }
            }
            if (selected_client != nullptr) {
                queue<stack<Packet*>> *selected_queue;
                if (in_or_out == "in") {
                    selected_queue = &(selected_client->incoming_queue);
                } else if (in_or_out == "out") {
                    selected_queue = &(selected_client->outgoing_queue);
                }
                int current_frame = 1;

                queue<stack<Packet*>> tempQueue = *selected_queue;

                while (!tempQueue.empty() && current_frame != frame) {
                    tempQueue.pop();
                    ++current_frame;
                }
                if (current_frame == frame && !tempQueue.empty()) {
                    cout << "Current Frame #" << frame << " on the " << (in_or_out=="out" ? "outgoing" : "incoming") << " queue of client " << client_ID << endl;
                    stack<Packet*> packets = tempQueue.front();
                    tempQueue.pop();

                    stack<Packet*> temp;
                    while (!packets.empty()) {
                        temp.push(packets.top());
                        packets.pop();
                    }
                    int hops = 0;
                    while (!temp.empty()) {
                        Packet* packet = temp.top();
                        if (ApplicationLayerPacket* a_l_p = dynamic_cast<ApplicationLayerPacket*>(packet)) {
                            cout << "Carried Message: \"" << a_l_p->message_data << "\"" << endl;
                            hops = a_l_p->numOfHops;
                        }
                        cout << "Layer " << packet->layer_ID << " info: ";
                        packet->print();
                        temp.pop();
                        if (packet->layer_ID == 3) {
                            cout << "Number of hops so far: " << hops << endl;
                        }
                    }
                } else {
                    cout << "No such frame." << endl;
                }
            }
            continue;
        } else if (command == "SHOW_Q_INFO") {
            string client_ID, in_or_out;
            iss >> client_ID >> in_or_out;
            cout << "Client " << client_ID << (in_or_out=="out" ? " Outgoing" : " Incoming") << " Queue Status" << endl;
            Client *selected_client = nullptr;
            for (Client &client : clients) {
                if (client.client_id == client_ID) {
                    selected_client = &client;
                    break;
                }
            }
            if (selected_client != nullptr) {
                queue<stack<Packet *>> *selected_queue;
                if (in_or_out == "in") {
                    selected_queue = &(selected_client->incoming_queue);
                } else if (in_or_out == "out") {
                    selected_queue = &(selected_client->outgoing_queue);
                }
                cout << "Current total number of frames: " << selected_queue->size() << endl;
            }
        } else if (command == "SEND") {
            int current_frame = 0;
            for (Client &sender : clients) {
                while (!sender.outgoing_queue.empty()) {
                    stack<Packet*> outgoing_packets = sender.outgoing_queue.front();
                    Client* nextHop = nullptr;
                    for (Client &client : clients) {
                        if (client.client_mac == dynamic_cast<PhysicalLayerPacket*>(outgoing_packets.top())->receiver_MAC_address) {
                            nextHop = &client;
                            break;
                        }
                    }
                    if (nextHop!= nullptr) {
                        nextHop->incoming_queue.push(outgoing_packets);
                        for (Log &log : sender.log_entries) {
                            if (sender.client_id == log.sender_id) {
                                log.activity_type = ActivityType::MESSAGE_SENT;
                                log.success_status = true;
                            }
                        }
                        sender.outgoing_queue.pop();

                        current_frame++;
                        while (!outgoing_packets.empty()) {
                            Packet* packet = outgoing_packets.top();
                            if (packet->layer_ID == 3) {
                                cout << "Client " << sender.client_id << " sending frame #" << current_frame << " to client " << nextHop->client_id << endl;
                            }
                            packet->print();
                            if (packet->layer_ID == 0) {
                                auto* appPacket = dynamic_cast<ApplicationLayerPacket*>(packet);
                                appPacket->numOfHops++;
                                cout << "Message chunk carried: \"" << appPacket->message_data << "\""<< endl;
                                cout << "Number of hops so far: " << appPacket->numOfHops << endl;
                                print_dash_line(8);
                                if (appPacket->message_data.find('.')!= std::string::npos ||
                                    appPacket->message_data.find('!')!= std::string::npos ||
                                    appPacket->message_data.find('?')!= std::string::npos ) {
                                    current_frame = 0;
                                }
                            }
                            outgoing_packets.pop();
                        }
                    }
                }
            }
        } else if (command == "RECEIVE") {
            int current_frame = 0;
            string assembled_message = "";
            for (Client &receiver : clients) {
                while(!receiver.incoming_queue.empty()) {
                    stack<Packet*> incoming_packets = receiver.incoming_queue.front();
                    receiver.outgoing_queue.push(incoming_packets);
                    string message_for_log = "";
                    receiver.incoming_queue.pop();


                    bool destination_reached = false;
                    Client* nextHop = nullptr;
                    Client* sender = nullptr;
                    string original_sender;
                    string destination;
                    int num_of_hops = 0;

                    for (Client &client : clients) {
                        if (client.client_mac == dynamic_cast<PhysicalLayerPacket*>(incoming_packets.top())->sender_MAC_address) {
                            sender = &client;
                            break;
                        }
                    }

                    stack<Packet*> temp_stack = incoming_packets;
                    while (!temp_stack.empty()) {
                        Packet* packet = temp_stack.top();
                        if (packet->layer_ID == 0) {
                            auto* a_l_p = dynamic_cast<ApplicationLayerPacket*>(packet);
                            destination = a_l_p->receiver_ID;
                            original_sender =  a_l_p->sender_ID;
                            num_of_hops = a_l_p->numOfHops;
                            message_for_log += a_l_p->message_data;

                            for (Client &client : clients) {
                                if (client.client_id == receiver.routing_table[destination]) {
                                    nextHop = &client;
                                    break;
                                }
                            }
                            if (receiver.client_id == destination) {
                                assembled_message += a_l_p->message_data;
                                destination_reached = true;
                            }
                        }
                        temp_stack.pop();
                    }
                    current_frame++;

                    if (destination_reached) {
                        cout << "Client " << receiver.client_id << " receiving frame #" << current_frame <<
                             " from client " << sender->client_id << ", originating from client " << original_sender << endl;

                    } else {
                        if (nextHop!= nullptr){
                            if(current_frame==1) {
                                cout << "Client " << receiver.client_id << " receiving a message from client " <<
                                     sender->client_id << ", but intended for client " << destination << ". Forwarding... \n";
                            }
                        }
                    }

                    while (!incoming_packets.empty()) {
                        Packet *packet = incoming_packets.top();

                        if (destination_reached) {
                            packet->print();
                            if (packet->layer_ID == 0) {
                                receiver.outgoing_queue.pop();
                                cout << "Message chunk carried: \""
                                     << dynamic_cast<ApplicationLayerPacket *>(packet)->message_data << "\"" << endl;
                                cout << "Number of hops so far: "
                                     << dynamic_cast<ApplicationLayerPacket *>(packet)->numOfHops << endl;
                                print_dash_line(8);
                                if (dynamic_cast<ApplicationLayerPacket *>(packet)->message_data.find('.') !=std::string::npos ||
                                    dynamic_cast<ApplicationLayerPacket *>(packet)->message_data.find('!') !=std::string::npos ||
                                    dynamic_cast<ApplicationLayerPacket *>(packet)->message_data.find('?') !=std::string::npos) {
                                    Log log(get_timestamp(), assembled_message, current_frame,
                                            num_of_hops, original_sender, destination, true,
                                            ActivityType::MESSAGE_RECEIVED);
                                    receiver.log_entries.push_back(log);
                                    current_frame = 0;
                                    cout << "Client " << receiver.client_id << " received the message \""
                                         << assembled_message << "\"" << " from client " << original_sender << "."
                                         << endl;
                                    print_dash_line(8);
                                    assembled_message = "";
                                }
                            }
                        } else if (nextHop == nullptr) {
                            if (packet->layer_ID == 0) {
                                receiver.outgoing_queue.pop();
                                cout << "Client " << receiver.client_id << " receiving frame #" << current_frame << " from client "<<
                                     sender->client_id << ", but intended for client " << destination << ". Forwarding... " << endl;
                                cout << "Error: Unreachable destination. Packets are dropped after " << num_of_hops <<" hops!" << endl;
                                if (dynamic_cast<ApplicationLayerPacket*>(packet)->message_data.find('.') != std::string::npos ||
                                    dynamic_cast<ApplicationLayerPacket*>(packet)->message_data.find('!') != std::string::npos ||
                                    dynamic_cast<ApplicationLayerPacket*>(packet)->message_data.find('?') != std::string::npos) {
                                    Log log(get_timestamp(), assembled_message, current_frame,
                                            num_of_hops, original_sender, destination, false,
                                            ActivityType::MESSAGE_DROPPED);
                                    receiver.log_entries.push_back(log);
                                    current_frame = 0;
                                    print_dash_line(8);
                                }
                            }
                        } else {
                            if (packet->layer_ID == 0) {
                                if (dynamic_cast<ApplicationLayerPacket*>(packet)->message_data.find('.') != std::string::npos ||
                                    dynamic_cast<ApplicationLayerPacket*>(packet)->message_data.find('!') != std::string::npos ||
                                    dynamic_cast<ApplicationLayerPacket*>(packet)->message_data.find('?') != std::string::npos) {
                                    Log log(get_timestamp(), assembled_message, current_frame,
                                            num_of_hops, original_sender, destination, true,
                                            ActivityType::MESSAGE_FORWARDED);
                                    receiver.log_entries.push_back(log);
                                    current_frame = 0;
                                    print_dash_line(8);
                                }
                            } else if (packet->layer_ID == 3) {
                                dynamic_cast<PhysicalLayerPacket*>(packet)->sender_MAC_address = receiver.client_mac;
                                dynamic_cast<PhysicalLayerPacket*>(packet)->receiver_MAC_address = nextHop->client_mac;
                                cout << "Frame #" << current_frame << " MAC address change: New sender MAC " << receiver.client_mac << ", new receiver MAC " << nextHop->client_mac << endl;
                            }
                        }
                        incoming_packets.pop();
                    }
                }
            }
        } else if (command == "PRINT_LOG") {
            string client_id;
            iss >> client_id;
            for (Client &client : clients) {
                if(client.client_id == client_id) {
                    if (client.log_entries.size()!=0){
                        cout << "Client " << client_id << " Logs:" << endl;
                        print_dash_line(14);}
                    for (int i = 0; i < client.log_entries.size(); ++i) {
                        cout << "Log Entry #" << i + 1 << ":\nActivity: ";
                        switch (client.log_entries[i].activity_type) {
                            case ActivityType::MESSAGE_RECEIVED:
                                cout << "Message Received" << endl;
                                break;
                            case ActivityType::MESSAGE_FORWARDED:
                                cout << "Message Forwarded" << endl;
                                break;
                            case ActivityType::MESSAGE_SENT:
                                cout << "Message Sent" << endl;
                                break;
                            case ActivityType::MESSAGE_DROPPED:
                                cout << "Message Dropped" << endl;
                                break;
                            default:
                                cout << "Unknown Activity" << endl;
                        }
                        cout << "Timestamp: " << client.log_entries[i].timestamp << endl;
                        cout << "Number of frames: " << client.log_entries[i].number_of_frames << endl;
                        cout << "Number of hops: " << client.log_entries[i].number_of_hops << endl;
                        cout << "Sender ID: " << client.log_entries[i].sender_id << endl;
                        cout << "Receiver ID: " << client.log_entries[i].receiver_id << endl;
                        cout << "Success: " << (client.log_entries[i].success_status ? "Yes" : "No") << endl;
                        if (client_id == client.log_entries[i].sender_id || client_id == client.log_entries[i].receiver_id){
                            cout << "Message: \"" << client.log_entries[i].message_content << "\"" << endl;
                        }
                        if (i != client.log_entries.size()-1) {
                            print_dash_line(14);
                        }
                    }
                }
            }
        } else {
            cout << "Invalid command." << endl;
        }
    }
}

void Network::printQueue(queue<stack<Packet*>>& q) {
    while (!q.empty()) {
        stack<Packet*> s = q.front();
        q.pop();

        cout << "Queue Front: ";
        while (!s.empty()) {
            Packet* packet = s.top();
            packet->print(); // Print packet information
            s.pop();
        }
        cout << endl;
    }
}

void Network::printStack(stack<Packet*>& s) {
    cout << "Stack Contents: ";
    stack<Packet*> temp; // Temporary stack to preserve the original stack order
    // Push elements onto a temporary stack to print and preserve the original order
    while (!s.empty()) {
        temp.push(s.top());
        s.pop();
    }
    while (!temp.empty()) {
        Packet* packet = temp.top();
        packet->print(); // Print packet information
        temp.pop();
    }
    cout << endl;
}


void Network::print_dash_line(int length) {
    string dashLine;
    for (int i = 0; i < length; ++i) {
        dashLine += "-";
    }
    cout << dashLine << endl;
}

string Network::get_timestamp() {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm time_info = *localtime(&now_c);

    stringstream ss;
    ss << put_time(&time_info, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}


vector<Client> Network::read_clients(const string &filename) {
    vector<Client> clients;
    // TODO: Read clients from the given input file and return a vector of Client instances.
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Unable to open file!" << endl;
        return clients;
    }
    int numOfClients;
    file >> numOfClients;
    file.ignore();
    for (int i = 0; i < numOfClients; ++i) {
        string id, ip, mac;
        file >> id >> ip >> mac;
        Client client(id, ip, mac);
        clients.push_back(client);
    }
    file.close();
    return clients;
}

void Network::read_routing_tables(vector<Client> &clients, const string &filename) {
    // TODO: Read the routing tables from the given input file and populate the clients' routing_table member variable.
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Unable to open file!" << endl;
        return;
    }
    string line;
    int clientIndex = 0;

    while (getline(file, line)) {
        if (line == "-") {
            ++clientIndex;
            continue;
        }
        istringstream iss(line);
        string destination, nearest_neighbor;
        iss >> destination >> nearest_neighbor;

        if (clientIndex < clients.size()) {
            clients[clientIndex].routing_table[destination] = nearest_neighbor;
        } else {
            break;
        }
    }
    file.close();
}

// Returns a list of token lists for each command
vector<string> Network::read_commands(const string &filename) {
    vector<string> commands;
    // TODO: Read commands from the given input file and return them as a vector of strings.
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Unable to open file!" << endl;
        return commands;
    }
    int numOfCommands;
    file >> numOfCommands;
    file.ignore();
    for (int i = 0; i < numOfCommands; ++i) {
        string command;
        getline(file, command);
        commands.push_back(command);
    }
    file.close();
    return commands;
}

Network::~Network() {
    // TODO: Free any dynamically allocated memory if necessary.
}
