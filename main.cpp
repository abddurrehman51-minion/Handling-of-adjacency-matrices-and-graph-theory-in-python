#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <filesystem>
using namespace std;
struct game_session {
    string P_1;
    string P_2;
    int p1_win = 0;
    int p2_win = 0;
    int draw   = 0;
};

// MENU / INPUT / DISPLAY
int main_menu(int Game_index);
int message(int Game_num);
int input();
void displayboard(char square[]);
void printGame(game_session *var);

// SAVE / LOAD / FILE UTILITIES
void savingGAMEtofile();
void current_stats();
int loadsavedGames(string filename, string &P1, string &P2);
int openfiles();
int existing_files(string &outName);
void resumeCheck(int resume, int load, game_session *var, int user_input);
void Playercheck(game_session *var, int *resume, int *load, int user_input);

// Game logic
int Game_board(int Grid_disp, char Grid_arr[]);
int game_interaction(int Grid_disp, char Grid_arr[], game_session *var, int resume);
int computer_game(int Grid_disp, char Grid_arr[], game_session *var, int resume);

// Win checkers
int checker(char arr[]);
int check_for_win(char square[]);

// Board modification
void mark_board(char square[], int choice, char mark);

// AI / Minimax
int minimax(char square[], int player);
int compmove(char square[], int player, int winner_name);
int functiontowin(char arr[], char player);

// Additional modern file functions
void stats(string P1, string P2, string Winner);
void save_progress(game_session var);
int loadGames(game_session *var, string filename);
void statsopen();
int fileexist(string &path);

// Take numeric input 
int input() {
    char ch;
    cin >> ch;

    int num = ch - '0';
    if (num >= 1 && num <= 9) return num;

    if (ch == '-') return -1;

    // Consume next character (if invalid)
    cin >> ch;
    num = ch - '0';
    return num;
}

// Display game board
void displayboard(char square[]) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    cout << "\n\n Tic Tac Toe \n\n";
    cout << "Player 1 (X) - Player 2 (O)\n\n";
    cout << "           |     |     \n";
    cout << "        " << square[0] << "  |  " << square[1] << "  |  " << square[2] << "\n";
    cout << "      _____|_____|_____\n";
    cout << "           |     |     \n";
    cout << "        " << square[3] << "  |  " << square[4] << "  |  " << square[5] << "\n";
    cout << "      _____|_____|_____\n";
    cout << "           |     |     \n";
    cout << "        " << square[6] << "  |  " << square[7] << "  |  " << square[8] << "\n";
    cout << "           |     |     \n";
}

// Mark a board index with X or O
void mark_board(char square[], int choice, char mark) {
    if (choice >= 1 && choice <= 9) {
        int index = choice - 1;
        if (square[index] == ('0' + choice)) {
            square[index] = mark;
            return;
        }
    }
    cout << "Invalid option\n";
}

// Original win-checker
int check_for_win(char square[]) {
    if (square[0] == square[1] && square[1] == square[2]) return 1;
    if (square[3] == square[4] && square[4] == square[5]) return 1;
    if (square[6] == square[7] && square[7] == square[8]) return 1;

    if (square[0] == square[3] && square[3] == square[6]) return 1;
    if (square[1] == square[4] && square[4] == square[7]) return 1;
    if (square[2] == square[5] && square[5] == square[8]) return 1;

    if (square[0] == square[4] && square[4] == square[8]) return 1;
    if (square[2] == square[4] && square[4] == square[6]) return 1;

    // Draw check
    for (int i = 0; i < 9; i++) {
        if (square[i] != 'X' && square[i] != 'O')
            return -1; // Game not finished
    }

    return 0; // Draw
}

// Second win-checker
int checker(char arr[]) {
    if ((arr[0] == arr[1] && arr[0] == arr[2]) ||
        (arr[0] == arr[3] && arr[0] == arr[6]) ||
        (arr[0] == arr[4] && arr[0] == arr[8])) return 1;

    if ((arr[2] == arr[4] && arr[2] == arr[6]) ||
        (arr[2] == arr[5] && arr[2] == arr[8])) return 1;

    if (arr[3] == arr[4] && arr[3] == arr[5]) return 1;

    if (arr[6] == arr[7] && arr[6] == arr[8]) return 1;

    if (arr[1] == arr[4] && arr[1] == arr[7]) return 1;

    return 0;
}

