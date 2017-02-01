#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <cmath>
#include <SDL2/SDL.h>
#include <GL/glew.h>


#define INT_MAX 2147483647
#define WIDTH 800
#define HEIGHT 600

#define MAX_X 30
#define MAX_Y 30


struct xyz{
    xyz(float _x, float _y, float _z):x(_x),y(_y),z(_z){}
    xyz() :x(0),y(0),z(0){}
    float x;
    float y;
    float z;
};



float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}





class Node{
public:

    Node(xyz pos,int id): m_Id(id),m_Pos(pos), m_GScore(INT_MAX),m_FScore(INT_MAX){
        m_Previous = NULL;
    }

    Node(std::string name, xyz pos,int id) : m_Id(id),m_Name(name),m_Pos(pos),m_GScore(INT_MAX),m_FScore(INT_MAX){
        m_Previous = NULL;
    }
    xyz GetPosition() const {return m_Pos;}
    void SetPostion(xyz pos){ m_Pos = pos;}

    Node* GetPrevious() const {return m_Previous;}
    void SetPrevious(Node* previous){ m_Previous = previous;}

    std::vector<Node*> GetNeighbors() const {return m_Neighbors;}
    void SetNeighbors(std::vector<Node*> neighbors){m_Neighbors = neighbors;}
    void AddNeighbor(Node* neighbor){m_Neighbors.push_back(neighbor);}

    std::string GetName() const {return m_Name;}
    void SetName(std::string name){m_Name = name;}

    int GetId() const {return m_Id;}
    void SetId(int id){m_Id = id;}

    float GetGScore() const {return m_GScore;}
    void SetGScore(float gscore){m_GScore = gscore;}

    float GetFScore() const {return m_FScore;}
    void SetFScore(float fscore){m_FScore = fscore;}


private:

    int m_Id;
    Node* m_Previous;
    std::vector<Node*> m_Neighbors;
    std::string m_Name;
    xyz m_Pos;

    float m_GScore;
    float m_FScore;
};


float getDistance_Pythagoras(xyz a, xyz b){
    xyz temp(b.x-a.x,b.y-a.y,b.z-a.z);

    return std::sqrt(temp.x*temp.x+temp.y*temp.y+temp.z*temp.z);
}


Node* getNodeById(int id, std::vector<Node>& nodes){
/*    for(int i = 0; i< nodes.size();i++){
        if(nodes[i].GetId() == id) return &nodes[i];
    }

    return NULL;
*/

    for(int i = 0; i<nodes.size();i++){
        if(nodes[i].GetId() == id) return &nodes[i];
    }
    return NULL; 
}


Node* getNodeById(int id, std::vector<Node*>& nodes){


    for(int i = 0; i < nodes.size();i++){
        if(nodes[i]->GetId() == id) return nodes[i];
    }
    return NULL;


    
}


std::vector<Node> firstStage(std::string path){
    std::vector<Node> nodes;
    
    std::ifstream fp(path.c_str());

    std::string line;
    
   

    while(std::getline(fp,line)){
        std::istringstream iss(line);
        std::string word;
        std::vector<std::string> words;


        int id;
        float x;
        float y;
        float z;
        std::string name;


        while(std::getline(iss,word,';')){
            words.push_back(word); 
        }

        id = std::stoi(words[0]);
        x = std::stof(words[2]);
        y = std::stof(words[3]);
        z = std::stof(words[4]);
        name = words[1];

        Node n = Node(name,xyz(x,y,z),id);

     
        nodes.push_back(n);
        words.clear();


        id   = 0;
        x    = 0;
        y    = 0;
        z    = 0;
        name = std::string();

    }

    return nodes;

}

void secondStage(std::string path, std::vector<Node>& nodes){
    std::ifstream fp(path.c_str());

    std::string line;


    while(std::getline(fp,line)){
        std::istringstream iss(line);
        std::string word;
        std::vector<std::string> words;

        int id;

        while(std::getline(iss,word,';')){
            words.push_back(word); 
        }

        for(int i= 5; i< words.size();i++){
            if(getNodeById(std::stoi(words[i]),nodes)  != NULL){
                getNodeById(std::stoi(words[0]),nodes)->AddNeighbor(getNodeById(std::stoi(words[i]),nodes));
                getNodeById(std::stoi(words[i]),nodes)->AddNeighbor(getNodeById(std::stoi(words[0]),nodes));
            }
        }

        words.clear();
    }
    
}


