#pragma once
#ifndef MINIWARS_H
#define MINIWARS_H

#ifdef FEOS
#include <feos.h>
#elif defined(ARM9)
#include <nds.h>
#endif

#include "atan.h"
#include "console.h"
#include "list.h"
#include "particle.h"
#include "weapon.h"
#include "mortar.h"
#include "plasma.h"
#include "shotgun.h"
#include "character.h"
#include "player.h"
#include "enemy.h"

#endif /* MINIWARS_H */

