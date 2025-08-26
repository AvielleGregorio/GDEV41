#include <string.h>
#include <iostream>
#include <vector>

using namespace std;

void printGrid(int num_rows, int num_columns, int playerrow, int playercol, int enemyrow, int enemycol){
  
    vector<vector<string>> grid;

    for (int i = 0; i < num_rows; i++) {
    vector<string> row;
    for (int j = 0; j < num_columns; j++) {
      if (j == 0){
        if(i == playerrow && j == playercol){
           row.push_back("|P|"); 
        }else if(i == enemyrow && j == enemycol){
           row.push_back("|E|"); 
        }
        else{
           row.push_back("|_|"); 
        }
      }
      else{
        if(i == playerrow && j == playercol){
           row.push_back("P|"); 
        }else if(i == enemyrow && j == enemycol){
           row.push_back("E|"); 
        }
        else{
           row.push_back("_|");
        }
      }
    }
    grid.push_back(row);
  }

  for (int i = 0; i < num_rows; i++) {
    for (int j = 0; j < num_columns; j++) {
      cout << grid[i][j].c_str();
    }
    cout << "\n";
  }

}


int main() {
  int num_rows;
  int num_columns;
  int playerrow;
  int playercol;
  int enemyrow;
  int enemycol;
  vector<vector<string>> grid;

  cout << "Enter number of rows: ";
  cin >> num_rows;
  cout << "Enter number of columns: ";
  cin >> num_columns;

  cout << "What is the position of the Player? (row and column, e.g. 1 2): ";  
  cin >> playerrow >> playercol;
  cout << "What is the position of the Enemy? (row and column, e.g. 1 2): ";
  cin >> enemyrow >> enemycol;


//This checks and makes sure that the player and enemy dont start in the same room agad agad, but in the future this can be like an instant gameover?
  if((playerrow == enemyrow) && (playercol == enemycol)){
    cout << "Invalid position! Make sure the player and enemy start in different positions within the grid." << endl;
    return 0;
  }

  printGrid(num_rows, num_columns, playerrow, playercol, enemyrow, enemycol);

  return 0;
}

