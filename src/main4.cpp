#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>
#include <fstream>
#include <map>
#include <iomanip>
#include <sstream>

using namespace std;

struct User {
    string password;
    vector<string> wrongQuestions;
    int pkWins;
    int pkCount;
    
    // Constructor for User
    User() : pkWins(0), pkCount(0) {}

    User(const string& pwd)
        : password(pwd), pkWins(0), pkCount(0) {}
};

map<string, User> users;

// Function prototypes
bool login(string& username);
void registerUser();
void generateQuestion(int& num1, int& num2, char& op);
void saveResults(const string& username, const string& mode, const vector<string>& questions, int score);
int fixedQuestionMode(const string& username);
int timedMode(const string& username);
void practiceFromWrongBook(const string& username);
void pkMode(const string& user1, const string& user2);
void loadUsers();
void saveUsers();

int main() {
    srand((unsigned)time(0));
    loadUsers();
    
    string username;
    int choice;
    cout << "��ӭʹ��С�ǿ������\n";
    
    while (true) {
        cout << "1. ��¼\n2. ע��\n3. �˳�\nѡ�������";
        cin >> choice;
        
        switch (choice) {
            case 1:
                if (login(username)) {
                    bool keepUsing = true;
                    while (keepUsing) {
                        cout << "1. �̶���Ŀ����\n2. �̶�ʱ��\n3. ���Ȿ��ϰ\n4. PKģʽ\n5. ע��\nѡ���ܣ�";
                        cin >> choice;
                        switch (choice) {
                            case 1:
                                fixedQuestionMode(username);
                                break;
                            case 2:
                                timedMode(username);
                                break;
                            case 3:
                                practiceFromWrongBook(username);
                                break;
                            case 4:
                                {
                                    string opponent;
                                    cout << "�����ս�û�����";
                                    cin >> opponent;
                                    if (users.find(opponent) != users.end()) {
                                        pkMode(username, opponent);
                                    } else {
                                        cout << "�û������ڡ�\n";
                                    }
                                }
                                break;
                            case 5:
                                keepUsing = false;
                                username.clear();
                                break;
                        }
                    }
                }
                break;
            case 2:
                registerUser();
                break;
            case 3:
                saveUsers();
                return 0;
            default:
                cout << "��Чѡ��\n";
                break;
        }
    }
}

bool login(string& username) {
    string password;
    cout << "�û�����";
    cin >> username;
    cout << "���룺";
    cin >> password;
    
    if (users.find(username) != users.end() && users[username].password == password) {
        cout << "��¼�ɹ���\n";
        return true;
    } else {
        cout << "�û������������\n";
        return false;
    }
}

void registerUser() {
    string username, password;
    cout << "ע�� - �����û�����";
    cin >> username;
    if (users.find(username) == users.end()) {
        cout << "�������룺";
        cin >> password;
        users[username] = User(password); // Use User constructor
        cout << "ע��ɹ���\n";
    } else {
        cout << "�û����Ѵ��ڡ�\n";
    }
}

void generateQuestion(int& num1, int& num2, char& op) {
    num1 = rand() % 100 + 1; // 1 to 100
    num2 = rand() % 100 + 1; // 1 to 100
    switch (rand() % 4) {
        case 0: op = '+'; break;
        case 1: op = '-'; break;
        case 2: op = '*'; break;
        case 3: 
            op = '/';
            while (num2 == 0 || num1 % num2 != 0) {
                num2 = rand() % 100 + 1;
            }
            break;
    }
}

void saveResults(const string& username, const string& mode, const vector<string>& questions, int score) {
    ofstream file("results.txt", ios::app);
    if (file.is_open()) {
        file << "�û���" << username << " ģʽ��" << mode << " ����ʱ�䣺" << time(0) << " ������" << score << endl;
        for (const string& q : questions) {
            file << q << endl;
        }
        file.close();
    } else {
        cout << "�޷���������\n";
    }
}

int fixedQuestionMode(const string& username) {
    vector<string> questions;
    int correctCount = 0;
    for (int i = 0; i < 30; ++i) {
        int num1, num2, userAnswer, correctAnswer;
        char op;
        generateQuestion(num1, num2, op);

        string question = to_string(num1) + " " + op + " " + to_string(num2) + " = ?";
        cout << "��Ŀ " << i+1 << ": " << question << " ";
        questions.push_back(question);

        cin >> userAnswer;
        switch (op) {
            case '+': correctAnswer = num1 + num2; break;
            case '-': correctAnswer = num1 - num2; break;
            case '*': correctAnswer = num1 * num2; break;
            case '/': correctAnswer = num1 / num2; break;
            default: correctAnswer = 0; break;
        }

        if (userAnswer == correctAnswer) {
            ++correctCount;
        } else {
            users[username].wrongQuestions.push_back(question); // Add to wrong book
        }
    }

    int score = (correctCount / 30.0) * 100;
    saveResults(username, "�̶���Ŀ����", questions, score);
    cout << "�÷֣�" << score << endl;
    return score;
}