// Print entire game session
void printGame(game_session *var) {
    cout << "\nPlayer 1: " << var->P_1;
    cout << "\nPlayer 2: " << var->P_2;
    cout << "\nPlayer 1 win score: " << var->p1_win;
    cout << "\nPlayer 2 win score: " << var->p2_win;
    cout << "\nDraw games: " << var->draw;
}

// Game board updater (unchanged logic, but safer C++)
int Game_board(int Grid_disp, char Grid_arr[]) {
    displayboard(Grid_arr);
    return Grid_disp;
}

// 2-PLAYER game logic
int game_interaction(int Grid_disp, char Grid_arr[], game_session *var, int resume) {
    if (Grid_disp < 9) {
        int turn_in = 0;
        int Win_check;

    user1:
        // Player 1 turn
        if (Grid_disp % 2 == 0) {
            cout << "\nPlayer 1 " << var->P_1 << " turn. Type number: ";
            turn_in = input();

            if (turn_in > 0 && turn_in < 10 && Grid_arr[turn_in - 1] != 'X' && Grid_arr[turn_in - 1] != 'O') {
                Grid_arr[turn_in - 1] = 'X';
            } else {
                cout << "Enter Available places ";
                turn_in = 0;
                goto user1;
            }

            Win_check = checker(Grid_arr);
            if (Win_check == 1) {
                string Winner = "player 1 is winner";
                var->p1_win++;
                cout << "\n" << Winner;
                stats(var->P_1, var->P_2, Winner);
                return 10;
            }
        } 
        
        else {
        user2:
            // Player 2 turn
            cout << "\nPlayer 2 " << var->P_2 << " turn. Type number: ";
            turn_in = input();

            if (turn_in > 0 && turn_in < 10 && Grid_arr[turn_in - 1] != 'X' && Grid_arr[turn_in - 1] != 'O') {
                Grid_arr[turn_in - 1] = 'O';
            } else {
                cout << "Enter Available places ";
                turn_in = 0;
                goto user2;
            }

            Win_check = checker(Grid_arr);
            if (Win_check == 1) {
                string Winner = "player 2 is winner";
                var->p2_win++;
                stats(var->P_1, var->P_2, Winner);
                cout << "\n" << Winner;
                return 10;
            }
        }
    }

    // Draw condition
    if (Grid_disp == 9) {
        cout << "Draw match";
        var->draw++;
        stats(var->P_1, var->P_2, "Draw match");
    }

    return Grid_disp + 1;
}


//MINIMAX AI SYSTEM 


int minimax(char square[], int player) {
    int winner_name = check_for_win(square);
    int turn = -1;
    int score = -2;

    if (winner_name == 0) return 0;       // Draw
    if (winner_name != -1) {
        return (winner_name == 1 ? -1 : 1);
    }

    for (int i = 0; i < 9; i++) {
        if (square[i] != 'X' && square[i] != 'O') {
            square[i] = (player % 2 == 1 ? 'X' : 'O');
            int tempscore = -minimax(square, (player % 2 == 1 ? 2 : 1));
            square[i] = '0' + (i + 1);

            if (tempscore > score) {
                score = tempscore;
                turn = i;
            }
        }
    }
    return score;
}

// AI chooses the best move using minimax
int compmove(char square[], int player, int winner_name) {
    int turn = -1;
    int score = -2;

    for (int i = 0; i < 9; ++i) {
        if (square[i] != 'X' && square[i] != 'O') {
            square[i] = 'O';
            int tempScore = -minimax(square, (player % 2 == 1 ? 2 : 1));
            square[i] = '0' + (i + 1);

            if (tempScore > score) {
                score = tempScore;
                turn = i;
            }
        }
    }
    return turn;
}

