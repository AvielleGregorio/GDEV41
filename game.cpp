#include <string.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>

using namespace std;

// This function prints the game area
void printGrid(int num_rows, int num_columns, int playerrow, int playercol, int enemyrow, int enemycol){
  
    vector<vector<string>> grid;

    for (int i = 0; i < 2*num_rows+1; i++) {
      vector<string> row;
      // for (int j = 0; j < 2*num_columns+1; j++) {
        if (i==0) {
          cout << "┌";
          for (int k = 0; k < num_columns; k++) {
            cout << "───";
            if (k != num_columns-1) {
              cout << "┬";
            }
          }
          cout << "┐";
        } else if (i == 2*num_rows) {
          cout << "└";
          for (int k = 0; k < num_columns; k++) {
            cout << "───";
            if (k != num_columns-1) {
              cout << "┴";
            }
          }
          cout << "┘";
        } else {
          if (i%2==0) {
            cout << "├";
            for (int k = 0; k < num_columns; k++) {
              cout << "───";
              if (k != num_columns-1) {
                cout << "┼";
              }
            }
            cout << "┤";
          } else {
            cout << "│ ";
            for (int k = 0; k < num_columns; k++) {
              if (playerrow == (i-1)/2 && playercol == k) {
                cout << "P ";
              } else if (enemyrow == (i-1)/2 && enemycol == k) {
                cout << "E ";
              } else {
                cout << "  ";
              }
              if (k != num_columns-1) {
                cout << "│ ";
              }
            }
            cout << "│";
          }
        }
      // }
      cout << endl;
    }

  // for (int i = 0; i < 2*num_rows+1; i++) {
  //   for (int j = 0; j < 2*num_columns+1; j++) {
  //     cout << grid[i][j].c_str();
  //   }
  //   cout << "\n";
  // }

}

// This function moves the enemy in one direction
void moveEnemy(int enemyDirection, int num_rows, int num_columns, int &enemyRow, int &enemyCol) {
  switch (enemyDirection)
  {
  case 0:
    enemyRow = (enemyRow+num_rows-1)%num_rows;
    break;
  case 1:
    enemyRow = (enemyRow+1) % num_rows;
    break;
  case 2:
    enemyCol = (enemyCol+1)%num_columns;
    break;
  case 3:
    enemyCol = (enemyCol+num_columns-1)%num_columns;
    break;
  
  default:
    break;
  }
}

int main() {
  int num_rows;
  int num_columns;
  int playerrow;
  int playercol;
  int enemyrow;
  int enemycol;
  bool enemyMoves;
  int enemyDirection;
  string playerinput;
  bool defeated;

  cout << enemyDirection << endl;
  vector<vector<string>> grid;

  ifstream settings("settings.txt");

  // cout << "Enter number of rows: ";
  settings >> num_rows;
  // cout << "Enter number of columns: ";
  settings >> num_columns;

  // cout << "What is the position of the Player? (row and column, e.g. 1 2): ";  
  settings >> playerrow >> playercol;
  // cout << "What is the position of the Enemy? (row and column, e.g. 1 2): ";
  settings >> enemyrow >> enemycol;

  settings >> enemyMoves;

  if (enemyMoves) {
    // The sum of rows and columns determines which direction the enemy moves
    enemyDirection = (num_rows+num_columns)%4;
  } else {
    // If the settings dictate that the enemy shouldn't move, the direction value
    // is set to an undefined direction, meaning it won't move.
    enemyDirection = 4;
  }

  printGrid(num_rows, num_columns, playerrow, playercol, enemyrow, enemycol);

  defeated = false;

  //Allowing the player to move within the grid with any of the following inputs: 
  //“north”, “south”, “east”, “west”, “n”, “s”, “e”, “w”. 
  //If the user inputs anything other than these possible inputs, the input is not registered. (7 pts)

    // This handles the main game loop. It handles user input, success, and failure.
    while(defeated != true){
        cout << "Where do you want to go?: ";  
        cin >> playerinput;
        if(playerinput == "north" || playerinput == "n"){
          playerrow = (playerrow+num_rows-1)%num_rows;
          moveEnemy(enemyDirection, num_rows, num_columns, enemyrow, enemycol);
          printGrid(num_rows, num_columns, playerrow, playercol, enemyrow, enemycol);
        } else if(playerinput == "south" || playerinput == "s"){
          playerrow = (playerrow+1)%num_rows;
          moveEnemy(enemyDirection, num_rows, num_columns, enemyrow, enemycol);
          printGrid(num_rows, num_columns, playerrow, playercol, enemyrow, enemycol);
        } else if(playerinput == "east" || playerinput == "e"){
          playercol = (playercol+1)%num_columns;
          moveEnemy(enemyDirection, num_rows, num_columns, enemyrow, enemycol);
          printGrid(num_rows, num_columns, playerrow, playercol, enemyrow, enemycol);
        } else if(playerinput == "west" || playerinput == "w"){
          playercol = (playercol+num_columns-1)%num_columns;
          moveEnemy(enemyDirection, num_rows, num_columns, enemyrow, enemycol);
          printGrid(num_rows, num_columns, playerrow, playercol, enemyrow, enemycol);
        } else if (playerinput == "attack" || playerinput == "a"){
          if((playerrow == enemyrow) && (playercol == enemycol)){
            cout << "Defeated the enemy." << endl;
            defeated = true;
            break;
          }
        } else if (playerinput == "exit"){
          defeated = true;
          break;
        }
        
    }

  return 0;
}

