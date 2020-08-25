#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include "GraphObject.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Penelope;
class Actor;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool ObjectObstructs(double x, double y, Actor* me);
    bool ObjectOverlaps(double x, double y, double x2, double y2);
    void SetLevelComplete(bool a) {levelComplete = a; }
    
    // EXIT functions
    void ExitElse(double x, double y);
    void UpdateStatusLine();
    
    //PIT functions
    void PitElse(double x, double y);
    
    // GOODIES
    bool PenelopeGoodie(double x, double y);
    
    // PENELOPE
    void VaccinesInc();
    void FlamesInc();
    void MinesInc();
    
    // FLAME
    void flameAll(double x, double y);
    bool AddFire(double x, double y, int direction);
    
    // VOMIT
    bool vomitable(double x, double y);
    void vomitAll(double x, double y);
    void vomitAt(double x, double y, int direction);
    
    // LEVEL
    void setLevelDied(bool a) {levelDied = a;}
    void setLevelComplete(bool a) {levelComplete = a;}
    
    // LANDMINE
    void LandMineAt(double x, double y);
    bool landmineALL(double x, double y);
    void landmineExplode(double x, double y);
    
    // ZOMBIE
    void DumbZombieVaccine(double x, double y);
    Direction SmartZombiePlan(double x, double y);
    
    // CITIZEN
    void CitizenPlan(double x, double y, Actor *me);
    void Zombiefied(double x, double y);
    void DecCitizens() { m_citizens--; }
    
private:
    int DistanceToClosestZombie(double x, double y);
    Penelope* p;
    std::vector<Actor*> allActors;
    bool initClean;
    bool levelComplete;
    bool levelDied;
    int m_zombies;
    int m_citizens;
    
    // StatusLine members
    std::string StatusLine;
    
};

#endif // STUDENTWORLD_H_