// Quick-win detector 
int functiontowin(char arr[], char player) {
    char cpy_arr[9];
    int arr_index[9], count = 0, res;

    for (int i = 0; i < 9; i++)
        cpy_arr[i] = arr[i];

    for (int i = 0; i < 9; i++) {
        if (cpy_arr[i] != 'X' && cpy_arr[i] != 'O') {
            arr_index[count] = i;
            count++;
        }
    }

    for (int i = 0; i < count; i++) {
        char temp = cpy_arr[arr_index[i]];
        cpy_arr[arr_index[i]] = player;

        res = checker(cpy_arr);
        if (res == 1) return arr_index[i];

        cpy_arr[arr_index[i]] = temp;
    }

    return 10;
}


// =====================================================
//  1 PLAYER GAME (PLAYER VS COMPUTER)
// =====================================================

int computer_game(int Grid_disp, char Grid_arr[], game_session *var, int resume) {
    if (Grid_disp < 9) {
        int computer_in, computer_in2, user_in;
        int Win_check;

    user1:
        if (Grid_disp % 2 == 0) {
            cout << "\nPlayer 1 " << var->P_1 << " turn. Type number: ";
            user_in = input();

            if (user_in > 0 && user_in < 10 && Grid_arr[user_in - 1] != 'X' && Grid_arr[user_in - 1] != 'O') {
                Grid_arr[user_in - 1] = 'X';
            } else {
                cout << "Enter Available places ";
                goto user1;
            }

            Win_check = checker(Grid_arr);
            if (Win_check == 1) {
                string Winner = "player 1 is winner";
                var->p1_win++;
                cout << "\n" << Winner;
                stats(var->P_1, var->P_2, Winner);
                return 10;
            }
        }

        else {
            // Check where computer can win
            computer_in2 = functiontowin(Grid_arr, 'O');
            // Check where player will win and block it
            computer_in = functiontowin(Grid_arr, 'X');

            if (computer_in2 < 9 && computer_in2 >= 0) {
                Grid_arr[computer_in2] = 'O';
            }
            else if (computer_in < 9 && computer_in >= 0) {
                Grid_arr[computer_in] = 'O';
            }
            else {
            computer_move:
                srand(time(0));
                computer_in = rand() % 9;
                if (Grid_arr[computer_in] != 'X' && Grid_arr[computer_in] != 'O') {
                    Grid_arr[computer_in] = 'O';
                } else goto computer_move;
            }

            Win_check = checker(Grid_arr);
            if (Win_check == 1) {
                string Winner = "computer is winner";
                cout << "\n" << Winner;
                var->p2_win++;
                stats(var->P_1, var->P_2, Winner);
                return 10;
            }
        }
    }

    if (Grid_disp == 9) {
        cout << "Draw match";
        var->draw++;
        stats(var->P_1, var->P_2, "draw match");
    }

    return Grid_disp + 1;
}

// =====================================================
// SECTION 4 — FILE HANDLING (save, load, stats)
// =====================================================

// Save the ongoing (temporary) game data to a new file
void savingGAMEtofile() {
    string name;
    cout << "Enter the file name: ";
    cin >> name;

    string filename = "./tictactoe/" + name + ".txt";

    // If file exists, ask again
    while (filesystem::exists(filename)) {
        cout << "File already exists, choose another name: ";
        cin >> name;
        filename = "./tictactoe/" + name + ".txt";
    }

    // Copy sessions.txt → new file
    ifstream in("sessions.txt");
    ofstream out(filename);

    if (!in) {
        cout << "No temporary session to save.\n";
        return;
    }

    out << in.rdbuf();  // copy entire file

    in.close();
    out.close();

    filesystem::remove("sessions.txt");

    cout << "Your file name is " << name
         << ". Enter 6 from main menu to load saved games.\n\n";
}

// Display current (not yet saved) stats
void current_stats() {
    ifstream fp("sessions.txt");
    if (!fp) {
        cout << "No current stats available.\n";
        return;
    }

    char ch;
    while (fp.get(ch))
        cout << ch;

    fp.close();
}

