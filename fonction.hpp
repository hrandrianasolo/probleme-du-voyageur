#include <vector>
#include <string>
#include <shared_mutex>
#include <random>
#include <mutex>
#include <chrono>
#include <thread>
#include <iostream>
#include <SDL2/SDL.h>
#include <random>
#include <map>

#define NBR_ANT 3
#define NBR_ROAD 10

int myRand(int min, int max);

class Node {    
    public:
        int posX;
        int posY;
        int distance_from_origine;
        int nbr_visited;

        float pheromone; 
        std::string road_name;
        std::map<int, Node> childs;

};

Node initGraphe(int window_weight, int window_height, int nbr_road);
int getRoadIdByName(Node graphe, std::string name);
void printGraphe(Node graphe);

class Ant {
    public:
        int posX;
        int posY;

        int current_pos_id;
        std::vector<int> road_visited;

};

std::vector<Ant> initAnt(int nbr_ant, Node & graphe);
bool isInAntVisited(int current_road_id , std::vector<int> road_visited);
void antMove(Node & graphe, Ant & ant, int nbr_road);