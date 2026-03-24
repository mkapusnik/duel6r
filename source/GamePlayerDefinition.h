#ifndef DUEL6_GAMEPLAYERDEFINITION_H
#define DUEL6_GAMEPLAYERDEFINITION_H

#include "Person.h"
#include "PlayerSkinColors.h"
#include "PlayerSounds.h"
#include "input/PlayerControls.h"

namespace Duel6 {
    class GamePlayerDefinition {
    private:
        Person &person;
        PlayerSkinColors colors;
        const PlayerSounds &sounds;
        const PlayerControls &controls;

    public:
        GamePlayerDefinition(Person &person, const PlayerSkinColors &colors, const PlayerSounds &sounds,
                             const PlayerControls &controls)
                : person(person), colors(colors), sounds(sounds), controls(controls) {}

        Person &getPerson() const {
            return person;
        }

        PlayerSkinColors &getColors() {
            return colors;
        }

        const PlayerSkinColors &getColors() const {
            return colors;
        }

        const PlayerSounds &getSounds() const {
            return sounds;
        }

        const PlayerControls &getControls() const {
            return controls;
        }
    };
}

#endif
