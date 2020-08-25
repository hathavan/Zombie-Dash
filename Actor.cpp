#include "Actor.h"
#include "GameConstants.h"
#include "StudentWorld.h"


Actor::Actor(StudentWorld* p, int imageID, double startX, double startY, int startDirection = right, int depth = 0)
:GraphObject(imageID, startX, startY, startDirection, depth), myWorld(p), obstructType(false), exitType(false), m_alive(true), damagevomit(false), m_infectNum(0), m_infected(false), m_explode(false), m_isExplodable(false)
{
}

//*     Penelope implementations    *//
Penelope::Penelope(StudentWorld* p, double startX, double startY)
:Actor(p, IID_PLAYER, startX, startY), m_mines(0), m_flames(0), m_vaccines(0)
{
        // Set all characteristics of Penelope in accordance to Manual
    setObstruct(true);
    setDamageFlame(true);
    setBlockFlame(false);
    setDamageVomit(true);
    setExplodable(true);
}

void Penelope::doSomething() {
        // Increment if Penelope is infected
    if (IsInfected())
        IncInfected();
        // Kill Penelope once at 500 ticks
    if (infectedNum() == 500) {
        getWorld()->setLevelDied(true);
        return;
    }
    int ch;
        // getWorld() returns pointer to StudentWorld that objects reside in
    if (getWorld()->getKey(ch)) {
        switch (ch) {
            case KEY_PRESS_UP:
                    // Arrow Keys
                setDirection(up);
                if (getWorld()->ObjectObstructs(getX(), getY() + 4, this))
                    break;
                    // moveTo() is a subset of GraphObject.h
                moveTo(getX(), getY() + 4);
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                if (getWorld()->ObjectObstructs(getX(), getY() - 4,this))
                    break;
                moveTo(getX(), getY() - 4);
                break;
            case KEY_PRESS_LEFT:
                setDirection(left);
                if (getWorld()->ObjectObstructs(getX() - 4, getY(), this))
                    break;
                moveTo(getX() - 4, getY());
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                if (getWorld()->ObjectObstructs(getX() + 4, getY(), this))
                    break;
                moveTo(getX() + 4, getY());
                break;
            case KEY_PRESS_SPACE:
                if (m_flames <= 0)
                    break;
                m_flames--;
                    // 1 - 4 as 0 - 3 would've killed Penelope
                getWorld()->playSound(SOUND_PLAYER_FIRE);
                for (int i = 1; i < 4; i++) {
                    if (getDirection() == up) {
                        if (!getWorld()->AddFire(getX(), getY() + (i * SPRITE_HEIGHT), getDirection()))
                            break;
                    }
                    else if (getDirection() == down) {
                        if(!getWorld()->AddFire(getX(), getY() - (i * SPRITE_HEIGHT), getDirection()))
                            break;
                    }
                    else if (getDirection() == left) {
                        if(!getWorld()->AddFire(getX() - (i * SPRITE_HEIGHT), getY() , getDirection()))
                            break;
                    }
                    else if (getDirection() == right) {
                        if(!getWorld()->AddFire(getX() + (i * SPRITE_HEIGHT), getY() , getDirection()))
                            break;
                    }
                }
                break;
            case KEY_PRESS_ENTER:
                    // Check to make sure Penelepe has vaccines
                if (m_vaccines <= 0)
                    break;
                m_vaccines--;
                SetInfected(false);
                ClearInfected();
                break;
            case KEY_PRESS_TAB:
                    // Check to make sure Penelope has mines
                if (m_mines <= 0)
                    break;
                m_mines--;
                    // ONLY STUDENTWORLD HAS ACCESS TO CREATION OF OBJECTS
                getWorld()->LandMineAt(getX(), getY());
                break;
        }
    }
}

//*     Wall implementations   *//
Wall::Wall(StudentWorld* p, double startX, double startY)
:Actor(p, IID_WALL, startX, startY)
{
    setObstruct(true);
    setDamageVomit(false);
    setDamageFlame(false);
    setBlockFlame(true);
}

//*     Exit implementations    *//
Exit::Exit(StudentWorld* p, double startX, double startY)
:Actor(p, IID_EXIT, startX, startY, right, 1)
{
    setObstruct(false);
    setDamageVomit(false);
    setDamageFlame(false);
    setBlockFlame(true);
}

void Exit::doSomething() {
    // Outsource bulk of the work to studentWorld as it can't return pointers, vectors etc.
    getWorld()->ExitElse(getX(), getY());
}

//* NOTE: EXIT & PIT COULD'VE BEEN DERIVED CLASSES OF A MORE GENERALIZED BASE CLASS
//        BUT I HAD NO TIME IMPLEMENT SUCH A CLASS (The doSomethings() are very similar for either class

//*     Pit implementations    *//
Pit::Pit(StudentWorld* p, double startX, double startY)
:Actor(p, IID_PIT, startX, startY)
{
    setObstruct(false);
    setDamageVomit(false);
    setDamageFlame(false);
    setBlockFlame(false);
}

void Pit::doSomething() {
        // Outsource bulk of the work to studentWorld as it can't return pointers, vectors etc.
    getWorld()->PitElse(getX(), getY());
}

