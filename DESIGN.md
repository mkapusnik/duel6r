# Main Menu Layout Design

This document schematically describes the main menu GUI layout built in [`source/Menu.cpp`](source/Menu.cpp).

The menu uses an 800 x 700 logical coordinate space, centered in the current window by [`Gui::Desktop::screenSize(...)`](source/gui/Desktop.cpp:48).
Coordinates below are approximate logical positions from the top-left corner of the menu canvas.

## Overall layout

```text
Logical menu canvas: 800 x 700

Y=700  (top in gameplay coordinate system)
┌────────────────────────────────────────────────────────────────────────────────┐
│                                      [ Play (F1) ] [ Clear (F3) ] [ Quit ]     │
│                                                                                │
│                                                                                │
│                                                                                │
│                       [ menu banner texture centered near bottom ]              │
│                                                                                │
│                                                                                │
│                                                                                │
│                                                                                │
│ [ Score table listbox: x=10,y=199,w≈97 chars,h=12 rows ]                       │
│ ┌────────────────────────────────────────────────────────────────────────────┐ │
│ │ Name | Elo | Pts | Win | Kill | Assist | Pen | Death | K/D | Shot | ...  │ │
│ │ ...                                                                        │ │
│ └────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                │
│ [Remove] [<<] [>>] [Add] [New person textbox]                                  │
│                                                                                │
│                                                                                │
│                                                                                │
│                                                                                │
│ Persons panel             Players panel       Controller column   Settings      │
│ ┌──────────────────┐      ┌─────────────┐     ┌────────────────┐ ┌───────────┐ │
│ │ personListBox    │      │playerListBox│     │controlSwitches │ │Game       │ │
│ │ x=10,y=539       │      │x=200,y=541  │     │x=330,y=539...  │ │Settings   │ │
│ │ 20 chars x15 rows│      │15 chars     │     │+ detect D btns │ │Game Mode  │ │
│ └──────────────────┘      └─────────────┘     └────────────────┘ │[ ] Assist │ │
│ Persons                   Players ### [E][S]  Controller [D]     │[ ] Liquid │ │
│                                                                  │[0000] Rnds│ │
│                                                                  └───────────┘ │
│                                                                  Elo panel     │
│                                                                  ┌───────────┐ │
│                                                                  │eloListBox │ │
│                                                                  └───────────┘ │
└────────────────────────────────────────────────────────────────────────────────┘
Y=0  (bottom in gameplay coordinate system)
```

## Control groups

### Header controls

The top header keeps only direct action buttons. Game mode selection lives in the right-side [`Game Settings`](DESIGN.md#game-settings-column) column.

```text
x=350           x=505           x=660
┌────────────┐  ┌────────────┐  ┌────────────┐
│ Play (F1)  │  │ Clear (F3) │  │ Quit (ESC) │
│            │  │            │  │            │
└────────────┘  └────────────┘  └────────────┘
```

### Scoreboard area

```text
x=10,y=219 label:
"Name | Elo | Pts | Win | Kill | Assist | Pen | Death | K/D | Shot | Acc. | ..."

x=10,y=199 listbox:
┌──────────────────────────────────────────────────────────────────────────────┐
│ scoreListBox, 97 chars wide, 12 rows, row height 16                         │
│ Persistent person ranking/statistics                                        │
└──────────────────────────────────────────────────────────────────────────────┘
```

### Person/player setup area

```text
x=10                 x=105       x=200       x=284       x=370
┌────────────┐       ┌──────┐    ┌──────┐    ┌─────┐     ┌──────────────┐
│ Remove     │       │ <<   │    │ >>   │    │ Add │     │ name textbox │
└────────────┘       └──────┘    └──────┘    └─────┘     └──────────────┘

x=10,y=539                 x=200,y=541              x=330,y=539..287
┌────────────────────┐     ┌───────────────┐        ┌──────────────────────┐
│ Persons list       │     │ Players list  │        │ Controller spinners  │
│ available people   │ ⇄   │ selected team │  ────▶ │ one row per player   │
│ double-click adds  │     │ double-click  │        │ each row has [D]     │
│                    │     │ removes       │        │ detect button        │
└────────────────────┘     └───────────────┘        └──────────────────────┘

x=10,y=560 label           x=200,y=560 label         x=330,y=560 label
Persons                    Players ### [E][S]        Controller [D]
                            E = Elo shuffle          D = detect all
                            S = random shuffle
```

### Game Settings column

The game settings are grouped in a dedicated right-side vertical column above the Elo panel.

```text
x=594,y=560 label: Game Settings

x=594,y=532
┌────────────────────────┐
│ Game mode spinner      │
│ Deathmatch / Predator  │
│ Team modes             │
└────────────────────────┘

x=594,y=504
┌────────────────┐
│ [ ] Assistance │
└────────────────┘

x=594,y=476
┌──────────────────┐
│ [ ] Quick Liquid │
└──────────────────┘

x=594,y=452          x=626,y=449
┌──────┐             ┌───────────┐
│ 0000 │             │ Rounds    │
└──────┘             └───────────┘
```

Vertical order:
1. Game mode spinner
2. Assistance checkbox
3. Quick Liquid checkbox
4. Round-count textbox, aligned vertically with the checkboxes on the left, with the `Rounds` label to its right

Behavior:
- The game mode spinner selects the active ruleset before starting a game.
- `Assistance` toggles global assist handling before starting a game.
- `Quick Liquid` toggles quick liquid behavior before starting a game.
- `Rounds` is a numeric textbox bound to [`GameSettings::maxRounds`](source/GameSettings.h:55).
  - `0` means no round limit, matching the existing configuration behavior.
  - Positive values limit the match to that many rounds.
  - The value is applied when pressing Enter while the textbox is focused, and again immediately before starting a match through [`Menu::applyRoundsTextbox()`](source/Menu.cpp:552).

### Elo panel

The Elo panel is below the Game Settings column.

```text
x=594,y=424 label: Elo scoreboard

x=594,y=403 listbox:
┌────────────────────────┐
│ eloListBox             │
│ 24 chars x 10 rows     │
│ Elo rank/name/score    │
└────────────────────────┘
```

## Text input behavior

The menu currently has two editable textboxes:

```text
┌───────────────────────┐        ┌──────────────────────┐
│ New person name input │        │ Round count input    │
│ allowed: name chars   │        │ allowed: digits only │
└───────────────────────┘        └──────────────────────┘
```

Only the focused textbox receives text input. This avoids accidental cross-entry between the person-name field and the round-count field.