int timedMode(const string& username) {
    vector<string> questions;
    time_t start = time(0);
    int correctCount = 0, totalCount = 0;
    while (difftime(time(0), start) < 180) { // 3 minutes
        int num1, num2, userAnswer, correctAnswer;
        char op;
        generateQuestion(num1, num2, op);
        
        string question = to_string(num1) + " " + op + " " + to_string(num2) + " = ?";
        cout << "��Ŀ: " << question << " ";
        questions.push_back(question);

        cin >> userAnswer;
        switch (op) {
            case '+': correctAnswer = num1 + num2; break;
            case '-': correctAnswer = num1 - num2; break;
            case '*': correctAnswer = num1 * num2; break;
            case '/': correctAnswer = num1 / num2; break;
            default: correctAnswer = 0; break;
        }

        if (userAnswer == correctAnswer) {
            ++correctCount;
        } else {
            users[username].wrongQuestions.push_back(question); // Add to wrong book
        }
        ++totalCount;
    }

    double accuracy = totalCount > 0 ? (correctCount / (double)totalCount) : 0;
    int score = correctCount * accuracy * 100;
    saveResults(username, "�̶�ʱ��", questions, score);
    cout << "�÷֣�" << score << endl;
    return score;
}

void practiceFromWrongBook(const string& username) {
    vector<string>& wrongQuestions = users[username].wrongQuestions;
    vector<string> questions;
    int correctCount = 0;
    for (int i = 0; i < wrongQuestions.size(); ++i) {
        int num1, num2, userAnswer, correctAnswer;
        char op;
        string question = wrongQuestions[i];
        
        // Parse num1, op, num2 from question
        sscanf(question.c_str(), "%d %c %d", &num1, &op, &num2);
        cout << "��Ŀ: " << question << " ";
        questions.push_back(question);

        cin >> userAnswer;
        switch (op) {
            case '+': correctAnswer = num1 + num2; break;
            case '-': correctAnswer = num1 - num2; break;
            case '*': correctAnswer = num1 * num2; break;
            case '/': correctAnswer = num1 / num2; break;
            default: correctAnswer = 0; break;
        }

        if (userAnswer == correctAnswer) {
            ++correctCount;
            wrongQuestions.erase(wrongQuestions.begin() + i);
            --i; // adjust index after erase
        }
    }
    cout << "��ϰ��ɣ���ȷ����" << correctCount << "�����Ⲿ�Ѹ��¡�\n";
}

void pkMode(const string& user1, const string& user2) {
    cout << "�û� " << user1 << " ��ʼ\n";
    int score1 = fixedQuestionMode(user1);
    cout << "�û� " << user2 << " ��ʼ\n";
    int score2 = fixedQuestionMode(user2);
    
    users[user1].pkCount++;
    users[user2].pkCount++;
    
    if (score1 > score2) {
        cout << user1 << " Ӯ�ˣ�\n";
        users[user1].pkWins++;
    } else if (score2 > score1) {
        cout << user2 << " Ӯ�ˣ�\n";
        users[user2].pkWins++;
    } else {
        cout << "ƽ�֣�\n";
    }
}

void loadUsers() {
    ifstream file("users.dat", ios::in);
    if (file.is_open()) {
        while (!file.eof()) {
            string username, password, question;
            int pkWins, pkCount;
            file >> username >> password >> pkWins >> pkCount;
            
            User user(password);
            user.pkWins = pkWins;
            user.pkCount = pkCount;

            while (getline(file, question) && question != "END") {
                if (!question.empty()) {
                    user.wrongQuestions.push_back(question);
                }
            }
            users[username] = user;
        }
        file.close();
    }
}

void saveUsers() {
    ofstream file("users.dat", ios::out);
    if (file.is_open()) {
        for (const auto& userPair : users) {
            const User& user = userPair.second;
            file << userPair.first << " " << user.password << " " << user.pkWins << " " << user.pkCount << endl;
            for (const string& question : user.wrongQuestions) {
                file << question << endl;
            }
            file << "END\n";
        }
        file.close();
    } else {
        cout << "�޷������û����ݡ�\n";
    }
}