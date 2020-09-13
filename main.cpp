#include "main.hpp"

///  Variable static pour les fonction de sld /// 
SDL_Window * fenetre(0);
SDL_Renderer * renderer;
TTF_Font *police = NULL;
SDL_Surface * texte = NULL;
SDL_Texture * affiche;

SDL_Color blanc = {255, 255, 255, SDL_ALPHA_OPAQUE};
SDL_Color rouge = {255, 0, 0, SDL_ALPHA_OPAQUE};
SDL_Rect Dest;

/*
* param weight : largeur de l'ecran
* param height : longeur de l'ecran
*
* Crée une fenête en SDL et verifie tous
* tous les paramètres necessaire pour ssa création
* 
* retrun void
*/
void createAndInitWindow(int weight, int height){		

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "Erreur lors de l'initialisation de la SDL : " << SDL_GetError() << std::endl;
        SDL_Quit();
    }
	
    fenetre = SDL_CreateWindow("Test SDL 2.0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, weight, height, SDL_WINDOW_SHOWN);

    if(fenetre == 0){
        std::cout << "Erreur lors de la creation de la fenetre : " << SDL_GetError() << std::endl;
        SDL_Quit();

    }

    renderer = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(renderer == NULL){
        std::cout << "Erreur lors de la creation de la renderer : " << SDL_GetError() << std::endl;
        SDL_Quit();
    }

    if(TTF_Init() == -1){
        std::cout << "Erreur lors de l'initialisation de TTF : " << TTF_GetError() << std::endl;
        SDL_Quit();
    }

    police = TTF_OpenFont("files/arial.ttf",50);
    if(!police) {
        std::cout << "Erreur lors de l'ouverture du TTF : " << TTF_GetError() << std::endl;
        SDL_Quit();
    }
     
    drawText();

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
}

/*
* Écrie les informations relative au projet
* à afficher sur l'écran
* 
* retrun void
*/
void drawText(){
    texte = TTF_RenderText_Blended(police," Ligne blanche : initialement le meilleur trajet", blanc);
    Dest.x = 5;
    Dest.y = 5;
    Dest.w = 400;
    Dest.h = 30;
    affiche = SDL_CreateTextureFromSurface(renderer, texte);
    SDL_RenderCopy(renderer, affiche, NULL,&Dest);

    texte = TTF_RenderText_Blended(police," Ligne rouge : le meilleur trajet apres le passage des fourmis", rouge);
    Dest.x = 5;
    Dest.y = 35;
    Dest.w = 500;
    Dest.h = 30;
    affiche = SDL_CreateTextureFromSurface(renderer, texte);
    SDL_RenderCopy(renderer, affiche, NULL,&Dest);
}

void updateRender() {
    SDL_RenderPresent(renderer);
}

void drawRect(int posx, int posy){

    SDL_Rect rect;
    rect.x = posx;
    rect.y = posy;
    rect.w = 5;
    rect.h = 5;

    SDL_RenderFillRect(renderer, &rect);
    updateRender();
}

/*
* param & graphe : le graphe principal contenant tous les chemins
* param isBegin : pour savoir quel coleur utiliser
* 
* Cette fonction vas simuler le trajet des fourmis
* Et vas dessiner le meilleur trajet
*
* retrun void
*/
void drawBestPath(Node & graphe, bool isBegin, int nbr_road){

    Ant ant;
    ant.current_pos_id = myRand(0, nbr_road);
    ant.road_visited.push_back(ant.current_pos_id);

    antMove(graphe, ant, nbr_road);
    for(int i = 0; i < nbr_road; i++){
        int depPosX = graphe.childs.at(ant.road_visited.at(i)).posX;
        int depPosY = graphe.childs.at(ant.road_visited.at(i)).posY;
        int destPosX = graphe.childs.at(ant.road_visited.at(i+1)).posX;
        int destPosY = graphe.childs.at(ant.road_visited.at(i+1)).posY;
    
        if(!isBegin){
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
        }

        SDL_RenderDrawLine(renderer, depPosX, depPosY, destPosX, destPosY);
        updateRender();
        SDL_Delay(800);
    }
}

int main(int argc, char ** argv) {

    if(argc < 3){
        std::cout << "Usage : ./antcoloni [NB_ROAD] [NB_ANT]" << std::endl;
        return 1;
    }
    
    if(atoi(argv[1]) < atoi(argv[2])){
        std::cout << "Info : [NB_ROAD] doit toujours être supérieure à [NB_ANT]" << std::endl;
        return 1;
    }

    int nbr_ant = 0;
    int nbr_road = 0;

    nbr_ant = atoi(argv[2]);
    nbr_road = atoi(argv[1]);

    std::vector<Ant> ants;
    std::vector<std::thread> tr;

    Node graphe;

    createAndInitWindow(800, 600);    

    graphe = initGraphe(800, 600, nbr_road);
    ants = initAnt(nbr_ant, graphe);

    std::cout << "\n" << std::endl;

    for(auto & road: graphe.childs){
        drawRect(road.second.posX, road.second.posY);
        SDL_Delay(800);
    }
    drawBestPath(graphe, true, nbr_road);
    
    for(int i = 0; i < 3; i++){
        for(auto & ant: ants){
            tr.push_back(std::thread(antMove, std::ref(graphe), std::ref(ant), nbr_road));
        }
        for(auto & thr : tr) {
            thr.join();
        }
            tr.clear();
    }

    SDL_Delay(2000);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    drawText();
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        
    for(auto & road: graphe.childs){
        drawRect(road.second.posX, road.second.posY);
        SDL_Delay(400);
    }

    drawBestPath(graphe, false, nbr_road);
    
    SDL_Event e;
    for (;;) {
        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT) {
            TTF_CloseFont(police);
            TTF_Quit();
            SDL_DestroyRenderer(renderer); 
            SDL_DestroyWindow(fenetre);
            SDL_Quit();
            break;
        }
    }

    return 0;
}
