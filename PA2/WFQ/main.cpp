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
queue_dict WFQ = queue_dict();
queue_dict GPS = queue_dict();
float prev_time = 0.0F;
float round_time = 0.0F;
string min_last_conn;
float min_last = numeric_limits<float>::max();
float current_time = 0;
float sum_of_active_weights = 0.0F;
ofstream output;


ostream& operator<<(ostream& stream, queue_dict qd) {
	unordered_map<string, queue<packet>>::iterator it;
	stream << "{" << endl;
	for (it = qd.begin(); it != qd.end(); it++) {
		stream << "\t{" << it->first << ", " << it->second << "}" << conn_dict[it->first] << "," << endl;
	}
	stream << "}";
	return stream;
}


//void calculate_all_lasts();
void update_min_last(queue_dict pq);
int receive(string pack);
void recalculate_round(float next_round);
float pop_next_packet();
float parse_arrival_time(string packet_line);
float handle_packet(string packet);
float calc_last(int packet_size, float weight, float ftpp = 0.0F);
void update_virtual_time();
tuple<float, string, int, float, bool> break_line(string connection_line);

int main()
{	
	float next_arrival_time, current_finish_time = 0, next_min_last_time;
	unsigned int s;
	string nextline;
	output.open(OUTPUT_PATH);
	/*     Handle first packet     */  
	if (!getline(cin, nextline)) { return 0; }
	next_arrival_time = parse_arrival_time(nextline);

	/*    Handle next Packets    */
	while (1) {
		update_min_last(GPS);
		if( prev_time != current_time) {
			sum_of_active_weights = GPS.sum_weighted_active_conns(conn_dict);
		}
		next_min_last_time = prev_time + (min_last - round_time) * sum_of_active_weights;
		s = (!GPS.empty() << 1) + !WFQ.empty();
		switch(s) {
		case 0:
			current_time = next_arrival_time;
			break;
		case 1:
			current_time = min(next_arrival_time, current_finish_time);
			break;
		case 2:
			current_time = min(next_arrival_time, next_min_last_time);
			break;
		case 3:
			current_time = min({ next_arrival_time, current_finish_time, next_min_last_time });
			break;
		}


		if (!GPS.empty() && current_time == next_min_last_time) {
			update_virtual_time();
			GPS.queue_pop(min_last_conn, conn_dict);
		}
		if (current_time == next_arrival_time) {
			handle_packet(nextline);
			if (!getline(cin, nextline)) { break; }
			next_arrival_time = parse_arrival_time(nextline);
			continue;
		}
		if (!WFQ.empty() && current_time == current_finish_time) {
			current_finish_time = pop_next_packet();
			if (WFQ.empty()) {
				current_finish_time = max(current_finish_time, next_arrival_time);
			}
		}
	}	
	while (!WFQ.empty()) {
		current_time = current_finish_time;
		current_finish_time = pop_next_packet();
	}
	return 0;
}


// Input a packet line
// Output a Connection quadruple of ToA, (Sadd Sport Dadd Dport) as string, size and weight
tuple<float, string, int, float, bool> break_line(string connection_line) {
	stringstream s(connection_line);
	string current_word, output;
	s >> current_word;
	float arriving_time = stof(current_word);
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
	if (sum_of_active_weights == 0.0F) {
		prev_time = (float)current_time;
		//round_time = 0.0F;
		return;
	}
	float dt = current_time - prev_time;
	//assert(dt >= 0);

	prev_time = (float)current_time;
	round_time += dt / sum_of_active_weights;
}


/*
	Input: t - time, finish time of previous packet ftpp, packet_size, packet weight and the round dictionary
	Output: return the last(packet) - its GPS finish time
*/
float calc_last(int packet_size, float weight, float ftpp) {
	return max(round_time, ftpp) + packet_size / weight;
}
/*
	Input: Packet string - packet
	Output: returns arrival time
	Function Handles inserts to the queue new packets
*/
float handle_packet(string p) {
	update_virtual_time();
	tuple<float, string, int, float, bool> packet_param = break_line(p);
	string connection = get<1>(packet_param); int packet_size = get<2>(packet_param); float weight = get<3>(packet_param); bool is_new_weight = get<4>(packet_param);
	float temp_weight = (conn_dict.contains(connection)) ? conn_dict[connection][WEIGHT] : weight;
	temp_weight = (GPS.contains(connection)) ? GPS[connection].back()[WEIGHT] : temp_weight;
	float packet_weight = get<4>(packet_param) ? weight : temp_weight;
	conn_dict.insert(connection, dict_val((int)get<0>(packet_param), weight), !GPS.contains(connection) && is_new_weight);
	float ftpp = GPS.contains(connection) ? GPS[connection].back()[LAST] : 0;
	float last = calc_last(packet_size, packet_weight, ftpp);
	WFQ.queue_push(get<1>(packet_param), packet(p, packet_size, last, packet_weight));
	GPS.queue_push(connection, packet(p, packet_size, last, packet_weight));
	return get<0>(packet_param);
}

float parse_arrival_time(string packet_line) {
	stringstream s(packet_line);
	float retval;
	s >> retval;
	return retval;
}

void update_min_last(queue_dict pq) {
	min_last = numeric_limits<float>::max();
	unordered_map<string, queue<packet>>::iterator it;
	string curr_conn;
	for (it = pq.begin(); it != pq.end(); it++) {
		curr_conn = it->first; packet& curr_val = it->second.front();				
		if (min_last == curr_val[LAST]) {
			if (conn_dict[curr_conn][TOA] > conn_dict[min_last_conn][TOA]) { continue; }
			min_last_conn = curr_conn;
			min_last = curr_val[LAST];
		}
		else if (min_last < curr_val[LAST]) { continue; }
		min_last_conn = curr_conn;
		min_last = curr_val[LAST];
	}
}

/*
	Input: Current time - ct
	Output: Finish time
	pops the next packet out of the queue
*/
float pop_next_packet() {	
	update_min_last(WFQ);
	packet curr_val = WFQ[min_last_conn].front();
	int packet_size = curr_val[SIZE];
	float finish_time = current_time + packet_size;
	WFQ.queue_pop(min_last_conn);
	output << ((int)current_time) << ": " << curr_val[PACKET_LINE] << endl;
	return finish_time;
}