void loadData(std::string path, std::vector<Node>& nodes){
    nodes = firstStage(path);
    secondStage(path,nodes);
/*
    for( Node n : nodes){
        std::cout << n.GetId()  << std::endl;
        std::cout << n.GetName()  << std::endl;
        std::cout << n.GetPosition().x << "  "<< n.GetPosition().y << "   "<< n.GetPosition().z  << std::endl;

        for(Node* ne : n.GetNeighbors()){
            std::cout << ne->GetId() <<";";
        }

        }*/
}


Node* getLowestFScore(std::vector<Node*>& nodes){
    Node* current = nodes[0];
    for(Node* n : nodes){
        if(n->GetFScore() < current->GetFScore()){
            current = n;
        }
    }
    return current;
}


int getIndexOfNodeById(int id,std::vector<Node*> nodes){
    for(int i=0; i<nodes.size();i++){
        if(nodes[i]->GetId() == id){
            return i;
        }
    }
    return -1;
}


int A_Search(int start, int end, std::vector<Node*>& nodes){


    std::vector<Node*> closedSet; //empty

    std::vector<Node*> openSet;
    openSet.push_back(getNodeById(start,nodes)); //only starting node

    openSet[0]->SetGScore(0);
    openSet[0]->SetFScore(getDistance_Pythagoras(openSet[0]->GetPosition(),getNodeById(end,nodes)->GetPosition()));


    while(!openSet.empty()){
        Node* current = getLowestFScore(openSet);

//        std::cout << "+++" << current->GetId() << std::endl;
        if(current->GetId() == end){
            //done
            //reconstruct path and return
            return 1;
        }
        closedSet.push_back(current);
        int index = getIndexOfNodeById(current->GetId(),openSet);
        openSet.erase(openSet.begin() + index);

        for(Node* neighbor : current->GetNeighbors()){
            
            if(getNodeById(neighbor->GetId(),closedSet) != NULL) continue;

            float tent_gScore = current->GetGScore() + getDistance_Pythagoras(current->GetPosition(),neighbor->GetPosition());

            if(getNodeById(neighbor->GetId(),openSet) == NULL){
                openSet.push_back(neighbor);
            }
            else if( tent_gScore >= neighbor->GetGScore()){
                continue;
            }

            neighbor->SetPrevious(current);
            neighbor->SetGScore(tent_gScore);
            neighbor->SetFScore(neighbor->GetGScore() + getDistance_Pythagoras(neighbor->GetPosition(),getNodeById(end,nodes)->GetPosition()));
        }

    }

    return -1;

}


int reconstructPath(Node* node,std::vector<Node*>& finalPath){
    
    Node* current = node;
    while(current->GetPrevious() != NULL){
        finalPath.push_back(current);
        current=current->GetPrevious();
    }
}



int main(int argc, char** argv){

    SDL_Window* win = SDL_CreateWindow("Path",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT,SDL_WINDOW_OPENGL);

    SDL_GL_CreateContext(win);
    glPointSize(5);

    int running = 1;
   


    std::vector<Node> nodes;
    loadData(argv[1],nodes);
    std::vector<Node*> working_nodes;

    for(int i = 0; i < nodes.size();i++){
        working_nodes.push_back(&nodes[i]);
    }

    std::vector<Node*> finalPath;

    

    if(A_Search(atoi(argv[2]),atoi(argv[3]),working_nodes) > 0){
        std::cout << "found a path!\n";
        reconstructPath(getNodeById(atoi(argv[3]),working_nodes),finalPath);
        for(int i=0;i< finalPath.size();i++){
            std::cout << finalPath[i]->GetName() << std::endl;
        }
    }else{
        std::cout << "didn't find a path!\n";
    }

    while(running){
        SDL_Event ev;
        SDL_PollEvent(&ev);
        if(ev.type == SDL_QUIT){
            running = 0;
        }
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_POINTS);
        for(Node n : nodes){
            float x,y;

            x = map(n.GetPosition().x,0,(float)MAX_X,-1,1);
            y = map(n.GetPosition().y,0,(float)MAX_Y,-1,1);

            glColor3f(1,1,1);
            glVertex3f(x,y,0);
        }
        glEnd();



        glBegin(GL_POINTS);
        for(Node* n : finalPath){
            float x,y;

            x = map(n->GetPosition().x,0,(float)MAX_X,-1,1);
            y = map(n->GetPosition().y,0,(float)MAX_Y,-1,1);
            glColor3f(1,0,0);
            glVertex3f(x,y,0);
        }
        glEnd();


        SDL_GL_SwapWindow(win);
    }

    return 0;
}







