// Save the game result to "stats.txt"
void stats(string P1_name, string P2_name, string Winner) {
    ofstream ptr("stats.txt", ios::app);
    ptr << "\n" << P1_name << " vs " << P2_name << ": " << Winner;
    ptr.close();
}

// Load saved games (append to sessions.txt)
int loadsavedGames(string filename, string &player1, string &player2) {
    if (!filesystem::exists(filename)) {
        cout << "No such file exists.\n";
        return 0;
    }

    ifstream file(filename);
    ofstream fp("sessions.txt", ios::app);

    fp << file.rdbuf();   // append entire file

    file.close();
    fp.close();

    return 0;
}

// Show all files inside ./tictactoe/
int openfiles() {
    string folder = "./tictactoe";

    if (!filesystem::exists(folder)) {
        cout << "Folder doesn't exist.\n";
        return 0;
    }

    for (auto &entry : filesystem::directory_iterator(folder)) {
        cout << entry.path().filename().string() << "\n";
    }

    return 1;
}

// Check if a file exists
int existing_files(string &nameOut) {
    string name;
    cout << "Enter the name of file: ";
    cin >> name;

    nameOut = "./tictactoe/" + name + ".txt";

    if (filesystem::exists(nameOut))
        return 0;

    cout << "Can't open file.\n";
    return 1;
}


// Save full game session including stats
void save_progress(game_session var) {
    string filename = "./Gaming_sessions/" + var.P_1 + "vs" + var.P_2 + ".txt";

    // If exists, ask overwrite or rename
    if (filesystem::exists(filename)) {
        int in;
        cout << "\nFile already exists.\n";
        cout << "Press 1 to type another name, 0 to overwrite: ";
        cin >> in;

        if (in == 1) {
            cout << "Type name: ";
            string newname;
            cin >> newname;
            filename = "./Gaming_sessions/" + newname + ".txt";
        }
    }

    ofstream fptr(filename);

    fptr << var.P_1 << "\n";
    fptr << var.P_2 << "\n";
    fptr << var.p1_win << "\n";
    fptr << var.p2_win << "\n";
    fptr << var.draw << "\n";

    fptr.close();

    cout << "\nYour game session is saved with name " << filename
         << ".\nTo resume it select 'resume game' from the menu.\n";
}


// Load a full game session
int loadGames(game_session *var, string filename) {
    string fullpath = "./Gaming_sessions/" + filename + ".txt";

    if (!filesystem::exists(fullpath)) {
        cout << "No such file.\n";
        return -1;
    }

    ifstream fptr(fullpath);
    if (!fptr) {
        cout << "Cannot open file. Retype the name.\n";
        return -1;
    }

    fptr >> var->P_1;
    fptr >> var->P_2;
    fptr >> var->p1_win;
    fptr >> var->p2_win;
    fptr >> var->draw;

    fptr.close();

    cout << "\nYour file has been loaded.\n"
         << "If you want to resume, exit and choose 'resume game' in menu.\n";

    return 0;
}


// Show all stats from stats.txt
void statsopen() {
    ifstream fptr("stats.txt");
    if (!fptr) {
        cout << "No stats found.\n";
        return;
    }

    char ch;
    while (fptr.get(ch))
        cout << ch;

    fptr.close();
}


// Check if file exists and handle renaming/overwrite
int fileexist(string &name) {
    if (!filesystem::exists(name))
        return 0;

    int in;
    cout << "\nFile already exists.\n";
    cout << "Press 1 to type another name or 0 to overwrite: ";
    cin >> in;

    if (in == 0) return 0;

    if (in == 1) {
        cout << "Type name: ";
        string newname;
        cin >> newname;
        name = "./Gaming_sessions/" + newname + ".txt";
        return 0;
    }

    return 0;
}
// =====================================================
// MENU SYSTEM, RESUME CHECK, PLAYER CHECK
// =====================================================

