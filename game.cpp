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
  string playerinput;
  bool defeated;

  vector<vector<string>> grid;

  cout << "Enter number of rows: ";
  cin >> num_rows;
  cout << "Enter number of columns: ";
  cin >> num_columns;

  //AVIELLE NOTE: I haven't made a provision against wrong input for the position of the player kasi like what
  //if they go beyond the grid theyve set?
  cout << "What is the position of the Player? (row and column, e.g. 1 2): ";  
  cin >> playerrow >> playercol;
  cout << "What is the position of the Enemy? (row and column, e.g. 1 2): ";
  cin >> enemyrow >> enemycol;

  printGrid(num_rows, num_columns, playerrow, playercol, enemyrow, enemycol);

  defeated = false;

  //Allowing the player to move within the grid with any of the following inputs: 
  //“north”, “south”, “east”, “west”, “n”, “s”, “e”, “w”. 
  //If the user inputs anything other than these possible inputs, the input is not registered. (7 pts)
  //AVIELLE NOTE! ^^idk if i did this the input is not registered because if i put anything else, since it doesnt
  //match in the if statements, it doesnt process it anyway
    while(defeated != true){
        cout << "Where do you want to go?: ";  
        cin >> playerinput;
        if(playerinput == "north" || playerinput == "n"){
          playerrow -= 1;
          printGrid(num_rows, num_columns, playerrow, playercol, enemyrow, enemycol);
        } else if(playerinput == "south" || playerinput == "s"){
          playerrow += 1;
          printGrid(num_rows, num_columns, playerrow, playercol, enemyrow, enemycol);
        } else if(playerinput == "east" || playerinput == "e"){
          playercol -= 1;
          printGrid(num_rows, num_columns, playerrow, playercol, enemyrow, enemycol);
        } else if(playerinput == "west" || playerinput == "w"){
          playercol += 1;
          printGrid(num_rows, num_columns, playerrow, playercol, enemyrow, enemycol);
        } else if (playerinput == "attack" || playerinput == "a"){
          if((playerrow == enemyrow) && (playercol == enemycol)){
            cout << "Defeated the enemy." << endl;
            defeated = true;
          }
        } else if (playerinput == "exit"){
          defeated = true;
        }
        
    }

  return 0;
}

