#include "Classes.hpp"
#include <sstream>
#include <list>
#include <fstream>
using namespace std;
using namespace dictionary;
using enum int_index;
using enum float_index;
using enum string_index;
#define WHITESPACE ' '
#define DEFAULT_WEIGHT 1.0
#define OUTPUT_PATH "./output.txt"

connection_dict conn_dict = connection_dict();
queue_dict packet_queue = queue_dict();
tuple<float, float> roundt(0.0, 0.0);
tuple<string, float> min_last("", numeric_limits<float>::max());
unsigned int current_time = 0;
ofstream output;


ostream& operator<<(ostream& stream, queue_dict qd) {
	unordered_map<string, queue<dict_val2>>::iterator it;
	stream << "{" << endl;
	for (it = qd.begin(); it != qd.end(); it++) {
		stream << "\t{" << it->first << ", " << it->second << "}" << conn_dict[it->first] << "," << endl;
	}
	stream << "}";
	return stream;
}


//void calculate_all_lasts();
void update_min_last();
unsigned int pop_next_packet();
unsigned int parse_arrival_time(string packet_line);
unsigned int handle_packet(string packet);
float calc_last(float t, int packet_size, float weight, float ftpp = 0.0F);
void update_virtual_time();
tuple<int, string, int, float, bool> break_line(string connection_line);

int main()
{	
	unsigned int next_arrival_time = 0, current_finish_time = current_time, current_arrival_time = 0;
	string nextline, current_line = "";
	output.open(OUTPUT_PATH);
	/*     Handle first packet    */   
	if (!getline(cin, current_line)) { return 0; }
	if (!getline(cin, nextline)) {
		current_time = handle_packet(current_line);
		pop_next_packet();
		return 0;
	}
	current_time = handle_packet(current_line);
	next_arrival_time = parse_arrival_time(nextline);
	current_line = nextline;
		
	//current_time = next_arrival_time;

	/*    Handle next Packets    */
	while (1) {		
		//calculate_all_lasts();
		//cout << "1: next_finish_time: " << current_finish_time << " next_arrival_time: " << next_arrival_time << " current_arrival_time: "
		//	<< current_arrival_time << " current time: " << current_time << endl;
		
		if (!packet_queue.empty() &&  current_time >= current_finish_time) {
			current_finish_time = pop_next_packet();			
		}
		if (packet_queue.empty() || current_finish_time >= next_arrival_time) {
			current_time = next_arrival_time;
			current_line = nextline;
			if (!getline(cin, nextline)) { break; }
			next_arrival_time = parse_arrival_time(nextline);		
			handle_packet(current_line);
		}
		else {
			current_time = current_finish_time;			
		}
		//cout << packet_queue << endl;
		//current_time = next_arrival_time;
		/*cout << "2: next_finish_time: " << current_finish_time << " next_arrival_time: " << next_arrival_time << " current_arrival_time: "
						 << current_arrival_time << " current time: " << current_time << endl;*/

	}	
	//cout << "1: next_finish_time: " << current_finish_time << " next_arrival_time: " << next_arrival_time << " current_arrival_time: "
	//	<< current_arrival_time << " current time: " << current_time << endl;
	handle_packet(current_line);
	while (!packet_queue.empty()) {
		current_time = max(current_finish_time, next_arrival_time);
		//cout << "sending packet" << endl;
		current_finish_time = pop_next_packet();
	}
	return 0;
}


// Input a packet line
// Output a Connection quadruple of ToA, (Sadd Sport Dadd Dport) as string, size and weight
tuple<int, string, int, float, bool> break_line(string connection_line) {
	stringstream s(connection_line);
	string current_word, output;
	s >> current_word;
	int arriving_time = stoi(current_word);
	s >> output;
	for (int i = 0; i < 3; i++) {
		s >> current_word;
		output += " " + current_word;
	}
	s >> current_word;
	int size = stoi(current_word);
	if (!s.rdbuf()->in_avail())
		return make_tuple(arriving_time, output, size, DEFAULT_WEIGHT, false);
	s >> current_word;
	float weight = stof(current_word);
	return make_tuple(arriving_time, output, size, weight, true);
}

/*
	Input: t - time, time to next event - x, sum of all activelinks weights and the round dictionary
	Output: return the round of which the next event will happen
*/

void update_virtual_time() {
	float sum_of_active_weights = packet_queue.sum_weighted_active_conns(conn_dict, get<1>(roundt));
	if (sum_of_active_weights == 0.0F) {
		get<0>(roundt) = (float)current_time;
		get<1>(roundt) = (float)current_time;
		return;
	}
	float x = current_time - get<0>(roundt);
	get<0>(roundt) = (float)current_time;
	get<1>(roundt) += x / sum_of_active_weights;
}
/*
	Input: t - time, finish time of previous packet ftpp, packet_size, packet weight and the round dictionary
	Output: return the last(packet) - its GPS finish time
*/
float calc_last(float t, int packet_size, float weight, float ftpp) {
	return max(get<1>(roundt), ftpp) + packet_size / weight;
}
/*
	Input: Packet string - packet
	Output: returns arrival time
	Function Handles inserts to the queue new packets
*/
unsigned int handle_packet(string packet) {
	update_virtual_time();
	tuple<int, string, int, float, bool> packet_param = break_line(packet);
	string connection = get<1>(packet_param);
	int packet_size = get<2>(packet_param);
	conn_dict.insert(connection, dict_val(get<0>(packet_param), get<3>(packet_param)), get<4>(packet_param));
	float ftpp = packet_queue.contains(connection) ? packet_queue[connection].back()[LAST] : 0;
	float last = calc_last((float)current_time, packet_size, conn_dict[connection][WEIGHT], ftpp);
	packet_queue.queue_push(get<1>(packet_param), dict_val2(packet, packet_size, last));	
	return get<0>(packet_param);
}

unsigned int parse_arrival_time(string packet_line) {
	stringstream s(packet_line);
	string retval;
	s >> retval;
	return stoi(retval);
}

void update_min_last() {
	unordered_map<string, queue<dict_val2>>::iterator it;
	string connection = get<0>(min_last);
	float current_min = get<1>(min_last);
	string curr_conn;	
	for (it = packet_queue.begin(); it != packet_queue.end(); it++) {
		curr_conn = it->first; dict_val2& curr_val = it->second.front();				
		if (current_min == curr_val[LAST]) {
			if (conn_dict[curr_conn][TOA] > conn_dict[connection][TOA]) { continue; }
			connection = curr_conn;
			current_min = curr_val[LAST];
		}
		else if (current_min < curr_val[LAST]) { continue; }
		connection = curr_conn;
		current_min = curr_val[LAST];
	}
	get<0>(min_last) = connection;
	get<1>(min_last) = current_min;
}

/*
	Input: Current time - ct
	Output: Finish time
	pops the next packet out of the queue
*/
unsigned int pop_next_packet() {	
	string& connection = get<0>(min_last);
	packet_queue.queue_pop(connection);
	get<1>(min_last) = numeric_limits<float>::max();
	update_min_last();
	if (packet_queue.empty()) {
		connection = "";
		return current_time;
	}
	dict_val2& curr_val = packet_queue[connection].front();
	int packet_size = curr_val[SIZE];
	int finish_time = current_time + packet_size;
	output << current_time << ": " << curr_val[PACKET_LINE] << endl;
	//output << get<0>(min_last) << ", " << setprecision(6) << get<1>(min_last) << endl;
	//output << packet_queue << endl;
	return finish_time;
}



