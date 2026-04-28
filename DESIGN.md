# Main Menu Layout Design

This document schematically describes the main menu GUI layout built in `source/Menu.cpp`.

The menu uses an 800 x 700 logical coordinate space, centered in the current window by `Gui::Desktop::screenSize(...)`.
Coordinates below are approximate logical positions from the top-left corner of the menu canvas.

## Overall layout

```text
Logical menu canvas: 800 x 700

Y=700  (top in gameplay coordinate system)
┌────────────────────────────────────────────────────────────────────────────────┐
│ [Game mode spinner: x=10,w=330]  [ Play (F1) ] [ Clear (F3) ] [ Quit (ESC) ]  │
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
│ Persons panel             Players panel       Controller column     Elo panel   │
│ ┌──────────────────┐      ┌─────────────┐     ┌────────────────┐   ┌─────────┐ │
│ │ personListBox    │      │playerListBox│     │controlSwitches │   │eloList  │ │
│ │ x=10,y=539       │      │x=200,y=541  │     │x=330,y=539...  │   │x=594... │ │
│ │ 20 chars x15 rows│      │15 chars     │     │+ detect D btns │   │24 chars │ │
│ └──────────────────┘      └─────────────┘     └────────────────┘   └─────────┘ │
│ Persons                   Players ### [E][S]  Controller [D]      Elo scoreboard│
│                                                                                │
│ [Assistance checkbox] [Quick Liquid checkbox] [Rounds label] [Rounds textbox]  │
└────────────────────────────────────────────────────────────────────────────────┘
Y=0  (bottom in gameplay coordinate system)
```

## Control groups

### Header controls

```text
x=10                         x=350           x=505           x=660
┌──────────────────────────┐  ┌────────────┐  ┌────────────┐  ┌────────────┐
│ Game mode spinner        │  │ Play (F1)  │  │ Clear (F3) │  │ Quit (ESC) │
│ Deathmatch / Predator /  │  │            │  │            │  │            │
│ Team modes               │  │            │  │            │  │            │
└──────────────────────────┘  └────────────┘  └────────────┘  └────────────┘
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

### Elo panel

```text
x=594,y=560 label: Elo scoreboard

x=594,y=539 listbox:
┌────────────────────────┐
│ eloListBox             │
│ 24 chars x 19 rows     │
│ Elo rank/name/score    │
└────────────────────────┘
```

### Bottom settings row

```text
x=11                  x=151                    x=330              x=417
┌──────────────────┐  ┌────────────────────┐   ┌────────────┐     ┌──────┐
│ [ ] Assistance   │  │ [ ] Quick Liquid   │   │ Rounds     │     │ 0000 │
└──────────────────┘  └────────────────────┘   └────────────┘     └──────┘
```

- `Assistance` toggles global assist handling before starting a game.
- `Quick Liquid` toggles quick liquid behavior before starting a game.
- `Rounds` is a numeric textbox bound to `GameSettings::maxRounds`.
  - `0` means no round limit, matching the existing configuration behavior.
  - Positive values limit the match to that many rounds.
  - The value is applied when pressing Enter while the textbox is focused, and again immediately before starting a match.

## Text input behavior

The menu currently has two editable textboxes:

```text
┌───────────────────────┐        ┌──────────────────────┐
│ New person name input │        │ Round count input    │
│ allowed: name chars   │        │ allowed: digits only │
└───────────────────────┘        └──────────────────────┘
```

Only the focused textbox receives text input. This avoids accidental cross-entry between the person-name field and the round-count field.
