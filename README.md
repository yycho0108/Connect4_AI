# Connect-4 AI
---

Minimax Q-Learning with Deep Neural Network for Connect-4

### Abstract

Prior to implementing Connect-4, I tried to apply simulating a Q-value function with a neural network that takes the current board state as input and output the corresponding value function. However, in both Tetris and 2048, the model became overly complex due to *imperfect information* (probabilistic computation of next-state). I have also previously had notable success with Tic Tac Toe, so I decided to model a relatively simple system: Connect-4, in which the number of possible actions are **constrained**, and the next state is **perfectly known**.

### Tasks
- [ ] Implement Game Logic
	- [ ] Placing Step
	- [ ] Evaluating Rewards
	- [ ] Computing Terminal State

- [ ] Implement Agent Logic
	- [ ] SARSA or pure Q-Learning?
	- [ ] Experience Replay
	- [ ] e-greedy agent
	- [ ] Validation with *Tables* in place of Neural Networks
	- [ ] Save/Load Agent
	- [ ] Generalizing Agent to learn for any game, given formatted input and output

### Documentation

- Parameters :
	- (@ [Agent.h](Agent.h)) alpha : learning rate
	- (@ [Agent.h](Agent.h)) epsilon : probability of random exploration
	- (@ [main.cpp](main.cpp)) n-m : board size
