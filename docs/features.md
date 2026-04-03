**# Features

## Match Setup and Session Flow

- The game must support local multiplayer sessions with a minimum of 2 players and a maximum of 15 simultaneous players.
- The main menu must let players build a roster from persistent people records, assign control schemes, add or remove active players, shuffle player order, clear stored stats, and start or exit a session.
- The setup flow must expose selectable rulesets for free-for-all play, Predator, and team-based variants, along with toggles that affect assist handling and sudden-death water behavior.
- Session state must persist across multiple rounds so that wins, kills, deaths, assists, penalties, and match progress accumulate until the session ends.

## Player Controls and Core Actions

- The game must support multiple predefined keyboard layouts plus SDL-style gamepad input, all mapped to the abstract actions move left, move right, jump, crouch, shoot, pick or swap weapon, and show status.
- During play, each player must be able to walk, jump, crouch, double-jump, shoot, pick up dropped weapons, swap away from a current weapon, and request an on-screen status display.
- Newly spawned players must receive brief spawn protection and a temporary location indicator so they can reorient after entering the round.
- Matches with fewer than five players must support toggling between a full-arena presentation and split-screen player views.

## World, Maps, and Environmental Hazards

- Arenas must be data-driven and selected from a shipped library of JSON-authored maps; the current repository includes 28 bundled maps.
- Levels must support solid terrain, water, waterfalls, moving elevators or platforms, decorative sprites, and map-specific layouts that influence combat routes and safe zones.
- Water must act as an environmental hazard by draining air, then life, with multiple water variants that differ in severity.
- Sudden death must be represented by rising water that pressures the remaining players when a round reaches a late-game state or when the relevant setup toggle is enabled.

## Combat, Weapons, and Pickups

- The game must provide a roster of distinct weapons with different reload cadence, ammo behavior, damage output, and special traits; the current code defines 17 weapons, with configuration deciding which are enabled by default.
- Weapon behaviors must remain visibly differentiated at a high level, including chargeable weapons, splash-damage weapons, rapid-fire weapons, beam-style weapons, and status-effect or novelty weapons.
- Players must begin rounds with a random enabled weapon and must drop their current weapon on death so that other players can claim it.
- The game must spawn both temporary bonuses and dropped-weapon pickups into the world during live rounds.

## Bonus Effects and Entity Interactions

- The game must support instant pickups such as extra life, reduced life, full heal, and additional bullets, plus timed effects such as faster reload, faster movement, powerful shots, invulnerability, invisibility, splitfire, vampire shots, infinite ammo, and snorkel.
- Bonus effects must alter how players interact with the world and with other combatants, for example by changing movement speed, survivability, underwater behavior, projectile output, or health recovery.
- Player-versus-player interaction must include direct damage, splash damage, weapon theft through pickups after death, assists based on contribution, and penalties for self-destructive or team-harming actions when rules require it.
- Player-versus-environment interaction must include drowning, movement penalties in water, platform riding, and survival pressure from escalating hazards.

## Game Modes, Winning Conditions, and Scoring

- Deathmatch must reward the sole surviving player when a round ends.
- Predator must designate one player as the predator and change the balance of visibility, survivability, and win conditions between the predator and the remaining players.
- Team modes must support 2-team, 3-team, and 4-team variants, each with friendly fire enabled or disabled, while keeping team identity visually readable.
- The scoring model must persist wins, kills, assists, penalties, deaths, shots fired, accuracy, damage, points, and Elo-style ranking data, and it must visibly discourage suicide, drowning, and invalid kills through negative scoring.
- The UI must surface both live rankings during play and fuller score summaries at round end or game end.

## Profiles, Customization, and Persistence

- The game must support name-based player profiles stored on disk, with optional skin colors, cosmetic choices, event sounds, and Lua behavior hooks.
- If a matching profile is absent, the game must still generate a valid playable participant using default sounds and randomized visual customization.
- Persistent people records must retain long-term statistics between launches and feed the menu roster on the next startup.
- Team modes must be allowed to override part of a player's look so that team color remains identifiable even when profiles are customized.

## Console, Configuration, and Scripting

- The game must provide an in-game console that is available from both menu and gameplay contexts for runtime inspection and live configuration.
- Startup configuration must be file-driven so that default audio, weapon availability, and other session-affecting settings can be changed without recompiling.
- Console commands must be able to alter runtime behavior such as renderer diagnostics, FPS display, music and volume, round count, weapon enablement, map selection, ghosting, and controller rescans.
- Lua profile scripts must be able to inspect match context at a high level, observe the controlled player, other players, the level, and active shots, and influence play through input-like actions rather than unrestricted world editing.**
