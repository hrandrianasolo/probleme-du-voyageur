#include "fonction.hpp"

std::shared_mutex mutex_;

/*
* param min : debut du random
* param max : fin du random
* 
* Cette fonction nous donne un chiffre au hasard
* Entre les balise min et max
*
* retrun int
*/
int myRand(int min, int max){

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(min, max);

    int resultat = dist6(rng); 
    while( min > resultat || max < resultat){
        resultat = dist6(rng);
    }

    return resultat;
}

// Fonction conerant le graphe ///

/*
* param window_weight : largeur de l'ecran
* param window_height : longuer de l'ecran
* 
* Cette fonction vas créer et initialiser
* un graphe de type connexe.
* Les position de chaque noeud sur l'ecran
* sera au hasard d'ou la necessité du
* largeur et longeur de l'ecran
*
* Les distances entre les noueds sont calculé
* selon le théorème de pythagore
*
* retrun Node
*/
Node initGraphe(int window_weight, int window_height, int nbr_road){

    Node graphe;
    graphe.road_name = "Graphe principale";
    graphe.posX = 0;
    graphe.posY = 0;
    
    // Creation of all road //
    for(int i = 0; i < nbr_road; ++i){
        Node road;
        road.posX = myRand(window_weight/8,window_weight - window_weight/8);
        road.posY = myRand(window_height/8,window_height - window_height/8);
        road.distance_from_origine = 0;
        road.nbr_visited = 0;

        road.pheromone = 0.0000001;
        road.road_name = std::to_string(i);
        
        graphe.childs.insert(std::pair<int, Node>(i, road));
    }

    // road's child allocation //
    for(auto & road: graphe.childs){
        for(auto & child_road: graphe.childs){
            if(road.first != child_road.first){
                int parentPosX = road.second.posX;
                int parentPosY = road.second.posY;
                int childPosX = child_road.second.posX;
                int childPosY = child_road.second.posY;

                int distance =  sqrt(((parentPosX - childPosX)*(parentPosX - childPosX)) + ((parentPosY - childPosY)*(parentPosY - childPosY)));
            
                // creation of the child road
                Node current_child_road;
                current_child_road.posX = child_road.second.posX;
                current_child_road.posY = child_road.second.posY;
                current_child_road.distance_from_origine = distance;
                current_child_road.nbr_visited = 1;

                current_child_road.pheromone = 0.6;
                current_child_road.road_name = child_road.second.road_name;

                road.second.childs.insert(std::pair<int, Node>(child_road.first, current_child_road));
            }
        }
    }
    return graphe;
}

int getRoadIdByName(Node graphe, std::string name){
    for(std::pair<int, Node> road : graphe.childs){
        if(road.second.road_name.compare(name) == 0){
            return road.first;
        }
    }

    return -1;
}

void printGraphe(Node graphe){

    std::cout << "Name : " << graphe.road_name << " Child size : " << graphe.childs.size() <<std::endl;
    if(graphe.childs.size() == 0){
        return;
    }
    
    for(auto & road : graphe.childs){    
        printGraphe(road.second);
    }

    std::cout << "\n\n";
}


/// Fonction conerant les fourmis ///

/*
* param nbr_ant : nombre de fourmis
* param & graphe : le graphe
* 
* Cette fonction vas créer fourmis et
* initialier ses positions par rapport 
* au graphe
*
* retrun Node
*/
std::vector<Ant> initAnt(int nbr_ant, Node & graphe){
    std::vector<Ant> ant_list;

    for(int i = 0; i < nbr_ant; i++){
        Ant ant;
        bool next = false;
        int random_road_id = -1;
        while(!next){
            random_road_id = myRand(0, 9);
            if((random_road_id >= 0 && random_road_id <= (int) graphe.childs.size()-1)){
                ant.current_pos_id = random_road_id;
                ant.road_visited.push_back(random_road_id);
                next = true;
            }
        }
        ant_list.push_back(ant);
    }

    return ant_list;
}

bool isInAntVisited(int current_road_id , std::vector<int> road_visited){
    for(int id: road_visited){
        if(current_road_id == id){
            return true;
        }
    }
    return false;
}


/*
* param & graphe : le graphe
* param & Ant : le fourmi
* 
* Dans cette fonction, tant que le fourmi
* n'a pas visiter tous les noeud, il se deplacera
* vers les meilleurs chemin qu'il n'a pas encore visiter
* et qui aura le plus de phéromone. 
* retrun Node
*/
void antMove(Node & graphe, Ant & ant, int nbr_road){
    
    while((int) ant.road_visited.size() < nbr_road+1){
        
        float best_pheromone = -1.0;
        int best_road_id = -1;  
        std::pair<int, Node> best_road;
        float new_pheromone = 0;
        
        // check best pheromone among child road
        {
            std::shared_lock lock(mutex_);
            for(auto & road: graphe.childs.at(ant.current_pos_id).childs){
                if((int) ant.road_visited.size() < nbr_road){
                    if(!isInAntVisited(road.first ,ant.road_visited)) {
                        float pheromone = (road.second.pheromone * road.second.nbr_visited) / road.second.distance_from_origine;
                        if(best_pheromone < pheromone){
                            best_pheromone = pheromone;
                            best_road_id = getRoadIdByName(graphe, road.second.road_name);
                            best_road = road;
                        }
                    }
                }
                else{
                    if(road.first == ant.road_visited.front()) {
                        float pheromone = (road.second.pheromone * road.second.nbr_visited) / road.second.distance_from_origine;
                        best_pheromone = pheromone;
                        best_road_id = getRoadIdByName(graphe, road.second.road_name);
                        best_road = road;
                    }
                }

            }

        }

        //check accesibility and update data
        {
            if(best_road_id >= 0 && best_pheromone >= 0.0){
                std::unique_lock lock(mutex_);

                new_pheromone = (best_road.second.pheromone/2) * ((1/(nbr_road*best_pheromone)) * best_road.second.nbr_visited);
                
                for(auto & road: graphe.childs.at(ant.current_pos_id).childs){
                    if(road.second.road_name.compare(best_road.second.road_name)){
                        road.second.pheromone = new_pheromone;
                        break;
                    }
                }
                
                for(auto & road: graphe.childs.at(best_road_id).childs){
                    if(road.second.road_name.compare(graphe.childs.at(ant.current_pos_id).road_name)){
                        road.second.pheromone = new_pheromone;
                        break;
                    }
                }

                graphe.childs.at(ant.current_pos_id).childs.at(best_road_id).nbr_visited++;
                ant.current_pos_id = best_road_id;
                ant.road_visited.push_back(best_road_id);
            }
        }
    }
}
