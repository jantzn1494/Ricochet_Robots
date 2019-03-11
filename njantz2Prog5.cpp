/*-------------------------------------------------------------------------------------
   njantz2Prog5.cpp
 
   Program #5: Ricochet Robots: Allow solving the sliding robots puzzle          
   Class:      CS 141, Spring 2018
   Author:     Nathan Jantz
   Lab:        Tuesday 1:00 pm
   System:     GNU C++ on Ubuntu 16.04
 
   There are a few tricky aspects to keep track of:
   1. A robot cannot move on top of another robot.
   2. A robot (numbered 1..4) that is not the goal robot can move on top of the destination
      letter.  If it does, then the letter disappears.  Once non-goal-robot moves off,
      then the goal letter must reappear.
 
   Grading Rubric:

   55 Execution points
        2 Displays header info and instructions on screen when run
        3 Displays the board (Since I'm giving you all the code for this...)
        5 Allows selecting default values (robot 2 to square M), random values, or user-selected values.
       15 Allows making moves, displaying move numbers.  Input can be upper or lower case, and can have spaces or not.
       20 Robots stop at walls and don't run over each other
        5 Game stops and message is given when goal robot makes it to the goal square.
        5 If some robot besides robot 2 goes to the destination square (square M in this case), the destination square should change to show the robot there. When     
          this robot moves away, the square should go back to displaying the letter that was there previously (M in our case.)

   45 Programming Style (Given only if program runs substantially correctly)
         5 Program naming convention is followed
         5 Meaningful identifier names
         5 Comments: Has header and this rubric.  Has comments on each block of code
        20 Appropriate data and control structures. Is implemented using classes.
        10 Code Layout: Appropriate indentation and blank lines
-------------------------------------------------------------------------------------*/
 

#include <iostream>
#include <cstring>      // for getline
#include <cstdlib>      // for the system command
#include <ctime>        // for randomizing goal
#include <cctype>       // For the letter checking functions, e.g. toupper( )
#include <fstream>      // For file input and output
using namespace std;

// Global data structure for board
const int BOARD_EDGE = 16;
const int BOARD_SIZE = BOARD_EDGE * BOARD_EDGE;
const int NUMBER_OF_ROBOTS = 4;
// declare constants to later use instead of numbers to select values from theBoard
const int NUMBER_OF_ELEMENTS = 5;   // Number of elements for each piece, enumerated below
const int PIECE = 0;    // to reference the character, such as in: theBoard[ i][ PIECE]
const int LEFT = 1;     // to reference the left wall character, such as in: theBoard[ i][ LEFT]
const int ABOVE = 2;    // to reference the above wall character, such as in: theBoard[ i][ ABOVE]
const int RIGHT = 3;    // to reference the right wall character, such as in: theBoard[ i][ RIGHT]
const int BELOW = 4;    // to reference the below wall character, such as in: theBoard[ i][ BELOW]


//----------------------------------------classes------------------------------------------

class Robot{
   public: //public class member functions
      Robot();                                        //default constructor
      Robot(int robotNum, int index, bool goalRobot); //constructor
      int getNum();                                   //returns robot number
      int getIndex();                                 //returns index of robot on theBoard
      void setIndex(int i);                           //sets index of robot
      bool getGoal();                                 //returns true if this robot is the goalRobot
      void setGoal(bool value);                       //sets goalRobot to true or false
      
   private://private members
       int robotNum;  //number value of robot name
       int index;     //index on theBoard
       bool goalRobot;//is this the goalRobot? Yes = true, No = false
};

//Constructors for Robot class
Robot::Robot(){ //default
   this->robotNum = 0;
   this->index = 0;
   this->goalRobot = false;
}

Robot::Robot(int robotNum, int index, bool goalRobot){
   this->robotNum = robotNum;
   this->index = index;
   this->goalRobot = goalRobot;
}

//Public member functions--getters and setters

int Robot::getNum(){
   int num = this->robotNum;
   return num;
}

int Robot::getIndex(){
   int index = this->index;
   return index;
}

bool Robot::getGoal(){
   bool value = goalRobot;
   return value;
}

void Robot::setIndex(int i){
   index = i;
}

void Robot::setGoal(bool value){
   goalRobot = value;
}


