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
    cout << "欢迎使用小登口算程序！\n";
    
    while (true) {
        cout << "1. 登录\n2. 注册\n3. 退出\n选择操作：";
        cin >> choice;
        
        switch (choice) {
            case 1:
                if (login(username)) {
                    bool keepUsing = true;
                    while (keepUsing) {
                        cout << "1. 固定题目数量\n2. 固定时间\n3. 错题本练习\n4. PK模式\n5. 注销\n选择功能：";
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
                                    cout << "输入对战用户名：";
                                    cin >> opponent;
                                    if (users.find(opponent) != users.end()) {
                                        pkMode(username, opponent);
                                    } else {
                                        cout << "用户不存在。\n";
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
                cout << "无效选择。\n";
                break;
        }
    }
}

bool login(string& username) {
    string password;
    cout << "用户名：";
    cin >> username;
    cout << "密码：";
    cin >> password;
    
    if (users.find(username) != users.end() && users[username].password == password) {
        cout << "登录成功！\n";
        return true;
    } else {
        cout << "用户名或密码错误。\n";
        return false;
    }
}

void registerUser() {
    string username, password;
    cout << "注册 - 输入用户名：";
    cin >> username;
    if (users.find(username) == users.end()) {
        cout << "输入密码：";
        cin >> password;
        users[username] = User(password); // Use User constructor
        cout << "注册成功！\n";
    } else {
        cout << "用户名已存在。\n";
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
        file << "用户：" << username << " 模式：" << mode << " 日期时间：" << time(0) << " 分数：" << score << endl;
        for (const string& q : questions) {
            file << q << endl;
        }
        file.close();
    } else {
        cout << "无法保存结果。\n";
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
        cout << "题目 " << i+1 << ": " << question << " ";
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
    saveResults(username, "固定题目数量", questions, score);
    cout << "得分：" << score << endl;
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
        cout << "题目: " << question << " ";
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
    saveResults(username, "固定时间", questions, score);
    cout << "得分：" << score << endl;
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
        cout << "题目: " << question << " ";
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
    cout << "练习完成，正确数：" << correctCount << "，错题簿已更新。\n";
}

void pkMode(const string& user1, const string& user2) {
    cout << "用户 " << user1 << " 开始\n";
    int score1 = fixedQuestionMode(user1);
    cout << "用户 " << user2 << " 开始\n";
    int score2 = fixedQuestionMode(user2);
    
    users[user1].pkCount++;
    users[user2].pkCount++;
    
    if (score1 > score2) {
        cout << user1 << " 赢了！\n";
        users[user1].pkWins++;
    } else if (score2 > score1) {
        cout << user2 << " 赢了！\n";
        users[user2].pkWins++;
    } else {
        cout << "平局！\n";
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
        cout << "无法保存用户数据。\n";
    }
}