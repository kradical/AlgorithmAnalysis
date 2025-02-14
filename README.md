# Analysis of Algorithms
## CSC 425 @ UVIC w/ Wendy Myrvold

## Assignment 1
### Dominating Set Certificate Checker
To run:
- navigate to the correct folder
- compile `gcc main.c`
- run `./a.out 0 < in.txt > out.txt` in terse mode
- run `./a.out 1 < in.txt > out.txt` in verbose mode

`out0KEY.text` is the expected terse output.

`out1KEY.text` is the expected verbose output.

## Assignment 2
### Minimum Dominating Set
To run:
- navigate to the correct folder
- compile `gcc main.c`
- run `./a.out < in.txt > out.txt`

To check if it is a correct dominating set use assignment 1:
- run `./a.out < in.txt | ../assignment1/a.out 0`

## Project
### Dominating Set Heuristics
This project consists of 3 heuristics:
- Random
- Random BFS
- Our own heuristic (Random Genetic for me)

To run:
- navigate to the correct folder
- compile `gcc main.c`
- run `./a.out 5 0 < in.txt > out.txt` in terse mode for 5 seconds
- run `./a.out 10 1 < in.txt > out.txt` in verbose mode for 10 seconds