//---------------------------------------functions----------------------------------------------
void displayIdentifyingInformationAndInstructions()
{
    // Display identifying information
    cout << "Author: Nathan Jantz         " << endl
         << "Program 5: RicoRobots in C++ " << endl
         << "Lab: Tuesday 1pm             " << endl
         << "April 9, 2018                " << endl;

    // Display Instructions
    cout << "Welcome to Ricochet Robots                                     " << endl
         << "(\'Ricochet Robot\' is a registered trademark of Hans im Glück " << endl
         << "Munich, Germany, 1999. The game was created by Alex Randolph.) " << endl
         << endl
         << "The object of the game is to get the designated numbered robot " << endl
         << "to the indicated letter position.                              " << endl
         << "The numbered squares are all robots.  They unfortunately have  " << endl
         << "no brakes, and so will continue in whatever direction they are " << endl
         << "moving until they encountar an obstacle.                       " << endl
         << "For each move enter the robot number and the desired direction." << endl
         << "For instance entering                                          " << endl
         << "   1 U " << endl
         << "would move the #1 robot up as far as it can go.                " << endl
         << "The first letter of input is the robot number (1 - 4),         " << endl
         << "and the second letter is the direction:                        " << endl
         << "  (L=left, U=up, R=right, D=down)                              " << endl
         << "Enter x to exit.  Have fun!                                    " << endl
         <<  endl;
}//end displayIdentifyingInformationAndInstructions()


void createBoard( char theBoard[ BOARD_SIZE][ NUMBER_OF_ELEMENTS])
{
    for( int i=0; i<BOARD_SIZE; i++) {
        theBoard[ i][ PIECE]= '.';    // Set each playing piece default
        // set the default wall values to be blank
        theBoard[ i][ LEFT]= ' ';
        theBoard[ i][ ABOVE]= ' ';
        theBoard[ i][ RIGHT]= ' ';
        theBoard[ i][ BELOW]=  ' ';
        
        // reset the edge pieces
        // reset the left wall if piece is on left edge
        if( (i % BOARD_EDGE) == 0) {
            theBoard[ i][ LEFT]= '|';
        }
        // reset the above wall if piece is on top edge
        if( i < BOARD_EDGE) {
            theBoard[ i][ ABOVE]= '-';
        }
        // reset the right wall if piece is on right edge
        if( ((i+1) % BOARD_EDGE) == 0) {
            theBoard[ i][ RIGHT]= '|';
        }
        // reset the below wall if piece is on bottom edge
        if( i >= (BOARD_SIZE - BOARD_EDGE) ) {
            theBoard[ i][ BELOW]= '-';
        }
    }//end for( int i...
    
}//end createBoard()


void readFromFileAndModifyBoard(
        char theBoard[ BOARD_SIZE][ NUMBER_OF_ELEMENTS],
        int theRobots[ ],
        int &numberOfDestinationPieces)
{
    int numberOfSpecialSquares;  // Read from file, denotes number of pieces with some kind of wall
    string inputLine;            // Input line from file
    int pieceNumber;             // pieceNumber for each square defined in data file
    char c;
    
    ifstream inStream;           // declare an input file stream
    inStream.open("board.txt");  // Open input file, associating the actual file name with "inStream"
    if ( inStream.fail() ) {
        cout << "Input file opening failed.  Exiting...\n\n";
        exit(-1);
    }
    
    // Read from file one line at a time.  First read the top three data file lines which are documentation.
    getline( inStream, inputLine);
    getline( inStream, inputLine);
    getline( inStream, inputLine);
    
    inStream >> numberOfDestinationPieces;      // read how many destination pieces there are
    inStream >> numberOfSpecialSquares;         // number of square with walls around them somewhere
    inStream.get( c);                           // Get rid of return character at end of current line
                                                // to get ready for reading a line at a time below
    
    // process the special squares, updating the board
    for( int i=0; i<numberOfSpecialSquares; i++) {
        getline( inStream, inputLine);
        char *pInputLine = &inputLine[0];
        // Extract the piece number from the input line array
        sscanf( pInputLine, "%d", &pieceNumber);
        // Advance the inputLine pointer to the space after the number
        pInputLine = strchr( pInputLine, ' ');

        // Read the information on whether or not there is each of the potential 4 walls.
        // The four values read correspond in order to the pieces on all four sides,
        // which are defined as global constants for the values LEFT=1, ABOVE=2, RIGHT=3, BELOW=4
        for( int position=1; position<=4; position++) {
            pInputLine++;   // advance to next character to be read
            sscanf( pInputLine, " %c", &c);         // Read the next potential wall character
            // Convert it to a blank if it was '0'.  '0' was left in input file to make it easier to read.
            if( c=='0') {
                c = ' ';
            }
            theBoard[ pieceNumber][ position] = c;
        }
        pInputLine++;

        // See if there is a piece letter still to be read on this input line.
        // Do this by finding the end of the line, and seeing the length of the string
        // after that.
        // An inputline sample for piece 18 would be:
        // 18 00|- A
        if( strlen( pInputLine) > 0) {
            sscanf( pInputLine, " %c", &theBoard[ pieceNumber][ PIECE]);
        }

    }//end for( int i...
    
    // At the end of the data file read the information on the location of the robots, updating the board.
    char junk[80];           // used to read and discard input file information
    // Loop starts counting from 1 (rather than 0), since we ignore the 0th position
    // of the robots array, so that robot numbers correspond to what is shown
    // on the board.
    for( int i=1; i<=NUMBER_OF_ROBOTS; i++) {
        inStream >> theRobots[ i];
        inStream.getline( junk, '\n');    // discard rest of line, which is color information, if using graphics
        // set robot on board, converting the integer value to a character
        theBoard[ theRobots[ i]][ PIECE] = (char)('0'+i);     // record the robot number in place on board
    }
    
    inStream.close();         // close the input file stream
}//end readFromFileAndModifyBoard()


