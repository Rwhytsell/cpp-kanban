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

void Save(vector<Item> data)
{
    fstream fs(FILE_PATH, ios_base::in | ios_base::out);
    for(Item item: data)
    {
        fs << item.ToString() << endl;
    }
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
    char input;
    int int_input;
    string input_text;
    vector<Item> boardItems;
    // Read from file
    boardItems = Load();
    // Display
    bool should_continue = true;
    while(should_continue) {
        vector<Item> icebox, todo, doing, done;
        for(auto item : boardItems)
        {
            switch (item.state) {
                case ItemState::ICEBOX:
                    icebox.push_back(item);
                    break;
                case ItemState::TODO:
                    todo.push_back(item);
                    break;
                case ItemState::DOING:
                    doing.push_back(item);
                    break;
                case ItemState::DONE:
                    doing.push_back(item);
                    break;
            }
        }

        system("clear");
        cout << "Kanban - " << VERSION << std::endl;
        cout << endl;
        cout << '\t' << "Icebox" << '\t' << '\t' << '\t' << '\t' << "Todo" << '\t' << '\t' <<  '\t' << '\t' << "Doing" << '\t' << '\t' << '\t' << '\t' << "Done" << endl;
        cout << '\t' << "______" << '\t' << '\t' << '\t' << '\t'<< "____" << '\t' << '\t' << '\t' << '\t'<< "_____" << '\t' << '\t' << '\t' << '\t' << "____" << endl;
        size_t board_max = max(icebox.size(), todo.size());
        board_max = max(board_max, doing.size());
        board_max = max(board_max, done.size());
        for(int i = 0; i < board_max; i ++)
        {
            string ice_text = icebox.size() > i ? icebox[i].description : " ";
            string todo_text = todo.size() > i ? todo[i].description : " ";
            string doing_text = doing.size() > i ? doing[i].description : " ";
            string done_text = done.size() > i ? done[i].description : " ";

            cout << "\t";
            cout << ice_text;
            cout << "\t"<< "\t" << '\t' << '\t';
            cout << todo_text;
            cout << "\t"<< "\t" << '\t' << '\t';
            cout << doing_text;
            cout << "\t"<< "\t" << '\t' << '\t';
            cout << done_text;
            cout << endl;
        }
        cout << endl;
        cout << "(a)dd item | (m)ove item | (d)elete item | (e)xit" << endl;
        cout << "> ";
        cin >> input;

        if (input == 'e')
        {
            should_continue = false;
        }

        if (input == 'a')
        {
            system("clear");
            cout << "Enter description of item: " << endl;
            input_text.clear();
            cin.ignore();
            getline(cin, input_text);
            Item new_item = Item{time(nullptr), input_text, ItemState::TODO};
            boardItems.push_back(new_item);
        }

        if (input == 'm')
        {
            system("clear");
            cout << "Which item do you want to move?:" << endl;
            for(int item_i = 0; item_i < boardItems.size(); item_i++)
            {
                cout << "[" << to_string(item_i) << "] " << boardItems[item_i].description << endl;
            }
            cin >> int_input;
            if(int_input < 0 || int_input >= boardItems.size())
            {
                break;
            }
            cout << "Where do you want to move it?" << endl << "[I]cebox | [T]odo | [D]oing | D[o]ne" << endl;
            cin >> input;
            Item bItem;
            switch (input) {
                case 'I':
                case 'i':
                    bItem = boardItems[int_input];
                    bItem.state = ItemState::ICEBOX;
                    boardItems[int_input] = bItem;
                    break;
                case 'T':
                case 't':
                    bItem = boardItems[int_input];
                    bItem.state = ItemState::TODO;
                    boardItems[int_input] = bItem;
                    break;
                case 'D':
                case 'd':
                    bItem = boardItems[int_input];
                    bItem.state = ItemState::DOING;
                    boardItems[int_input] = bItem;
                    break;
                case 'O':
                case 'o':
                    bItem = boardItems[int_input];
                    bItem.state = ItemState::DONE;
                    boardItems[int_input] = bItem;
                    break;
            }
        }

        if (input == 'd')
        {
            system("clear");
            cout << "Which item do you want to delete?:" << endl;
            for(int item_i = 0; item_i < boardItems.size(); item_i++)
            {
                cout << "[" << to_string(item_i) << "] " << boardItems[item_i].description << endl;
            }
            cin >> int_input;
            if(int_input < 0 || int_input >= boardItems.size())
            {
                break;
            }
            boardItems.erase(boardItems.begin()+int_input);
        }
        Save(boardItems);
    }
    return 0;
}
