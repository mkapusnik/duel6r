# Main Menu Layout Design

This document schematically describes the main menu GUI layout built in [`source/Menu.cpp`](source/Menu.cpp).

The menu uses an 850 x 700 logical coordinate space, centered in the current window by [`Gui::Desktop::screenSize(...)`](source/gui/Desktop.cpp:48).
Coordinates below are approximate logical positions from the top-left corner of the menu canvas.

## Overall layout

```text
Logical menu canvas: 850 x 700

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
│ [ Score table listbox: x=10,y=199,w≈103 chars,h=12 rows, full width ]          │
│ ┌────────────────────────────────────────────────────────────────────────────┐ │
│ │ Name | Elo | Pts | Win | Kill | Assist | Pen | Death | K/D | Shot | ...  │ │
│ │ ...                                                                        │ │
│ └────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                │
│ Elo scoreboard    [Remove] [<<] [>>] [Add] [New person textbox]                │
│ ┌──────────────────┐                                                           │
│ │ eloListBox       │                                                           │
│ │ x=10,y=539       │                                                           │
│ │ 23 chars x15 rows│                                                           │
│ │                  │                                                           │
│ │                  │                                                           │
│ │                  │ Persons panel       Players panel   Controller  Settings  │
│ │                  │ ┌──────────────┐    ┌───────────┐   ┌────────┐ ┌────────┐ │
│ │                  │ │personListBox │    │players    │   │controls│ │Game    │ │
│ │                  │ │x=210,y=539   │    │x=370      │   │x=500   │ │Settings│ │
│ │                  │ └──────────────┘    └───────────┘   └────────┘ │GameMode│ │
│ │                  │ Persons              Players [E][S] Controller │Assist  │ │
│ │                  │                                             [D] │Liquid  │ │
│ │                  │                                                 │Rounds  │ │
│ └──────────────────┘                                                 └────────┘ │
└────────────────────────────────────────────────────────────────────────────────┘
Y=0  (bottom in gameplay coordinate system)
```

## Control groups

### Header controls

The top header keeps only direct action buttons. Game mode selection lives in the right-side [Game Settings column](#game-settings-column).

```text
x=350           x=505           x=660
┌────────────┐  ┌────────────┐  ┌────────────┐
│ Play (F1)  │  │ Clear (F3) │  │ Quit (ESC) │
└────────────┘  └────────────┘  └────────────┘
```

### Left Elo column

The Elo scoreboard is on the far left and now matches the height of the Persons list.

```text
x=10,y=560 label: Elo scoreboard

x=10,y=539 listbox:
┌───────────────────────┐
│ eloListBox            │
│ 23 chars x 15 rows    │
│ row height 18         │
│ Elo rank/name/score   │
│ Same height as        │
│ personListBox         │
│                       │
└───────────────────────┘
```

### Scoreboard area

```text
x=10,y=219 label:
"Name | Elo | Pts | Win | Kill | Assist | Pen | Death | K/D | Shot | Acc. | ..."

x=10,y=199 listbox:
┌──────────────────────────────────────────────────────────────────────────────┐
│ scoreListBox, 103 chars wide, 12 rows, row height 16                        │
│ Persistent person ranking/statistics spans the menu width above lower panels │
└──────────────────────────────────────────────────────────────────────────────┘
```

### Person/player setup area

```text
x=210                x=300       x=390       x=475       x=560
┌────────────┐       ┌──────┐    ┌──────┐    ┌─────┐     ┌──────────────┐
│ Remove     │       │ <<   │    │ >>   │    │ Add │     │ name textbox │
└────────────┘       └──────┘    └──────┘    └─────┘     └──────────────┘

x=210,y=539                x=370,y=541            x=500,y=539..287
┌──────────────────┐       ┌─────────────┐        ┌────────────┐
│ Persons list     │       │ Players list│        │ Controller │
│ available people │  ⇄    │ selected    │  ────▶ │ spinners   │
│ double-click adds│       │ players     │        │ + [D] btns │
└──────────────────┘       └─────────────┘        └────────────┘

x=210,y=560 label          x=370,y=560 label       x=500,y=560 label
Persons                    Players ### [E][S]      Controller [D]
                            E = Elo shuffle        D = detect all
                            S = random shuffle
```

### Game Settings column

The game settings remain grouped in a dedicated right-side vertical column.

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

x=594,y=452          label rendered by textbox on the right
┌──────┐             ┌───────────┐
│ 0000 │             │ Rounds    │
└──────┘             └───────────┘
```

Vertical order:
1. Game mode spinner
2. Assistance checkbox
3. Quick Liquid checkbox
4. Round-count textbox, aligned vertically with the checkboxes on the left, with the `Rounds` label rendered by the textbox to its right

Behavior:
- The game mode spinner selects the active ruleset before starting a game.
- `Assistance` toggles global assist handling before starting a game.
- `Quick Liquid` toggles quick liquid behavior before starting a game.
- `Rounds` is a numeric textbox bound to [`GameSettings::maxRounds`](source/GameSettings.h:55).
  - `0` means no round limit, matching the existing configuration behavior.
  - Positive values limit the match to that many rounds.
  - The value is applied when pressing Enter while the textbox is focused, and again immediately before starting a match through [`Menu::applyRoundsTextbox()`](source/Menu.cpp:552).

## Text input behavior

The menu currently has two editable textboxes:

```text
┌───────────────────────┐        ┌──────────────────────┐
│ New person name input │        │ Round count input    │
│ allowed: name chars   │        │ allowed: digits only │
└───────────────────────┘        └──────────────────────┘
```

Only the focused textbox receives text input. This avoids accidental cross-entry between the person-name field and the round-count field.