// Resume game setup and transitions
void resumeCheck(int resume, int load, game_session *var, int user_input) {
    if (resume == 0 && load == 1) {
        cout << "Game loaded. Choose 'resume game' to continue.\n";
        return;
    }
    if (resume == 1 && load == 0) {
        cout << "No saved game data is loaded.\n";
        return;
    }
    if (resume == 0 && load == 0) {
        cout << "Please load a game first using option 6.\n";
        return;
    }
}

// Let user type the player names and reset session variables
void Playercheck(game_session *var, int *resume, int *load, int user_input) {
    *resume = 0;
    *load   = 0;

    cout << "Enter the name of Player 1: ";
    cin >> var->P_1;

    if (user_input == 2) {
        var->P_2 = "Computer";
    } else {
        cout << "Enter the name of Player 2: ";
        cin >> var->P_2;
    }

    var->p1_win = 0;
    var->p2_win = 0;
    var->draw   = 0;
}


// Shows menu after ending a match
int message(int Game_num) {
    cout << "\nPress 1 for rematch\n";
    cout << "Press 2 for new game\n";
    cout << "Press 3 for exit\n";

    int ans;
    cin >> ans;

    if (ans == 1) return Game_num;   // rematch
    if (ans == 2) return -1;         // new game
    return -2;                       // exit
}


// Full MAIN MENU system 
int main_menu(int Game_index) {
    cout << "\n============================\n";
    cout << "          TIC TAC TOE       \n";
    cout << "============================\n\n";

    cout << "1. Two player game\n";
    cout << "2. Play with computer\n";
    cout << "3. Show stats\n";
    cout << "4. Show current temporary stats\n";
    cout << "5. Save current temporary session\n";
    cout << "6. Load saved session file\n";
    cout << "7. Resume loaded game\n";
    cout << "0. Exit\n\n";

    cout << "Enter your choice: ";
    cin >> Game_index;

    return Game_index;
}

#include <iostream>
#include <cstring>
using namespace std;

int main() {
    game_session currentGame;   // Holds current session stats
    int game_index = 0;         // Tracks number of games played
    int resume = 0;             // Resume flag
    int load = 0;               // Load flag
    int user_choice;
    char Grid_arr[9];           // Board array
    int Grid_disp;

    // Initialize board
    for(int i = 0; i < 9; i++) {
        Grid_arr[i] = '1' + i;  // '1' to '9'
    }

    while(true) {
        // Show menu and get user input
        user_choice = main_menu(game_index);

        if(user_choice == -1) {
            cout << "\nExiting game. Goodbye!" << endl;
            break;  // Exit loop
        }

        switch(user_choice) {
            case 1: // 2-player game
                resumeCheck(resume, load, &currentGame, 1);
                Grid_disp = 0;
                while(Grid_disp < 10) {
                    displayboard(Grid_arr);
                    Grid_disp = game_interaction(Grid_disp, Grid_arr, &currentGame, resume);
                }
                game_index++;
                break;

            case 2: // 1-player game
                resumeCheck(resume, load, &currentGame, 2);
                Grid_disp = 0;
                while(Grid_disp < 10) {
                    displayboard(Grid_arr);
                    Grid_disp = computer_game(Grid_disp, Grid_arr, &currentGame, resume);
                }
                game_index++;
                break;

            case 3: // Resume game
                if(load == 0) {
                    cout << "No saved game to resume!" << endl;
                } else {
                    cout << "Resuming game..." << endl;
                    // Implement load/resume logic here
                }
                break;

            case 4: // View stats
                statsopen();
                break;

            case 5: // Save current progress
                save_progress(currentGame);
                break;

            case 6: // Load a saved game
                {
                    char filename[100];
                    cout << "Enter file name to load: ";
                    cin >> filename;
                    loadGames(&currentGame, filename);
                    load = 1;
                }
                break;

            default:
                cout << "Invalid input. Try again!" << endl;
        }

        // Reset the board for next game
        for(int i = 0; i < 9; i++)
            Grid_arr[i] = '1' + i;
    }

    return 0;
}
