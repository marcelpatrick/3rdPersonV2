# 3rdPersonV2

# Project Steps:
1. Create Components (Parent Classes)
2. Create Sub-Components (Child Classes: Tank, Tower, Projectile)
3. User Input (Moving the Actor, Firing)
4. Actions and Events (Hit Events, Health Component, Apply Damage)
5. Game Rules, Game Mode and Game Controller (Game Cycle: Start > Death > Winning, Loosing > End)
6. Special Effects (Sounds, Particles)


# 1: Create Components

## 1.1: Main Character C++ class
Create a new C++ class "ShooterCharacter" inheriting from the Character class that will be the base class for both our player and our enemies.
On File click on Refresh Virtual Studio Code project.

## 1.2: Main Character Blueprints

In Unreal, select the c++ class and click on Create a new blueprint based on "this class", name it "BP_ShooterCharacter".
Inside the BP, View Port > Details > Static Mesh: select which static mesh you are going to use to physically represent your player in the world.
Adjust the camera and the spring arm, camera and projectile spawn point position.
In BP, View Port > Details > Rendering > disable player hidden in game.

## 1.3: Declare Component Variables:
In its header file initialize and expose the variables that will correspond to each of the components that we will use in the world.

```cpp
 
```

## 1.4: Construct Component Objects:
In the cpp file, Construct these components so that they are able to ve visible in our world

```cpp
 
```

## 1.5: Create our own GameMode BP class

In Unreal, Create a new BP class inheriting from GameMode Base: BP_ShooterGameMode
Inside the BP, Details, Classes, Default Pawn Class, select our main character BP.
In Unreal, Blueprints, GameMode, Select GameMode base class, select our BP_ShooterGameMode class.

