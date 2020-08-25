#include "StudentWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include "Actor.h"
#include "Level.h"
#include <vector>
#include <string>
#include <iostream> // defines the overloads of the << operator
#include <sstream>
#include <cmath>
#include <iomanip>
#include <cstdlib>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_zombies(0), m_citizens(0)
{
}

StudentWorld::~StudentWorld() {
    if (!initClean)
        cleanUp();
}

int StudentWorld::init()
{
        // Initialize citizens to 0 every level or else it falls through
    m_citizens = 0;
        // TO MAKE SURE CONSECUTIVE CALLS OF CLEANUP() ARE OKAY
    initClean = false;
        // Code directly attained from lab manual
    Level lev(assetPath());
    ostringstream oss;
        // Use stringstreams to load level
    oss << "level0" << getLevel() << ".txt";
    string levelFile = oss.str();
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    else if (result == Level::load_fail_file_not_found)
        return GWSTATUS_PLAYER_WON;
        // Loads all objects in game
    else if (result == Level::load_success) {
        for (int i = 0; i < LEVEL_WIDTH; i++) {
            for (int j = 0; j < LEVEL_HEIGHT; j++) {
                if (lev.getContentsOf(i, j) == Level::player)
                    p = new Penelope(this, SPRITE_WIDTH*i,SPRITE_HEIGHT*j);
                else if (lev.getContentsOf(i, j) == Level::wall) {
                    Wall* w = new Wall(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j);
                    allActors.push_back(w);
                }
                else if (lev.getContentsOf(i, j) == Level::exit) {
                    Exit *e = new Exit(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j);
                    allActors.push_back(e);
                }
                else if (lev.getContentsOf(i, j) == Level::pit) {
                    Pit *p = new Pit(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j);
                    allActors.push_back(p);
                }
                else if (lev.getContentsOf(i, j) == Level::vaccine_goodie) {
                    Vaccine *v = new Vaccine(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j);
                    allActors.push_back(v);
                }
                else if (lev.getContentsOf(i, j) == Level::landmine_goodie) {
                    LandMine *l = new LandMine(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j);
                    allActors.push_back(l);
                }
                else if (lev.getContentsOf(i, j) == Level::gas_can_goodie) {
                    GasCan *g = new GasCan(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j);
                    allActors.push_back(g);
                }
                else if (lev.getContentsOf(i, j) == Level::dumb_zombie) {
                    DumbZombie* z = new DumbZombie(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j);
                    allActors.push_back(z);
                }
                else if (lev.getContentsOf(i, j) == Level::smart_zombie) {
                    SmartZombie* s = new SmartZombie(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j);
                    allActors.push_back(s);
                }
                else if (lev.getContentsOf(i, j) == Level::citizen) {
                    m_citizens++;
                    Citizen* c = new Citizen(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j);
                    allActors.push_back(c);
                }
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
        // Allow penelope to do something
    if (p->aliveStatus())
        p->doSomething();
        // Allow everyone else to do something
    for (vector<Actor*>::iterator it = allActors.begin(); it != allActors.end(); it++) {
        if ((*it)->aliveStatus()) {
            (*it)->doSomething();
                // Level terminates if Penelope is dead
            if (levelDied) {
                levelDied = false;
                decLives();
                playSound(SOUND_PLAYER_DIE);
                return GWSTATUS_PLAYER_DIED;
            }
                // Level terminates if level is complete!
            if (levelComplete) {
                levelComplete = false;
                playSound(SOUND_LEVEL_FINISHED);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }
        // Cleanup after all objects are given a chance to do something
    for (vector<Actor*>::iterator it = allActors.begin(); it != allActors.end(); it++) {
        if ((*it)->aliveStatus() == false) {
            delete *it;
            allActors.erase(it);
                // This line of code is important as it makes sure a null pointer isn't de-referenced
            it = allActors.begin();
        }
    }
        // Update status line on top
    UpdateStatusLine();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete p;
        // Use iterator to iterate through all actors and delete them
    for (vector<Actor*>::iterator it = allActors.begin(); it != allActors.end(); ) {
        delete *it;
        it = allActors.erase(it);
    }
    initClean = true;
}

void StudentWorld::UpdateStatusLine() {
        // Followed code from stringstreams page on website
    ostringstream oss;
        // Fills extra width with 0s
    oss.fill('0');
    oss << "Score: " << setw(6) << getScore();
        // Fills extra width spaces
    oss.fill(' ');
    oss << setw(9) << "Level: " << getLevel();
    oss << setw(9) << "Lives: " << getLives();
    oss << setw(12) << "Vaccines: " << p->GetVaccines();
    oss << setw(10) << "Flames: " << p->GetFlames();
    oss << setw(9) << "Mines: " << p->GetMines();
    oss << setw(12) << "Infected: " << p->infectedNum();
    StatusLine = oss.str();
    setGameStatText(StatusLine);
}

void StudentWorld::ExitElse(double x, double y) {
        // Penelope exits only if all citizens are exited
    if (ObjectOverlaps(x, y, p->getX(), p->getY()) && m_citizens == 0) {
        levelComplete = true;
        return;
    }
        // Exits all citizens
    for (vector<Actor*>::iterator it = allActors.begin(); it != allActors.end(); it++) {
        if ((*it)->canExit() && ObjectOverlaps(x, y, (*it)->getX(), (*it)->getY()))
            (*it)->Exit();
    }
}

void StudentWorld::PitElse(double x, double y) {
    if (ObjectOverlaps(x, y, p->getX(), p->getY())) {
            // Terminate level since penelope died
        levelDied = true;
        return;
    }
    
    for (vector<Actor*>::iterator it = allActors.begin(); it != allActors.end(); it++) {
        if ((*it)->canLandMine() && (*it)->obstructs() && (*it)->aliveStatus() && ObjectOverlaps(x, y, (*it)->getX(), (*it)->getY())) {
            (*it)->setAliveStatus(false);
                // Zombies & Citizens die differently
            (*it)->Die();
        }
    }
}

bool StudentWorld::PenelopeGoodie(double x, double y) {
    if (ObjectOverlaps(x, y, p->getX(), p->getY())) {
        increaseScore(50);
            // Play corresponding sound
        playSound(SOUND_GOT_GOODIE);
        return true;
    }
    return false;
}

    // An important function used when determining movement for any function
bool StudentWorld::ObjectObstructs(double x, double y, Actor* me) {
    double x2 = p->getX();
    double y2 = p->getY();
        // Make sure object doesn't think it's overlapping with itself!!!!!
        // That's why the actor passes in its own pointer
    if (abs(x - x2) < SPRITE_WIDTH  && abs(y2 - y) < SPRITE_HEIGHT && (me != p))
        return true;
        // Simple math to make sure object doesn't obstruct
    for (vector<Actor*>::iterator it = allActors.begin(); it != allActors.end(); it++) {
        if ((*it)->obstructs() && (*it) != me) {
            double x2 = (*it)->getX();
            double y2 = (*it)->getY();
            if (abs(x - x2) < SPRITE_WIDTH  && abs(y2 - y) < SPRITE_HEIGHT)
                return true;
        }
    }
    return false;
}
    // Checks to see if any interacting objects overlap with one another
bool StudentWorld::ObjectOverlaps(double x, double y, double x2, double y2) {
    double xDiff = x2 - x;
    double yDiff = y2 - y;
        // Instead of 16 euclidean distance from above, it's 10
    if ((pow(xDiff, 2) + pow(yDiff, 2)) <= pow(10, 2))
        return true;
    return false;
}


    // When Penelope overlaps a goodie, these functions set the correct amount
void StudentWorld::VaccinesInc() {
    p->SetVaccines();
}
void StudentWorld::FlamesInc() {
    p->SetFlames();
}
void StudentWorld::MinesInc() {
    p->SetMines();
}

// FLAME
void StudentWorld::flameAll(double x, double y) {
    if (ObjectOverlaps(x, y, p->getX(), p->getY())) {
            // Terminate Level if Penelope dies by flame
        levelDied = true;
        return;
    }
    for (vector<Actor*>::iterator it = allActors.begin(); it != allActors.end(); it++) {
            // Flames can trigger landmines
        if ((*it)->canExplode() && ObjectOverlaps(x, y, (*it)->getX(), (*it)->getY()) && (*it)->aliveStatus()) {
            (*it)->setAliveStatus(false);
            landmineExplode((*it)->getX(), (*it)->getY());
        }
            // Otherwise flames kill everything (including goodies, zombies, citizens)
        else if ((*it)->canDamageFlame() && ObjectOverlaps(x, y, (*it)->getX(), (*it)->getY()) && (*it)->aliveStatus()) {
            (*it)->setAliveStatus(false);
            (*it)->Die();
        }
    }
}

    // Used by either Penelope's flamethrower or the landmine weapon
    // Boolean to make sure fires don't go past a certain iteration for Penelope's gun
bool StudentWorld::AddFire(double x, double y, int direction) {
    Flame *f;
    for (vector<Actor*>::iterator it = allActors.begin(); it != allActors.end(); it++) {
        if ((*it)->canBlockFlame() && ObjectOverlaps(x, y, (*it)->getX(), (*it)->getY()))
            return false;
    }
    f = new Flame(this, x, y, direction);
    // playSound(SOUND_PLAYER_FIRE);
    allActors.push_back(f);
    return true;
}

//VOMIT
    // Similar to fire but performs different actions
void StudentWorld::vomitAll(double x, double y) {
    if (ObjectOverlaps(x, y, p->getX(), p->getY())) {
            p->SetInfected(true);
            playSound(SOUND_ZOMBIE_VOMIT);
    }
    for (vector<Actor*>::iterator it = allActors.begin(); it != allActors.end(); it++) {
        if ((*it)->canDamageVomit() && ObjectOverlaps(x, y, (*it)->getX(), (*it)->getY()) && (*it)->aliveStatus())
        {
                // Other than Penelope, only citizens can get infected
            playSound(SOUND_CITIZEN_INFECTED);
            (*it)->SetInfected(true);
            playSound(SOUND_ZOMBIE_VOMIT);
        }
    }
}

    // Boolean function that checks to see if there's Penelope or Citizen that can be vomited on
    // at (x,y)
bool StudentWorld::vomitable(double x, double y) {
    if (ObjectOverlaps(x, y, p->getX(), p->getY())) {
        return true;
    }
    for (vector<Actor*>::iterator it = allActors.begin(); it != allActors.end(); it++) {
        if ((*it)->canDamageVomit() && ObjectOverlaps(x, y, (*it)->getX(), (*it)->getY()) && (*it)->aliveStatus())
        {
            return true;
        }
    }
    return false;
}

    // If previous function returns true then new vomit is instanced at (x,y)
void StudentWorld::vomitAt(double x, double y, int direction) {
    Vomit *v = new Vomit(this, x, y, direction);
    allActors.push_back(v);
}

// LANDMINE
    // When Penelope chooses to place a new landMine, this instances it
void StudentWorld::LandMineAt(double x, double y) {
    LandMineWep* l = new LandMineWep(this, x, y);
    allActors.push_back(l);
}

    // Called when landmine becomes active
bool StudentWorld::landmineALL(double x, double y) {
    if (ObjectOverlaps(x, y, p->getX(), p->getY())) {
            // can kill penelope
        setLevelDied(true);
        landmineExplode(x, y);
        return true;
    }
    for (vector<Actor*>::iterator it = allActors.begin(); it != allActors.end(); it++) {
        if ((*it)->canLandMine() && ObjectOverlaps(x, y, (*it)->getX(), (*it)->getY()) && (*it)->aliveStatus()) {
                // can be triggered by zombies & citizens
            (*it)->setAliveStatus(false);
            landmineExplode(x, y);
            (*it)->Die();
            return true;
        }
    }
    return false;
}

    // Creates new flames 8 boxes around landmine & places a pit in the middle
void StudentWorld::landmineExplode(double x, double y) {
    playSound(SOUND_LANDMINE_EXPLODE);
    Flame* f = new Flame(this, x, y, 0);
    allActors.push_back(f);
    // North
    f = new Flame(this, x, y + SPRITE_HEIGHT, 90);
    allActors.push_back(f);
    // South
    f = new Flame(this, x, y - SPRITE_HEIGHT, 90);
    allActors.push_back(f);
    // East
    f = new Flame(this, x + SPRITE_WIDTH, y , 90);
    allActors.push_back(f);
    // West
    f = new Flame(this, x - SPRITE_WIDTH, y , 90);
    allActors.push_back(f);
    // Northwest
    f = new Flame(this, x - SPRITE_WIDTH, y + SPRITE_HEIGHT, 90);
    allActors.push_back(f);
    // Northeast
    f = new Flame(this, x + SPRITE_WIDTH, y + SPRITE_HEIGHT, 90);
    allActors.push_back(f);
    // Southwest
    f = new Flame(this, x - SPRITE_WIDTH, y - SPRITE_HEIGHT, 90);
    allActors.push_back(f);
    // SouthEast
    f = new Flame(this, x + SPRITE_WIDTH, y - SPRITE_HEIGHT, 90);
    allActors.push_back(f);
    Pit* p = new Pit(this, x, y);
    allActors.push_back(p);
}

    // When dumb zombie chooses to fling the vaccine this function instances it
void StudentWorld::DumbZombieVaccine(double x, double y) {
    Vaccine* v = new Vaccine(this, x, y);
    allActors.push_back(v);
}

    // Elaborate movement plan for a smart zombie
Direction StudentWorld::SmartZombiePlan(double x, double y) {
        // Check distance to Penelope
    int lowestDist(100000);
    Actor* closest = nullptr;
    double xDiff = (p->getX()) - x;
    double yDiff = (p->getY()) - y;
    int num = (pow(xDiff, 2) + pow(yDiff, 2));
    if (num < lowestDist) {
        lowestDist = num;
        closest = p;
    }
        // Check to see if any citizen is closer than Penelope
    for (vector<Actor*>::iterator it = allActors.begin(); it != allActors.end(); it++) {
        if ((*it)->canDamageVomit()) {
            double xDiff = ((*it)->getX()) - x;
            double yDiff = ((*it)->getY()) - y;
            int num = (pow(xDiff, 2) + pow(yDiff, 2));
            if (num < lowestDist) {
                lowestDist = num;
                closest = *it;
            }
        }
    }
        // Follow algorithm (detailed in pseudcode in report) to get closer to Citizen/Penelope
    if (lowestDist <= pow(80, 2)) {
        if (closest->getX() == x && closest->getY() != y) {
            if (closest->getY() > y)
                return 90;
            else
                return 270;
        }
        if (closest->getX() != x && closest->getY() == y) {
            if (closest->getX() > x )
                return 0;
            else
                return 180;
        }
        else {
            if (randInt(1, 2) == 2) {
                if (closest->getX() > x)
                    return 0;
                else
                    return 180;
            }
            else {
                if (closest->getY() > y )
                    return 90;
                else
                    return 270;
            }
        }
    }   // if nothing works, go back to DumbZombie() movements
    else {
        switch (randInt(1, 4)) {
            case 1:
                return 90;
            case 2:
                return 270;
            case 3:
                return 180;
            default:
                return 0;
        }
    }
}

void StudentWorld::CitizenPlan(double x, double y, Actor* me) {
        // Get Distance to penelope
    double xDiff = (p->getX()) - x;
    double yDiff = (p->getY()) - y;
    int p_dist = (pow(xDiff, 2) + pow(yDiff, 2));
    int z_dist(100000);
    Actor *z;
        // Get Distance to closest zombie
    for (vector<Actor*>::iterator it = allActors.begin(); it != allActors.end(); it++) {
        if ((*it)->canDamageFlame() && !(*it)->canDamageVomit() && (*it)->obstructs() && (*it)->aliveStatus()) {
            double xDiff = ((*it)->getX()) - x;
            double yDiff = ((*it)->getY()) - y;
            int num = (pow(xDiff, 2) + pow(yDiff, 2));
            if (num < z_dist) {
                z_dist = num;
                z = *it;
            }
        }
    }   // If penelope is closer then try to get closer to penelope
    if ((p_dist < z_dist || z_dist == 100000) && p_dist <= pow(80, 2)) {
        if (p->getX() == x && p->getY() != y) {
            if (p->getY() > y) {
                if (!ObjectObstructs(x , y + 2, me)) {
                    me->setDirection(90);
                    me->moveTo(x , y + 2);
                    return;
                }
            } else {
                if (!ObjectObstructs(x , y - 2, me)) {
                    me->setDirection(270);
                    me->moveTo(x , y - 2);
                    return;
                }
            }
        } else if (p->getY() == y && p->getX() != x) {
            if (p->getX() > x) {
                if (!ObjectObstructs(x + 2,y , me)) {
                    me->setDirection(0);
                    me->moveTo(x + 2, y );
                    return;
                }
            } else {
                if (!ObjectObstructs(x - 2, y, me)) {
                    me->setDirection(180);
                    me->moveTo(x - 2, y );
                    return;
                }
            }
        } else if (p->getY() != y && p->getX() != x) {
            if (randInt(1, 2) == 2) {
                if (p->getX() > x) {
                    if (!ObjectObstructs(x + 2, y, me)) {
                        me->setDirection(0);
                        me->moveTo(x + 2, y);
                        return;
                    } else {
                        if (p->getY() > y) {
                            if (!ObjectObstructs(x, y + 2, me)) {
                                me->setDirection(90);
                                me->moveTo(x, y + 2);
                                return;
                            }
                        } else {
                            if (!ObjectObstructs(x, y - 2, me)) {
                                me->setDirection(270);
                                me->moveTo(x, y - 2);
                                return;
                            }
                        }
                    }
                } else {
                    if (!ObjectObstructs(x - 2, y, me)) {
                        me->setDirection(180);
                        me->moveTo(x - 2, y);
                        return;
                    } else {
                        if (p->getY() > y) {
                            if (!ObjectObstructs(x, y + 2, me)) {
                                me->setDirection(90);
                                me->moveTo(x, y + 2);
                                return;
                            }
                        } else {
                            if (!ObjectObstructs(x, y - 2, me)) {
                                me->setDirection(270);
                                me->moveTo(x, y - 2);
                                return;
                            }
                        }
                    }
                }
            } else {
                if (p->getY() > y) {
                    if (!ObjectObstructs(x, y + 2, me)) {
                        me->setDirection(90);
                        me->moveTo(x, y + 2);
                        return;
                    } else {
                        if (p->getX() > x) {
                            if (!ObjectObstructs(x + 2, y, me)) {
                                me->setDirection(0);
                                me->moveTo(x + 2, y);
                                return;
                            }
                        } else {
                            if (!ObjectObstructs(x - 2, y, me)) {
                                me->setDirection(180);
                                me->moveTo(x - 2, y);
                                return;
                            }
                        }
                    }
                } else {
                    if (!ObjectObstructs(x, y - 2, me)) {
                        me->setDirection(270);
                        me->moveTo(x, y - 2);
                        return;
                    } else {
                        if (p->getX() > x) {
                            if (!ObjectObstructs(x + 2, y, me)) {
                                me->setDirection(0);
                                me->moveTo(x + 2, y);
                                return;
                            }
                        } else {
                            if (!ObjectObstructs(x - 2, y, me)) {
                                me->setDirection(180);
                                me->moveTo(x - 2, y);
                                return;
                            }
                        }
                    }
                }
            }
        }
            // If Zombie is closer then try to get as far from zombie as possible
    } else if (z_dist <= pow(80,2)) {
        int lowestDist(-1), dir(-1);
            // Up direction
        if (!ObjectObstructs(me->getX(), me->getY() + 2, me)) {
            int num = DistanceToClosestZombie(me->getX(), me->getY() + 2);
            if (num > lowestDist) {
                lowestDist = num;
                dir = 90;
            }
        }
        if (!ObjectObstructs(me->getX(), me->getY() - 2, me)) {
            int num = DistanceToClosestZombie(me->getX(), me->getY() - 2);
            if (num > lowestDist) {
                lowestDist = num;
                dir = 270;
            }
        }
        if (!ObjectObstructs(me->getX() + 2, me->getY(), me)) {
            int num = DistanceToClosestZombie(me->getX() + 2, me->getY());
            if (num > lowestDist) {
                lowestDist = num;
                dir = 0;
            }
        }
        
        if (!ObjectObstructs(me->getX() - 2, me->getY(), me)) {
            int num = DistanceToClosestZombie(me->getX() - 2, me->getY());
            if (num > lowestDist) {
                lowestDist = num;
                dir = 180;
            }
        }
        if (lowestDist < z_dist)
            return;
        me->setDirection(dir);
        switch (dir) {
            case 90:
                me->moveTo(me->getX(), me->getY() + 2);
                return;
            case 270:
                me->moveTo(me->getX(), me->getY() - 2);
                return;
            case 0:
                me->moveTo(me->getX() + 2, me->getY());
                return;
            case 180:
                me->moveTo(me->getX() - 2, me->getY());
                return;
        }
    }
}

int StudentWorld::DistanceToClosestZombie(double x, double y) {
        // Helper function used in Citizen's moving plan function
        // Calculates distance to the closest zombie
    int z_dist(100000);
    for (vector<Actor*>::iterator it = allActors.begin(); it != allActors.end(); it++) {
        if ((*it)->canDamageFlame() && !(*it)->canDamageVomit() && (*it)->aliveStatus()) {
            double xDiff = ((*it)->getX()) - x;
            double yDiff = ((*it)->getY()) - y;
            int num = (pow(xDiff, 2) + pow(yDiff, 2));
            if (num < z_dist)
                z_dist = num;
        }
    }
    return z_dist;
}

    // Function that spawns zombie when a citizen gets vomited on
void StudentWorld::Zombiefied(double x, double y) {
        // 70% chance of a dumb zombie spawning & 30% of smart zombie spawning
    if (randInt(1, 10) >= 7) {
        DumbZombie* d = new DumbZombie(this, x, y);
        allActors.push_back(d);
    } else {
        SmartZombie* s = new SmartZombie(this, x, y);
        allActors.push_back(s);
    }
}