//*     Goodie implementations  *//
Goodie::Goodie(StudentWorld* p, int imageID, double startX, double startY)
:Actor(p,imageID, startX, startY, right, 1)
{
        // Implemented per last set of bullet points from manual
    setAliveStatus(true);
    setDamageFlame(true);
    setBlockFlame(false);
    setDamageVomit(false);
    setObstruct(false);
}

void Goodie::doSomething() {
    if (aliveStatus()) {
        if (getWorld()->PenelopeGoodie(getX(), getY())) {
                // EACH GOODIE DOES SOMETHING DIFFERENT!!
            doDifferent();
            setAliveStatus(false);
        }
    }
}

Vaccine::Vaccine(StudentWorld* p, double startX, double startY)
:Goodie(p, IID_VACCINE_GOODIE, startX, startY)
{
}

void Vaccine::doDifferent() {
        // Vaccines increments by 1
    getWorld()->VaccinesInc();
}

GasCan::GasCan(StudentWorld* p, double startX, double startY)
:Goodie(p, IID_GAS_CAN_GOODIE, startX, startY)
{
}

void GasCan::doDifferent() {
        // Flames increment by 5
    getWorld()->FlamesInc();
}

LandMine::LandMine(StudentWorld* p, double startX, double startY)
:Goodie(p, IID_LANDMINE_GOODIE, startX, startY)
{
}

void LandMine::doDifferent() {
        // Mines increment by 2
    getWorld()->MinesInc();
}

//*     PROJECTILE IMPLEMENTATIONS    *//
Projectile::Projectile(StudentWorld *p, int imageID, double startX, double startY, int startDirection)
:Actor(p, imageID, startX, startY, startDirection), m_ticks(0)
{
    setDamageVomit(false);
    setDamageFlame(false);
    setObstruct(false);
}

void Projectile::doSomething() {
        // Flames & Vomit only last 2 ticks
    m_ticks++;
    if (m_ticks > 2) {
        setAliveStatus(false);
    }
    if (aliveStatus()) {
            // Flames & Vomit do different things
        doDifferent();
    }
}

Flame::Flame(StudentWorld *p, double startX, int startY, int startDirection)
:Projectile(p, IID_FLAME, startX, startY, startDirection)
{
}

void Flame::doDifferent() {
    getWorld()->flameAll(getX(), getY());
}

// NOTE: I notice the similarity between flameAll & vomitAll but was unable to figure out how to combine them
Vomit::Vomit(StudentWorld *p, double startX, int startY, int startDirection)
:Projectile(p, IID_VOMIT, startX, startY, startDirection)
{
}

void Vomit::doDifferent() {
    getWorld()->vomitAll(getX(), getY());
}

//*     LANDMINE IMPLEMENTATION       *//
LandMineWep::LandMineWep(StudentWorld* p, double startX, double startY)
:Actor(p, IID_LANDMINE, startX, startY, right, 1), m_safeTicks(30)
{
    setDamageFlame(true);
    setBlockFlame(false);
    setDamageVomit(false);
    setObstruct(false);
    setExplode(true);
}

void LandMineWep::doSomething() {
    if (!aliveStatus())
        return;
        // Landmines have 30 safe ticks
    if (!Active()) {
        m_safeTicks--;
        if (!Active())
            return;
    }
    if (getWorld()->landmineALL(getX(), getY()))
        setAliveStatus(false);
}

//*     ZOMBIE IMPLEMENTATIONS    *//
Zombie::Zombie(StudentWorld* p, int imageID,  double startX, double startY)
:Actor(p, imageID, startX, startY, right, 0), m_distPlan(0), m_paralyze(0)
{
        // Similar to Penelope except it can't be damaged by vomit
    setObstruct(true);
    setExplodable(true);
    setExplode(false);
    setCanExit(false);
    setDamageFlame(true);
    setBlockFlame(false);
    setDamageVomit(false);
}

void Zombie::doSomething() {
        // ALL Zombies are paralyzed every 2 ticks
    m_paralyze++;
    if (!aliveStatus() || (m_paralyze % 2 == 0) )
        return;
        // There's 1 in 3 chance that a zombie will choose to vomit on a human
    int num = randInt(1, 3);
    switch (getDirection()) {
        case up:
            if (getWorld()->vomitable(getX(), getY() + 16) && num == 2)
                getWorld()->vomitAt(getX(), getY() + 16, up);
            break;
        case down:
            if (getWorld()->vomitable(getX(), getY() - 16) && num ==2)
                getWorld()->vomitAt(getX(), getY() - 16, up);
            break;
        case left:
            if (getWorld()->vomitable(getX() - 16, getY()) && num == 2)
                getWorld()->vomitAt(getX() - 16, getY(), up);
            break;
        case right:
            if (getWorld()->vomitable(getX() + 16, getY()) && num == 2)
                getWorld()->vomitAt(getX() + 16, getY(), up);
            break;
    }
        // Smart Zombies move differently than Dumb Zombies
    moveDifferent();
        // moveDifferent() sets the direction to proceed in next
    switch (getDirection()) {
        case up:
            if (getWorld()->ObjectObstructs(getX(), getY() + 1,this)) {
                m_distPlan = 0;
                break;
            } else {
                moveTo(getX(), getY() + 1);
                m_distPlan--;
            }
            break;
        case down:
            if (getWorld()->ObjectObstructs(getX(), getY() - 1, this)) {
                m_distPlan = 0;
                break;
            } else {
                moveTo(getX(), getY() - 1);
                m_distPlan--;
            }
            break;
        case right:
            if (getWorld()->ObjectObstructs(getX() + 1, getY(),this)) {
                m_distPlan = 0;
                break;
            } else {
                moveTo(getX() + 1, getY());
                m_distPlan--;
            }
            break;
        case left:
            if (getWorld()->ObjectObstructs(getX() - 1, getY(), this)) {
                m_distPlan = 0;
                break;
            } else {
                moveTo(getX() - 1, getY());
                m_distPlan--;
            }
            break;
    }
}

