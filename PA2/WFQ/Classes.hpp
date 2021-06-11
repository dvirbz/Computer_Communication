#ifndef CLASSES_HPP
#define CLASSES_HPP

#pragma once
#include <string>
#include <iostream>
#include <tuple>
#include <queue>
#include <assert.h>
#include <algorithm>
#include <unordered_map>
#include <stdexcept>
#include <functional>
#include <iterator>
#include <limits>
#include <iomanip>


using namespace std;
#define WHITESPACE ' '

namespace dictionary
{
    enum class int_index {
        TOA,
        SIZE,
    };
    enum class float_index {
        WEIGHT,
        LAST,
    };
    enum class string_index {
        PACKET_LINE
    };

    class dict_val {
    public:
        tuple<unsigned int, float> val;
        dict_val();
        dict_val(unsigned int arrival_time, float weight);
        unsigned int& operator[](dictionary::int_index i);
        float& operator[](dictionary::float_index i);
    };

    class dict_val2 {
    public:
        tuple<string, unsigned int, float> val;
        dict_val2();
        dict_val2(string packet_line, unsigned int size, float last);
        unsigned int& operator[](dictionary::int_index i);
        float& operator[](dictionary::float_index i);
        string& operator[](dictionary::string_index i);
    };

    class connection_dict {
    public:
        unordered_map<string, dict_val> conn_dict;
        connection_dict();
        connection_dict(unordered_map<string, dict_val> conn_dict);
        dict_val& operator[](const string& key);
        void insert(string key, dict_val val, bool new_weight = false);
        unordered_map<string, dict_val>::iterator begin();
        unordered_map<string, dict_val>::iterator end();
        bool contains(string key);
        bool empty();
    };
    class queue_dict {
    public:
        unordered_map<string, queue<dict_val2>> conn_queue;
        queue_dict();
        queue<dict_val2>& operator[](const string& key);
        bool contains(string key);
        void queue_push(string key, dict_val2 size);
        dict_val2 queue_pop(string key);
        float sum_weighted_active_conns(connection_dict cd, float round);
        unordered_map<string, queue<dict_val2>>::iterator begin();
        unordered_map<string, queue<dict_val2>>::iterator end();
        bool empty();
    };
}

ostream& operator<<(ostream& stream, dictionary::dict_val& dv);
ostream& operator<<(ostream& stream, dictionary::dict_val2& dv);
ostream& operator<<(ostream& stream, queue<dictionary::dict_val2> q);
ostream& operator<<(ostream& stream, dictionary::connection_dict& cd);
//ostream& operator<<(ostream& stream, dictionary::queue_dict qd);
ostream& operator<<(ostream& stream, dictionary::int_index qd);
ostream& operator<<(ostream& stream, dictionary::float_index qd);
ostream& operator<<(ostream& stream, dictionary::string_index qd);


#endif