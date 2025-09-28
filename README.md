# Sonic-Heroes
Sonic Heroes implemented in OOP using CPP.
The project is implemented only using iostream and fstream, no advanced libraries like the vector or std::array are used.
This project starts with a menu where you start the game or see the highscores, which are saved inn a file and top 5 are viewed at once.
The levels are loaded when you start the game and then you have 3 players in front of you, the players are interchangable using the 'z' key.
Each player has their own ability, sonic can sprint, knuckles can break breakable walls and tails can fly.
Each movement and action is implemented as an animation including the abilities, specially when tails flies, he carries the other two along him and they are hanging below him
The game has coins and health implemented, along with enemies who have unique way of attack and damage.
The levels are written in a txt file and that txt file is read and loaded as a grid and the respective sprites are loaded and animated on the screen, moreover every block has their own dynamic; the brick block can be breaken by knuckles, the hanging blocks could be jumped over.
The project using the concepts of objec oriented programming, relying on polymorphism heavily to control the players and enemies.
There is a single rungame loop running which is implementing and updating the game every frame using polumorphism and dealing with every in game scenario on its own.
