				
								 How to setup this project in local system


 		Be sure your Compiler Matches the SFML version 3.0.0((MinGW-W64 x86_64-ucrt-posix-seh, built by Brecht Sanders, r2) 14.2.0)

1. Setup Compiler and be sure it is working.

2. Clone this Repositories in your local System.

3. Add the path of SFML bin folder in environment.

4. Open the Command Palate (Ctrl + Shift + P)
	 i. Open Edit Configuration UI.
	 ii. Add the path of SFML include folder in Inlcude path option.

5. You are ready to go now.

6. To run use this code:
   
	To compile

     g++ main.cpp User.cpp Menu.cpp LevelSelection.cpp SudokuGame.cpp -I../SFML-3.0.0/include -L../SFML-3.0.0/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -o main
     
	  To run

    	./file_name 
