# Console Game Monopoly
Monopoly is a popular board game where players buy, trade, and develop properties to bankrupt their opponents and become the richest player. It’s a mix of strategy, negotiation, and luck, making it a classic family and party game. The game has many rules but for simlicity we changed some. For example if we step on space nobody owns we do not have decision if we want to buy it or not. If we have enought money we have to buy the space. If not we go bankrupt. More rules are written in `task.pdf
**The game board we used in our game :**
![First](https://i.imgur.com/xx1UNkk.png)
## Modes
The game uses the console output to show the differences in the game board. Outputs can be various and they depends on command line arguments we put in the console when we started playing. For catching arguments I used constrol structue switch statements. Here are the arguments we can use. They can not be combined except the "-n" argument. 
1. `-n` argument is used for picking how many players we want to play with. We can choose from 2 to 4. If we skip `-n` argument the game will start with 2 players.
2. `-s` argument is used for printing game board status every round. We can see how many spaces are there and if anyone owns them.
   ![Second](https://i.imgur.com/JJR6X8o.png)
3. `-p` argument is used for printing players status every round. We can see on what space player stands, how much money does he have or what spaces does he own.
   ![Third](https://i.imgur.com/VQCfFWi.png)
4. `-g` argument is the combination of both `-s` and `-p` arguments. There is also winner indicator printed every round
     ![Fourth](https://i.imgur.com/3Lgkk0L.png)

**Z4 2024**
