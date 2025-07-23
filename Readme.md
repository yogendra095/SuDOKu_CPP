### How to setup this project in local system
1. Clone this Repositories in your local System.
2. Add the path of bin folder in environment.
3. Open the Command Palate (Ctrl + Shift + P)<br>
&nbsp; i. Open Edit Configuration UI.<br>
&nbsp; ii. Add the path of include folder in Inlcude path option.
4. You are ready to go now.
5. To run use this code:
    ```
    //To Develop

    g++ file_name.cpp -I../SFML-3.0.0/include -L../SFML-3.0.0/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -o file_name

    AND
    //To run

    ./file_name 
    ```