void DisplayBoard( char theBoard[ BOARD_SIZE][ NUMBER_OF_ELEMENTS])
{
    // display the top edge
    cout << "  ---------------------------------------------------------------- " << endl;
    
    // display the "body" of the board
    for( int i=0; i<BOARD_SIZE; i++) {
        // Figure out what character should be displayed to the left.  It will
        // be a wall if the current spot has a left wall, or if the spot to the
        // left has a right wall.
        
        char leftCharacter = theBoard[ i][ LEFT];   // set left display char
        // See if the piece to the left has a right wall.  Don't do this for the
        // first piece on the board, since it has no left neighbor.
        if ( (i>0) && (theBoard[ i-1][ RIGHT] != ' ')) {
            leftCharacter = theBoard[ i-1][ RIGHT];
        }
        cout << " " << leftCharacter << " " << theBoard[ i][ PIECE];
        // see if we're at the end of a row
        if( ((i+1) % BOARD_EDGE) == 0) {
            // we are at the end of a row, so display right wall and go to next line
            cout << "  " << theBoard[ i][ RIGHT] << endl;
            // Now display any walls immediately below the line of pieces just displayed
            // Backup our index counter j to the beginning of this line again, to find any
            // walls displayed below this line.  Don't do it for the bottom row though.
            if( i < BOARD_SIZE - BOARD_EDGE) {
            
                cout << " |";       // display the left boundary
                for( int j=i-BOARD_EDGE+1; j<(i+1); j++) {
                    // Set the character to be displayed.  This is a wall if the
                    // current spot has a wall below, or if the spot below has a wall
                    // above.
                    char belowCharacter = theBoard[ j][ BELOW];
                    // Only check the square below if we're NOT on the bottom row
                    if ( (j<(BOARD_SIZE - BOARD_EDGE)) &&      // verify not on bottom row
                        (theBoard[ j+16][ ABOVE] != ' ')) {    // piece below has wall above
                        belowCharacter = theBoard[ j+16][ ABOVE];
                    }
                    for( int i=0; i<3; i++) {
                       cout << belowCharacter;        // display the character
                    }
                    // display extra spaces, if we're not at the end of a row
                    if( ((j+1) % BOARD_EDGE) != 0) {
                        cout << " ";
                    }
                }//end for( int j...
                cout << " |" << endl;       // display the right boundary
            }//end if( i< BOARD_SIZE...

        }//end if( ((i+1...
        
    }//end for( int i=0;...
    
    // display the bottom edge
    cout << "  ---------------------------------------------------------------- " << endl;
    cout << endl;
}//end DisplayBoard()


