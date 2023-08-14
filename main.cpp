#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <chrono>
#include <fstream>

using namespace std;

const string VERSION = "0.0.0";
const char DELIMITER = '|';
const string FILE_PATH = "/home/user/.kb";

vector<string> explode(const string& str, const char& ch) {
    string next;
    vector<string> result;

    // For each character in the string
    for (string::const_iterator it = str.begin(); it != str.end(); it++) {
        // If we've hit the terminal character
        if (*it == ch) {
            // If we have some characters accumulated
            if (!next.empty()) {
                // Add them to the result vector
                result.push_back(next);
                next.clear();
            }
        } else {
            // Accumulate the next character into the sequence
            next += *it;
        }
    }
    if (!next.empty())
        result.push_back(next);
    return result;
}

enum ItemState {
    ICEBOX = 0,
    TODO = 1,
    DOING = 2,
    DONE = 3
};

struct Item {
    time_t last_updated;
    string description;
    ItemState state;
    string Serialize();
    string ToString();
    static Item Deserialize(string *data);
};

string Item::Serialize() {
    return this->ToString();
}

Item Item::Deserialize(string *data) {
    vector<string> tokens = explode(*data, DELIMITER);
    time_t updated = (time_t)stol(tokens[0]);
    ItemState state = (ItemState)stoi(tokens[1]);
    string desc = tokens[2];
    return Item{updated, desc, state};
}

string Item::ToString() {
    string s;
    s.assign(to_string(last_updated) + DELIMITER + to_string(this->state) + DELIMITER + description);
    return s;
}

void Save(string data)
{
    fstream fs(FILE_PATH, ios_base::in | ios_base::out | ios_base::app);
    fs << data << endl;
    fs.close();
}

vector<Item> Load()
{
    vector<Item> items;
    fstream fs;
    fs.open(FILE_PATH, ios_base::in | ios_base::out | ios_base::app);

    string text;
    while(getline(fs, text))
    {
        items.push_back(Item::Deserialize(&text));
    }
    fs.close();
    return items;


}

int main() {
    std::cout << "Kanban - " << VERSION << std::endl;
    vector<Item> boardItems;
    // Read from file
    boardItems = Load();
    // Display
    for(int i = 0; i < boardItems.size(); i++)
    {
        cout << "Deserialized Item: " << boardItems[i].ToString() << endl;
    }
    return 0;
}
