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


void calculate_all_lasts();
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
	next_arrival_time = parse_arrival_time(nextline);
	update_virtual_time();
	calculate_all_lasts();
	//current_time = next_arrival_time;

	/*    Handle next Packets    */
	while (1) {
		update_virtual_time();
		calculate_all_lasts();
		//cout << "1: next_finish_time: " << current_finish_time << " next_arrival_time: " << next_arrival_time << " current_arrival_time: "
		//	<< current_arrival_time << " current time: " << current_time << endl;
		handle_packet(current_line);
		while (!packet_queue.empty() && current_time < next_arrival_time) {
			if (current_finish_time <= current_time) {				
				//cout << "sending packet" << endl;
				current_finish_time = pop_next_packet();
			}
			current_time = min(next_arrival_time, current_finish_time);
			//cout << "," << endl;
		}
		if (packet_queue.empty() || current_finish_time >= next_arrival_time) {
			current_time = next_arrival_time;
			current_line = nextline;
			if (!getline(cin, nextline)) { break; }
			next_arrival_time = parse_arrival_time(nextline);
			
		}
		else {
			current_time = current_finish_time;
		}
		//cout << packet_queue << endl;
		//current_time = next_arrival_time;
		/*cout << "2: next_finish_time: " << current_finish_time << " next_arrival_time: " << next_arrival_time << " current_arrival_time: "
						 << current_arrival_time << " current time: " << current_time << endl;*/

	}
	update_virtual_time();
	calculate_all_lasts();
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
	float sum_of_active_weights = packet_queue.sum_weighted_active_conns(conn_dict);
	if (sum_of_active_weights == 0.0F) {
		get<0>(roundt) = current_time;
		get<1>(roundt) = current_time;
		return;
	}
	float x = current_time - get<0>(roundt);
	get<0>(roundt) = current_time;
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
	tuple<int, string, int, float, bool> packet_param = break_line(packet);
	conn_dict.insert(get<1>(packet_param), dict_val(get<0>(packet_param), get<3>(packet_param)), get<4>(packet_param));
	packet_queue.queue_push(get<1>(packet_param), dict_val2(packet, get<2>(packet_param), 0));
	return get<0>(packet_param);
}

unsigned int parse_arrival_time(string packet_line) {
	stringstream s(packet_line);
	string retval;
	s >> retval;
	return stoi(retval);
}

void calculate_all_lasts() {
	unordered_map<string, queue<dict_val2>>::iterator it;
	string connection = get<0>(min_last);
	float current_min = get<1>(min_last);
	string curr_conn;
	for (it = packet_queue.begin(); it != packet_queue.end(); it++) {
		curr_conn = it->first; dict_val2& curr_val = it->second.front();
		//get<2>(curr_val.val) = calc_last(current_time, curr_val[SIZE], conn_dict[curr_conn][WEIGHT]);
		curr_val[LAST] = calc_last(current_time, curr_val[SIZE], conn_dict[curr_conn][WEIGHT]);
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
	update_virtual_time();
	calculate_all_lasts();
	string connection = get<0>(min_last);
	//cout << "2: connection: " << connection << endl;
	dict_val2& curr_val = packet_queue[connection].front();
	int packet_size = curr_val[SIZE];
	int finish_time = current_time + packet_size;
	float ftpp = curr_val[LAST];
	//cout << current_time << ": " << curr_val[PACKET_LINE] << endl;
	output << current_time << ": " << curr_val[PACKET_LINE] << endl;
	//output << get<0>(min_last) << ", " << setprecision(6) << get<1>(min_last) << endl;
	//output << packet_queue << endl;
	packet_queue.queue_pop(connection);
	get<1>(min_last) = numeric_limits<float>::max();
	if (!packet_queue.contains(connection)) { return finish_time; }
	int next_packet_size = packet_queue[connection].front()[SIZE];
	packet_queue[connection].front()[LAST] = calc_last(current_time, next_packet_size, conn_dict[connection][WEIGHT], ftpp);
	return finish_time;

}