bool MoveRobot( Robot &robot, char userDirection, char theBoard[ BOARD_SIZE][ NUMBER_OF_ELEMENTS]){
   bool returnVal = false;
   int currentIndex = robot.getIndex();
   cout << currentIndex << endl;
   if(userDirection == 'U'){                                   //User Direction is up
      //check for empty location or destinationLetter and check for walls
      while(((theBoard[currentIndex - 16][PIECE] == '.') || (theBoard[currentIndex - 16][PIECE] >= 'A'))      
              &&((theBoard[currentIndex][ABOVE] != '-') && (theBoard[currentIndex - 16][BELOW] != '-'))){
         int nextIndex1 = currentIndex - 16;                   //move up
         if(nextIndex1 >= 0){                                  //don't move past array values of theBoard
            theBoard[currentIndex][PIECE] = '.';               //change currentIndex of theBoard from robot # to '.'
            robot.setIndex(nextIndex1);                        //set robot index to new index
            currentIndex = robot.getIndex();                   //set currentIndex to new index
            theBoard[nextIndex1][PIECE] = '0' + robot.getNum();//robot displayed in new position
            returnVal = true;
         }
      }
   }
      
   else if(userDirection == 'D'){                              //User Direction is down
      //check for empty location or destinationLetter and check for walls
      while(((theBoard[currentIndex + 16][PIECE] == '.') || (theBoard[currentIndex + 16][PIECE] >= 'A'))
              &&((theBoard[currentIndex][BELOW] != '-') && (theBoard[currentIndex + 16][ABOVE] != '-'))){
         int nextIndex2 = currentIndex + 16;                   //move down
         if(nextIndex2 < BOARD_SIZE){                          //don't move past array values of theBoard
            theBoard[currentIndex][PIECE] = '.';               //change currentIndex of theBoard from robot # to '.'
            robot.setIndex(nextIndex2);                        //set robot index to new index
            currentIndex = robot.getIndex();                   //set currentIndex to new index
            theBoard[nextIndex2][PIECE] = '0' + robot.getNum();//robot displayed in new position
            returnVal = true;
         }
      }
   }
      
   else if(userDirection == 'L'){                              //User Direction is left
      //check for empty location or destinationLetter and check for walls
      while(((theBoard[currentIndex - 1][PIECE] == '.') || (theBoard[currentIndex - 1][PIECE] >= 'A'))
              &&((theBoard[currentIndex][LEFT] != '|') && (theBoard[currentIndex - 1][RIGHT] != '|'))){
         int nextIndex3 = currentIndex - 1;                    //move left
         if(nextIndex3 >= 0){                                  //don't move past array values of theBoard
            theBoard[currentIndex][PIECE] = '.';               //change currentIndex of theBoard from robot # to '.'
            robot.setIndex(nextIndex3);                        //set robot index to new index
            currentIndex = robot.getIndex();                   //set currentIndex to new index
            theBoard[nextIndex3][PIECE] = '0' + robot.getNum();//robot displayed in new position
            returnVal = true;
         }
      }
   }
      
   else if(userDirection == 'R'){                              //User Direction is right
      //check for empty location or destinationLetter and check for walls
      while(((theBoard[currentIndex + 1][PIECE] == '.') || (theBoard[currentIndex + 1][PIECE] >= 'A'))
              &&((theBoard[currentIndex][RIGHT] != '|') && (theBoard[currentIndex + 1][LEFT] != '|'))){
         int nextIndex4 = currentIndex + 1;                    //move right
         if(nextIndex4 < BOARD_SIZE){                          //don't move past array values of theBoard
            theBoard[currentIndex][PIECE] = '.';               //change currentIndex of theBoard from robot# to '.'
            robot.setIndex(nextIndex4);                        //set robot index to new index
            currentIndex = robot.getIndex();                   //set currentIndex to new index
            theBoard[nextIndex4][PIECE] = '0' + robot.getNum();//robot displayed in new position
            returnVal = true;
         }
      }
   }
   return returnVal;
}//end of MoveRobot()


void SetBoardDestination(char theBoard[ BOARD_SIZE][ NUMBER_OF_ELEMENTS], char destinationLetter){
   for(int i = 0; i < BOARD_SIZE; i++){ //iterates over theBoard values
      if(theBoard[i][PIECE] != '.'){    //if PIECE is not empty
         if((theBoard[i][PIECE] >= 'A') && (theBoard[i][PIECE] <= 'O')){ //and if PIECE contains a letter
            if(theBoard[i][PIECE] != destinationLetter){ //change PIECE to empty if not destinationLetter
               theBoard[i][PIECE] = '.';
            }
         }
      }
   }
}//end of SetBoardDestination()


