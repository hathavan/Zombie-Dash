#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


class StudentWorld;

//*     ABC CLASS   *//
class Actor: public GraphObject {
    
public:
    Actor(StudentWorld* p, int imageID, double startX, double startY, int startDirection, int depth);
    
    virtual ~Actor() { return; }
        // Actor objects will never be instanced
    virtual void doSomething() = 0;
    bool aliveStatus() const { return m_alive; }
    void setAliveStatus(bool a) { m_alive = a;}
    
        //* Common methods to help rule out objects for various functions *//
    bool obstructs() const { return obstructType; }
    void setObstruct(bool val) { obstructType = val; }
    
    bool overlaps() const { return !obstructType; }
    
    bool canExit() const { return exitType; }
    void setCanExit(bool a) { exitType = a; }
    
    bool canDamageFlame() const { return damageFlame; }
    void setDamageFlame(bool a) {damageFlame = a; }
    
    bool canDamageVomit() const { return damagevomit; }
    void setDamageVomit(bool a) {damagevomit = a; }
    
    bool canBlockFlame() const {return blockflame; }
    void setBlockFlame(bool a) {blockflame = a; }
    
        // FOR LANDMINE
    bool canExplode() const { return m_explode; }
    void setExplode(bool a) { m_explode = a; }
    
        // FOR PPL, ZOMBIES
    bool canLandMine() const { return m_isExplodable; }
    void setExplodable(bool a) { m_isExplodable = a; }
    
        // For Penelope & Citizens
    bool IsInfected() const { return m_infected; }
    void IncInfected() { m_infectNum++; }
    void ClearInfected() { m_infectNum = 0;}
    void SetInfected(bool a) { m_infected = a; }
    int infectedNum() {return m_infectNum;}
    
    StudentWorld* getWorld() { return myWorld; }
    
    virtual void Die() { return; }
        // For citizens
    virtual void Exit() {return; }
    
private:
    StudentWorld* myWorld;
    bool obstructType;
    bool exitType;
    bool m_alive;
    bool damageFlame;
    bool blockflame;
    bool damagevomit;
    bool m_infected;
    int m_infectNum;
    bool m_explode;
    bool m_isExplodable;    
};

//class Person: public Actor {
//    Person(StudentWorld* p, double startX, startY);
//    virtual void doSomething() = 0;
//
//};

//*     PENELOPE CLASS      *//
class Penelope: public Actor {
public:
    Penelope(StudentWorld* p, double startX, double startY);
    virtual void doSomething();
    
    // Accessor Functions
    int GetVaccines() const { return m_vaccines; }
    int GetFlames() const { return m_flames; }
    int GetMines() const { return m_mines; }
    
    // Mutator Functions
    void SetVaccines() { m_vaccines++; }
    void SetFlames() { m_flames += 5;}
    void SetMines() {m_mines += 2;}
    
    
private:
    int m_mines;
    int m_flames;
    int m_vaccines;
};

//*     WALL CLASS    *//
class Wall: public Actor {
public:
    Wall(StudentWorld* p, double startX, double startY);
    // Walls do nothing
    virtual void doSomething() { return; }
    
};


//*     EXIT CLASS      *//
class Exit: public Actor {
public:
    Exit(StudentWorld* p, double startX, double startY);
    virtual void doSomething();
};


//*     PIT CLASS   *//
class Pit: public Actor {
public:
    Pit(StudentWorld* p, double startX, double startY);
    virtual void doSomething();
};


//*    GOODIE ABC CLASS     *//
class Goodie: public Actor {
public:
    Goodie(StudentWorld* p, int imageID, double startX, double startY);
    virtual void doSomething();
    virtual void doDifferent() = 0;
    virtual ~Goodie() { return; }
};

class Vaccine: public Goodie {
public:
    Vaccine(StudentWorld* p, double startX, double startY);
    virtual void doDifferent();
};

class GasCan: public Goodie {
public:
    GasCan(StudentWorld* p, double startX, double startY);
    virtual void doDifferent();
};

class LandMine: public Goodie {
public:
    LandMine(StudentWorld* p, double startX, double startY);
    virtual void doDifferent();
};

//*     PROJECTILE ABC CLASS    *//
class Projectile: public Actor {
public:
    Projectile(StudentWorld *p, int imageID, double startX, double startY, int startDirection);
    virtual void doSomething();
    virtual void doDifferent() = 0;
    virtual ~Projectile() { return; }
private:
    int m_ticks;
};

class Flame: public Projectile {
public:
    Flame(StudentWorld *p, double startX, int startY, int startDirection);
    virtual void doDifferent();
};

class Vomit: public Projectile {
public:
    Vomit(StudentWorld *p, double startX, int startY, int startDirection);
    virtual void doDifferent();
};

    //*     LANDMINE WEAPON CLASS    *//
class LandMineWep: public Actor {
public:
    LandMineWep(StudentWorld* p, double startX, double startY);
    bool Active() { return m_safeTicks <= 0;}
    virtual void doSomething();
private:
    int m_safeTicks;
    
};

//*     ZOMBIE ABC CLASS    *//
class Zombie: public Actor {
public:
    Zombie(StudentWorld* p, int imagID, double startX, double startY);
    virtual void doSomething();
    virtual void moveDifferent() = 0;
    int getDistPlan() { return m_distPlan; }
    void setDistPlan(int a) {m_distPlan = a; }
    virtual ~Zombie() {return;}
private:
    int m_distPlan;
    int m_paralyze;
};

class DumbZombie: public Zombie {
public:
    DumbZombie(StudentWorld* p, double startX, double startY);
    virtual void moveDifferent();
    virtual void Die();
};

class SmartZombie: public Zombie {
public:
    SmartZombie(StudentWorld* p, double startX, double startY);
    virtual void moveDifferent();
    virtual void Die();
};

//*     CITIZEN CLASS    *//
class Citizen: public Actor {
public:
    Citizen(StudentWorld* p, double startX, double startY);
    virtual void doSomething();
    virtual void Die();
    virtual void Exit();
private:
    int m_paralyze;
};
#endif // ACTOR_H_
