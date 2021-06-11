#include "Classes.hpp"

using namespace dictionary;
using enum int_index;
using enum float_index;
using enum string_index;




dict_val::dict_val() {
    this->val = make_tuple(0, 0.0);
}

dict_val::dict_val(unsigned int arrival_time, float weight) {
    this->val = make_tuple(arrival_time, weight);
}

unsigned int& dict_val::operator[](int_index i) {
    switch (i) {
    case TOA:
        return get<0>(val);
        break;
    default:
        cout << "dict_val has no field " << i << endl;
        assert(false);
        static unsigned int defval = numeric_limits<unsigned int>::max();
        return defval;
        break;
    }
}

float& dict_val::operator[](float_index i) {
    switch (i) {
    case WEIGHT:
        return get<1>(val);
        break;
    default:
        cout << "dict_val has no field " << i << endl;
        assert(false);
        static float defval = numeric_limits<float>::max();
        return defval;
        break;
    }
}

dict_val2::dict_val2() {
    this->val = make_tuple("", 0, 0.0);
}

dict_val2::dict_val2(string packet_line, unsigned int size, float last) {
    this->val = make_tuple(packet_line, size, last);
}

unsigned int& dict_val2::operator[](int_index i) {
    switch (i) {
    case SIZE:
        return get<1>(val);
        break;
    default:
        cout << "dict_val2 has no field " << i << endl;
        assert(false);
        static unsigned int defval = numeric_limits<unsigned int>::max();
        return defval;
        break;
    }
}

float& dict_val2::operator[](float_index i) {
    switch (i) {
    case LAST:
        return get<2>(this->val);
        break;
    default:
        cout << "dict_val2 has no field " << i << endl;
        assert(false);
        static float defval = numeric_limits<float>::max();
        return defval;
        break;
    }
}

string& dict_val2::operator[](string_index i) {
    switch (i) {
    case PACKET_LINE:
        return get<0>(val);
        break;
    default:
        cout << "dict_val2 has no field " << i << endl;
        assert(false);
        static string defval = "";
        return defval;
        break;
    }
}

connection_dict::connection_dict() {
    this->conn_dict = {};
}

connection_dict::connection_dict(unordered_map<string, dict_val> conn_dict) {
    this->conn_dict = conn_dict;
}

dict_val& connection_dict::operator[](const string& key) {
    static dict_val defval = dict_val(0, 0.0);
    if (!this->conn_dict.count(key)) return defval;

    return this->conn_dict[key];
}

bool connection_dict::contains(string key) {
    return (bool)this->conn_dict.count(key);
}

void connection_dict::insert(string key, dict_val val, bool new_weight) {
    int arrival_time = (this->contains(key)) ? this->conn_dict[key][TOA] : val[TOA];
    float weight = (!this->contains(key) || new_weight) ? val[WEIGHT] : this->conn_dict[key][WEIGHT];
    this->conn_dict.insert({ key, dict_val(arrival_time, weight) });
    this->conn_dict[key][WEIGHT] = weight;
}

unordered_map<string, dict_val>::iterator connection_dict::begin()
{
    return this->conn_dict.begin();
}

unordered_map<string, dict_val>::iterator connection_dict::end()
{
    return this->conn_dict.end();
}

queue_dict::queue_dict() {
    this->conn_queue = {};
}

queue<dict_val2>& queue_dict::operator[](const string& key) {
    static queue<dict_val2> defval = queue<dict_val2>();
    if (!this->conn_queue.count(key)) { return defval; }

    return this->conn_queue[key];
}

bool queue_dict::contains(string key) {
    return (bool)this->conn_queue.count(key);
}

void queue_dict::queue_push(string key, dict_val2 val) {
    if (!this->contains(key)) {
        this->conn_queue.insert({ key, queue<dict_val2>() });
    }
    this->conn_queue[key].push(val);
}

dict_val2 queue_dict::queue_pop(string key) {
    static dict_val2 defval("", 0, 0.0);
    if (!this->contains(key)) { return defval; }
    dict_val2 retval = this->conn_queue[key].front();
    this->conn_queue[key].pop();
    if (this->conn_queue[key].empty()) {
        this->conn_queue.erase(key);
    }
    return retval;
}

bool is_active_weight(queue<dict_val2> dup, float round)
{
    queue<dict_val2> q = dup;
    while (!q.empty())
    {
        if (q.front()[LAST] > round)
            return true;
        q.pop();
    }
    return false;
}

/*
    Input: A connection links dictionary
    Output: Sum of all active links weights
*/
float queue_dict::sum_weighted_active_conns(connection_dict cd, float round) {
    float retval = 0;
    unordered_map<string, queue<dict_val2>>::iterator it;
    for (it = this->conn_queue.begin(); it != this->conn_queue.end(); it++) {        
        if (is_active_weight(it->second, round))
            retval += cd[it->first][WEIGHT];
    }
    return retval;
}


unordered_map<string, queue<dict_val2>>::iterator queue_dict::begin() {
    return this->conn_queue.begin();
}
unordered_map<string, queue<dict_val2>>::iterator queue_dict::end() {
    return this->conn_queue.end();
}

bool connection_dict::empty() {
    return this->conn_dict.empty();
}

bool queue_dict::empty() {
    return this->conn_queue.empty();
}

ostream& operator<<(ostream& stream, dict_val& dv) {
    stream << "{" << dv[TOA] << ", " << setprecision(3) << dv[WEIGHT] << "}";
    return stream;
}

ostream& operator<<(ostream& stream, dict_val2& dv) {
    stream << "{" << dv[PACKET_LINE] << ", " << dv[SIZE] << ", " << setprecision(10) << dv[LAST] << "}";
    return stream;
}

ostream& operator<<(ostream& stream, queue<dict_val2> q) {
    queue<dict_val2> dup = q;
    stream << "(";
    while (!dup.empty()) {
        stream << dup.front();
        dup.pop();
        if (dup.empty()) { break; }
        stream << ", ";
    }
    stream << ")";
    return stream;

}

ostream& operator<<(ostream& stream, connection_dict& cd) {
    unordered_map<string, dict_val>::iterator it;
    stream << "{" << endl;
    for (it = cd.begin(); it != cd.end(); it++) {
        stream << "\t{" << it->first << ", " << it->second << "}," << endl;
    }
    stream << "}";
    return stream;
}

//ostream& operator<<(ostream& stream, queue_dict qd) {
//    unordered_map<string, queue<dict_val2>>::iterator it;
//    stream << "{" << endl;
//    for (it = qd.begin(); it != qd.end(); it++) {
//        stream << "\t{" << it->first << ", " << it->second << "}," << endl;
//    }
//    stream << "}";
//    return stream;
//}

ostream& operator<<(ostream& stream, dictionary::int_index i) {
    switch (i) {
    case TOA:
        stream << "TOA";
        break;
    case SIZE:
        stream << "SIZE";
        break;
    }
    return stream;
}
ostream& operator<<(ostream& stream, dictionary::float_index i) {
    switch (i) {
    case WEIGHT:
        stream << "WEIGHT";
        break;
    case LAST:
        stream << "LAST";
        break;
    }
    return stream;
}
ostream& operator<<(ostream& stream, dictionary::string_index i) {
    switch (i) {
    case PACKET_LINE:
        stream << "WEIGHT";
        break;
    }
    return stream;
}