void RandGoal(int &goalRobot, char &destinationLetter){
   srand(time(0)); //random seed
   int randNum = rand(); //random number
   goalRobot = (randNum % 4) + 1; //goal robot becomes 1, 2, 3, or 4
   destinationLetter = 'A' + (randNum % 15); //goal destination becomes A, B, ..., or O
}//end of RandGoal()


int SetGoalLocation( char theBoard[ BOARD_SIZE][ NUMBER_OF_ELEMENTS], char destinationLetter){
   int returnVal;
   for(int i = 0; i < BOARD_SIZE; i++){ //iterates over theBoard to find destinationLetter
      if(theBoard[i][PIECE] == destinationLetter){
         returnVal = i; //assigns returnVal to index of destinationLetter
      }
   }
   return returnVal;
}//end of SetGoalLocation()


bool Win(Robot robot, int goalLocation){
   bool returnVal = false; //default
   if (robot.getGoal() == true){ //if goalRobot is true
      if(robot.getIndex() == goalLocation){ //if index of robot is equal to goalLocation, then true
         returnVal = true;
      }
   }
   return returnVal;
}//end of Win()


void FindBoard(int &goalRobot, char &destinationLetter, char theBoard[ BOARD_SIZE][NUMBER_OF_ELEMENTS]){
   char userChoice;                             //user input value
   
   cout << "Enter 'r' for random robot and goal, 'd' for default or 's' to select: ";
   
   int val = 0;
   while (val == 0){     //val is 0 until goalRobot and destinationLetter are selected
      cin >> userChoice; //user input for default, random, or select goals
      userChoice = tolower(userChoice);//lowercase
      
      if (userChoice == 'd'){    //user chooses default goal
         goalRobot = 2;          //goalRobot default value
         destinationLetter = 'M';//destinationLetter default value
         val++;                  //adds to val
      }
      else if(userChoice == 'r'){               //user chooses random goal
         RandGoal(goalRobot, destinationLetter);//assigns random values to goalRobot and destinationLetter
         val++;                                 //adds to val
      }
      else if(userChoice == 's'){ //user chooses to select goal
         DisplayBoard( theBoard); //displays board with all robots and all destination letters
         cout << endl << "Please choose goal robot (number) and destination (letter). (e.g. 2 M): ";
         cin >> goalRobot;        //user choice of robot
         cin >> destinationLetter;//user choice of destination
         val++;                   //adds to val
      }
      else{ //userChoice is invalid
         cout << endl << userChoice << " is not a valid input. Enter 'r' for random robot and goal, 'd' for default or 's' to select: ";
      }
   }
}//end of FindBoard()

//----------------------------------------main---------------------------------------------


