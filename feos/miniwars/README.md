# Overview

This is the home of a 2d top-down, pixel-sized, fps.

# Controls

The player is moved with the control arrows. Aiming and shooting is done with the touch screen.

# Weapons

Various weapons are available:

## knife

not a real weapon per se. It is always available, and used automatically when the player is 1px next to an opponent. You still need to move *towards* the opponent to slay. 

## pitiful plasma

That is the default gun. Has a slow firing rate but quite good accuracy when moving. Accuracy goes to perfect on standing still. 

Upgrades increase firing rate.

## gatling plasma

This is the improved version of the pitiful one. 
Fast rate. Poor accuracy. 

Upgrades increase rate.

## shotgun

This is not a particle gun. Hitting is instant (ray casting).
Sends many shots in wide spread. Each shot is small, but their sum is deadly.

## machine gun

Another non particle gun. Same small shots, but very fast rate.
Long rifles decrease accuracy. 
Accuracy is perfect for the first shot when still.

Upgrade increase accuracy.

## rpg

Slow moving particle. Splash damage when hit. Very low firing rate.

Upgrade increase rate.

## bomb

Same as rpg, but stops at destination, and explodes after a while.
Ultra low firing rate.

Upgrade increse rate.

# Graphics

Each item is one colored pixel. Plasma and explosions are done via pixel sized particle effects.

Cyclings palettes could be used to make plasma and explosion glowy.

# Game mechanics

Aming is fuzzy when moving. Stopping gradually increase pecision. Moving also has inertia. 

## Weapon upgrades

There is a ribbon with the weapons.
Each kill moves to next weapon slot. You can activate it if you want. Activation resets the ribbon slot. Successive activation increase power/rate/range.