DumbZombie::DumbZombie(StudentWorld* p, double startX, double startY)
:Zombie(p, IID_ZOMBIE, startX, startY)
{
}

    // Dumb Zombies just move randomly without running into obstructions
void DumbZombie::moveDifferent(){
    if (getDistPlan() != 0)
        return;
    else if (getDistPlan() == 0 ) {
        int num = randInt(3, 10);
        setDistPlan(num);
        switch (randInt(1, 4)) {
            case 1:
                setDirection(up);
                break;
            case 2:
                setDirection(down);
                break;
            case 3:
                setDirection(left);
                break;
            case 4:
                setDirection(right);
                break;
        }
    }
}

void DumbZombie::Die() {
    setAliveStatus(false);
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
    getWorld()->increaseScore(1000);
        // Dumb Zombies fling vaccines out in a random direction
    if (randInt(1, 10) == 5) {
        switch (randInt(1, 4)) {
            case 1:
                    // Pass in this to make sure it doesn't consider itself an obstruction
                if (!getWorld()->ObjectObstructs(getX(), getY() + SPRITE_HEIGHT, this))
                    getWorld()->DumbZombieVaccine(getX(), getY() + SPRITE_HEIGHT);
                break;
            case 2:
                if (!getWorld()->ObjectObstructs(getX(), getY() - SPRITE_HEIGHT, this))
                    getWorld()->DumbZombieVaccine(getX(), getY() - SPRITE_HEIGHT);
                break;
            case 3:
                if (!getWorld()->ObjectObstructs(getX() + SPRITE_WIDTH, getY(), this))
                    getWorld()->DumbZombieVaccine(getX() + SPRITE_WIDTH, getY());
                break;
            case 4:
                if (!getWorld()->ObjectObstructs(getX() - SPRITE_WIDTH, getY(), this))
                    getWorld()->DumbZombieVaccine(getX() - SPRITE_WIDTH, getY());
                break;
        }
    }
}

SmartZombie::SmartZombie(StudentWorld* p, double startX, double startY)
:Zombie(p, IID_ZOMBIE, startX, startY)
{
}

void SmartZombie::moveDifferent() {
    if (getDistPlan() != 0)
        return;
    else if (getDistPlan() == 0) {
        setDistPlan(randInt(3, 10));
            // Outsource to studentworld as you need to consider citizens & penelope
        Direction dir = getWorld()->SmartZombiePlan(getX(), getY());
        setDirection(dir);
        return;
    }
}

void SmartZombie::Die() {
    setAliveStatus(false);
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
    getWorld()->increaseScore(2000);
}
    // Identical to Penelope initializations
    // NOTE: no time but could've put Penelope & Citizen under common 'Human' base class
Citizen::Citizen(StudentWorld* p, double startX, double startY)
:Actor(p,IID_CITIZEN, startX, startY, right, 0), m_paralyze(0)
{
    setObstruct(true);
    setDamageFlame(true);
    setBlockFlame(false);
    setDamageVomit(true);
    setExplodable(true);
    setCanExit(true);
}

void Citizen::doSomething() {
    if (!aliveStatus())
        return;
    if (IsInfected())
        IncInfected();
        // Check for infection and act accordingly
    if (infectedNum() == 500) {
        setAliveStatus(false);
        getWorld()->playSound(SOUND_ZOMBIE_BORN);
        getWorld()->increaseScore(-1000);
        getWorld()->Zombiefied(getX(), getY());
        getWorld()->DecCitizens();
        return;
    }
    m_paralyze++;
        // Paralyzed every even tick (from instanstiation)
    if ((m_paralyze % 2) == 0)
        return;
    getWorld()->CitizenPlan(getX(), getY(), this);
}

    // Citizens disappear by either dying or exiting the level
void Citizen::Die() {
    setAliveStatus(false);
    getWorld()->playSound(SOUND_CITIZEN_DIE);
    getWorld()->DecCitizens();
    getWorld()->increaseScore(-1000);
}

void Citizen::Exit() {
    setAliveStatus(false);
    getWorld()->playSound(SOUND_CITIZEN_SAVED);
    getWorld()->DecCitizens();
    getWorld()->increaseScore(500);
}