int main()
{
   // theBoard itself is represented by a one-dimensional array.
   // The other entries are used to keep track of walls.  Each row
   // in theBoard array will contain the character to be displayed,
   // followed by any "walls" to be displayed around the displayed
   // character.  For instance the first couple of entries are be:
   //
   //       PIECE   LEFT ABOVE RIGHT BELOW
   //      ---0---  --1-  --2-  --3-  --4-
   //    0    .       |     _
   //    1    .             _
   //   ...
   //   255   .                   |     _
   //
   char theBoard[ BOARD_SIZE][ NUMBER_OF_ELEMENTS];
   // Declare the 4 robots, which will store the board position of where they are found
   // Robots are displayed using values 1-4.  We declare 5 robots, though we will never use
   // the 0th robot in the array, so that we can use the robot digit itself as the index.
   int theRobots[ NUMBER_OF_ROBOTS + 1] = {0,0,0,0,0};

   int goalRobot;                  // the robot that needs to reach the goal
   char destinationLetter;         // the goal letter
   int numberOfDestinationPieces;  // Number of destination pieces.  Value
                                 //    comes from the data file.

   displayIdentifyingInformationAndInstructions();
   createBoard( theBoard);   // create the board, initializing all pieces to their default settings
   readFromFileAndModifyBoard( theBoard, theRobots, numberOfDestinationPieces); // read from the datafile, updating board

   //Create all Robots
   Robot one( 1, theRobots[1], false);
   Robot two( 2, theRobots[2], false);
   Robot three( 3, theRobots[3], false);
   Robot four( 4, theRobots[4], false);
   
   FindBoard(goalRobot, destinationLetter, theBoard);               //Prompts and creates board based on user input
   system("clear");                                                 //clear screen
   
   SetBoardDestination(theBoard, destinationLetter);                //clears board of all letters except the goal
   int goalLocation = SetGoalLocation(theBoard, destinationLetter); //Sets goal location to index of destination letter
   
   //set value of goalRobot to true
   for(int i = 1; i <= 4; i++){
      if(goalRobot == i){
         switch (i){
            case 1:
               one.setGoal(true);
               break;
            case 2:
               two.setGoal(true);
               break;
            case 3:
               three.setGoal(true);
               break;
            case 4:
               four.setGoal(true);
               break;
         }
      }
   }
   
   int moveIndex = 1;      //User's first turn
   int userRobot;          //User's choice of robot to move
   char userDirection;     //User's choice of direction
   bool winVal = false;    //True when User gets goalRobot to destinationLetter
   
   while(winVal == false){ //While user has not won
      //prompt user and display board
      cout << "Move robot " << goalRobot << " to square " << destinationLetter << endl;
      DisplayBoard( theBoard);
      cout << moveIndex << ". Please enter the robot to move and the direction (e.g. 2 r): ";
      //Recieve user values
      cin >> userRobot;
      cin >> userDirection;
      userDirection = toupper(userDirection); //changes user value to uppercase
      cout << endl;
      
      int oldIndex;        //stores old index of robot last moved
      switch (userRobot){  //Cases are based upon which robot user wishes to move
         case 1:
            oldIndex = one.getIndex();                            //set old index
            if(MoveRobot( one, userDirection, theBoard) == true){ //MoveRobot returns true when index values have changed
               moveIndex++;                                       //Move completed
               system("clear");                                   //clear screen
               winVal = Win(one, goalLocation);                   //Did last move cause user to win?
               if (oldIndex == goalLocation){                     //If robot was covering destinationLetter on theBoard, change back to destinationLetter
                  theBoard[goalLocation][PIECE] = destinationLetter;
               }
            }
            else{          //User's input was invalid, prompts to try again
               system("clear");
               cout << "Unable to make robot " << userRobot << " move in designated direction. Please try again." << endl;
            }
            break;
         case 2:
            oldIndex = two.getIndex();                            //set old index
            if(MoveRobot( two, userDirection, theBoard) == true){ //MoveRobot returns true when index values have changed
               moveIndex++;                                       //Move completed
               system("clear");                                   //clear screen
               winVal = Win(two, goalLocation);                   //Did last move cause user to win?
               if (oldIndex == goalLocation){                     //If robot was covering destinationLetter on theBoard, change back to destinationLetter
                  theBoard[goalLocation][PIECE] = destinationLetter;
               }
            }
            else{          //User's input was invalid, prompts to try again
               system("clear");
               cout << "Unable to make robot " << userRobot << " move in designated direction. Please try again." << endl;
            }
            break;
         case 3:
            oldIndex = three.getIndex();                            //set old index
            if(MoveRobot( three, userDirection, theBoard) == true){ //MoveRobot returns true when index values have changed
               moveIndex++;                                         //Move completed
               system("clear");                                     //clear screen
               winVal = Win(three, goalLocation);                   //Did last move cause user to win?
               if (oldIndex == goalLocation){                       //If robot was covering destinationLetter on theBoard, change back to destinationLetter
                  theBoard[goalLocation][PIECE] = destinationLetter;
               }
            }
            else{          //User's input was invalid, prompts to try again
               system("clear");
               cout << "Unable to make robot " << userRobot << " move in designated direction. Please try again." << endl;
            }
            break;
         case 4:
            oldIndex = four.getIndex();                            //set old index
            if(MoveRobot( four, userDirection, theBoard) == true){ //MoveRobot returns true when index values have changed
               moveIndex++;                                        //Move completed
               system("clear");                                    //clear screen
               winVal = Win(four, goalLocation);                   //Did last move cause user to win?
               if (oldIndex == goalLocation){                      //If robot was covering destinationLetter on theBoard, change back to destinationLetter
                  theBoard[goalLocation][PIECE] = destinationLetter;
               }
            }
            else{          //User's input was invalid, prompts to try again
               system("clear");
               cout << "Unable to make robot " << userRobot << " move in designated direction. Please try again." << endl;
            }
            break;
      }//end of switch
   }//end of while loop

   DisplayBoard( theBoard);
   cout << "Congratulations! You reached the goal in " << (moveIndex - 1) << " moves. Thanks for playing!" << endl;

}//end main()


