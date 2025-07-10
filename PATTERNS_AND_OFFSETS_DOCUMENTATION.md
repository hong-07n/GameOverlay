# GameOverlay Patterns and Data Offsets Documentation

This document provides a comprehensive list of all patterns and data offsets used in the GameOverlay repository for Path of Exile game memory scanning and data extraction.

## Table of Contents
1. [Overview](#overview)
2. [Pattern Scanning System](#pattern-scanning-system)
3. [Available Patterns](#available-patterns)
4. [Data Offset Structures](#data-offset-structures)
5. [How to Find New Patterns](#how-to-find-new-patterns)
6. [How to Use Offsets](#how-to-use-offsets)
7. [Examples](#examples)

## Overview

The GameOverlay system uses **pattern scanning** (also known as signature scanning) to find dynamic addresses in the Path of Exile game process. This technique allows the overlay to locate important game data structures even when the game is updated, as long as the byte patterns remain consistent.

The system consists of:
- **Patterns**: Byte sequences used to find static addresses in memory
- **Data Offsets**: Structure definitions that describe how game data is laid out in memory
- **PatternFinder**: The engine that scans memory for patterns
- **RemoteObjects**: Classes that use the found addresses and offsets to read game data

## Pattern Scanning System

### How Pattern Scanning Works

1. **Pattern Definition**: Patterns are defined as hex byte sequences with wildcards
2. **Memory Scanning**: The PatternFinder scans the game process memory for these patterns
3. **Address Calculation**: When found, the pattern location + offset gives a static address
4. **Data Access**: The static address is used to read game data structures

### Pattern Format

Patterns use the following format:
- **Hex bytes**: `48`, `8B`, `F1` (actual bytes to match)
- **Wildcards**: `??` or `?` (don't care what byte is here)
- **Skip marker**: `^` (indicates how many bytes to skip to reach the data)
- **Separators**: Spaces or commas between bytes

Example: `"48 83 EC ?? 48 8B F1 33 ED 48 39 2D ^ ?? ?? ?? ??"`

## Available Patterns

The following patterns are defined in `StaticOffsetsPatterns.cs`:

### 1. Game States
**Pattern**: `"48 83 EC ?? 48 8B F1 33 ED 48 39 2D ^ ?? ?? ?? ??"`
**Purpose**: Locates the main game state structure containing all game states
**Usage**: Access current game state (login, character select, in-game, etc.)
**Offset Structure**: `GameStateOffsets`

### 2. File Root
**Pattern**: `"48 ?? ?? ^ ?? ?? ?? ?? 41 ?? ?? ?? 39 ?? ?? ?? ?? ?? 0F 8E"`
**Purpose**: Finds the root of the file loading system
**Usage**: Access loaded game files and detect when files are loaded/unloaded
**Offset Structure**: `LoadedFilesOffset`

### 3. AreaChangeCounter
**Pattern**: `"FF ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? FF 05 ^ ?? ?? ?? ?? ?? 8D ?? ?? ?? 8B ??"`
**Purpose**: Locates a counter that increments when changing areas
**Usage**: Detect when player moves between different game areas
**Offset Structure**: `AreaChangeOffset`

### 4. GameWindowScaleValues
**Pattern**: `"C7 ?? 00 00 80 3F C7 ?? 04 00 00 80 3F C3 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? C3 ?? ?? ?? ?? ^ ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? C3 ?? ?? ?? ??"`
**Purpose**: Finds UI scaling values for the game window
**Usage**: Get current UI scale settings for proper overlay positioning
**Offset Structure**: Custom float values

### 5. Terrain Rotator Helper
**Pattern**: `"4c ?? ?? ?? 48 ?? ?? ^ ?? ?? ?? ?? 4c ?? ?? 8b ?? 2b ?? 33 ??"`
**Purpose**: Helper for terrain rotation calculations
**Usage**: Access terrain rotation data for map positioning
**Offset Structure**: Custom data structure

### 6. Terrain Rotation Selector
**Pattern**: `"^ ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 4c ?? ?? ?? 48 ?? ?? ?? ?? ?? ?? 4c ?? ?? 8b ?? 2b ?? 33 ??"`
**Purpose**: Selector for terrain rotation data
**Usage**: Additional terrain rotation information
**Offset Structure**: Custom data structure

## Data Offset Structures

### Core Game Structures

#### GameStateOffsets
Located in: `GameOffsets/Objects/GameStateOffsets.cs`
```csharp
[StructLayout(LayoutKind.Explicit, Pack = 1)]
public struct GameStateOffset
{
    [FieldOffset(0x08)] public StdVector CurrentStatePtr;
    [FieldOffset(0x48)] public IntPtr State0;    // Login State
    [FieldOffset(0x58)] public IntPtr State1;    // Character Select
    [FieldOffset(0x68)] public IntPtr State2;    // In Game State
    [FieldOffset(0x78)] public IntPtr State3;
    // ... up to State11
}
```
**Purpose**: Access different game states and current active state

#### InGameStateOffset
Located in: `GameOffsets/Objects/States/InGameStateOffset.cs`
```csharp
[StructLayout(LayoutKind.Explicit, Pack = 1)]
public struct InGameStateOffset
{
    [FieldOffset(0x18)] public IntPtr AreaInstanceData;
    [FieldOffset(0x78)] public IntPtr WorldData;
    [FieldOffset(0x1A0)] public IntPtr UiRootPtr;
    [FieldOffset(0x438)] public IntPtr IngameUi;
}
```
**Purpose**: Access in-game data when player is actively playing

#### AreaChangeOffset
Located in: `GameOffsets/Objects/AreaChangeOffset.cs`
```csharp
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct AreaChangeOffset
{
    public int counter;
}
```
**Purpose**: Simple counter that increments when changing areas

### Entity System

#### EntityOffsets
Located in: `GameOffsets/Objects/States/InGameState/EntityOffsets.cs`
```csharp
[StructLayout(LayoutKind.Explicit, Pack = 1)]
public struct EntityOffsets
{
    [FieldOffset(0x00)] public ItemStruct ItemBase;
    [FieldOffset(0x30)] public StdVector UnknownListPtr;
    [FieldOffset(0x60)] public uint Id;
    [FieldOffset(0x70)] public byte IsValid; // 0x0C = Valid, 0x03 = Invalid
}
```
**Purpose**: Base structure for all entities (players, monsters, items, NPCs)

#### EntityDetails
```csharp
[StructLayout(LayoutKind.Explicit, Pack = 1)]
public struct EntityDetails
{
    [FieldOffset(0x08)] public StdWString name;
    [FieldOffset(0x30)] public IntPtr ComponentLookUpPtr;
}
```
**Purpose**: Entity name and component lookup information

### Component System

#### LifeOffset
Located in: `GameOffsets/Objects/Components/Life.cs`
```csharp
[StructLayout(LayoutKind.Explicit, Pack = 1)]
public struct LifeOffset
{
    [FieldOffset(0x000)] public ComponentHeader Header;
    [FieldOffset(0x190)] public VitalStruct Mana;
    [FieldOffset(0x1C8)] public VitalStruct EnergyShield;
    [FieldOffset(0x228)] public VitalStruct Health;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct VitalStruct
{
    public IntPtr PtrToLifeComponent;
    public float Regeneration;      // > 0 if regenerating
    public int Total;               // Maximum value
    public int Current;             // Current value
    public int ReservedFlat;        // Flat reservation (e.g., Clarity)
    public int ReservedPercent;     // Percentage reservation (e.g., Heralds)
    public int ReservedTotal => (int)Math.Ceiling(this.ReservedPercent / 10000f * this.Total) + this.ReservedFlat;
    public int Unreserved => this.Total - this.ReservedTotal;
}
```
**Purpose**: Access health, mana, and energy shield information

#### PlayerOffsets
Located in: `GameOffsets/Objects/Components/Player.cs`
```csharp
[StructLayout(LayoutKind.Explicit, Pack = 1)]
public struct PlayerOffsets
{
    [FieldOffset(0x000)] public ComponentHeader Header;
    [FieldOffset(0x160)] public StdWString Name;
}
```
**Purpose**: Player-specific information like character name

#### RenderOffsets
Located in: `GameOffsets/Objects/Components/Render.cs`
```csharp
[StructLayout(LayoutKind.Explicit, Pack = 1)]
public struct RenderOffsets
{
    [FieldOffset(0x0000)] public ComponentHeader Header;
    [FieldOffset(0x7C)] public StdTuple3D<float> CurrentWorldPosition;
    [FieldOffset(0x88)] public StdTuple3D<float> CharactorModelBounds;
    [FieldOffset(0xE8)] public float TerrainHeight;
}
```
**Purpose**: Entity position and rendering information

#### ActorOffset
Located in: `GameOffsets/Objects/Components/Actor.cs`
```csharp
[StructLayout(LayoutKind.Explicit, Pack = 1)]
public struct ActorOffset
{
    [FieldOffset(0x234)] public int AnimationId;
}
```
**Purpose**: Animation and actor-specific data

### Additional Components
- **Buffs**: Status effects and auras
- **Charges**: Skill charges (endurance, frenzy, power)
- **Chest**: Container and lootable object data
- **ObjectMagicProperties**: Item magical properties
- **Positioned**: Entity positioning data
- **Shrine**: Shrine effects and data
- **TriggerableBlockage**: Doors and barriers

### UI Elements

#### UiElementBaseOffset
Located in: `GameOffsets/Objects/UiElement/UiElementBaseOffset.cs`
**Purpose**: Base structure for all UI elements

#### ImportantUiElementsOffsets
Located in: `GameOffsets/Objects/States/InGameState/ImportantUiElementsOffsets.cs`
**Purpose**: Key UI elements like inventory, skill panel, map, etc.

### File Structures

#### BaseItemTypesDatOffsets
Located in: `GameOffsets/Objects/FilesStructures/BaseItemTypesDatOffsets.cs`
**Purpose**: Item type definitions from game data files

#### WorldAreaDatOffsets
Located in: `GameOffsets/Objects/FilesStructures/WorldAreaDatOffsets.cs`
**Purpose**: Area and zone definitions from game data files

### Native Types

Located in: `GameOffsets/Natives/`
- **StdVector**: C++ std::vector implementation
- **StdWString**: C++ std::wstring implementation
- **StdMap**: C++ std::map implementation
- **StdList**: C++ std::list implementation
- **StdBucket**: Hash bucket structure
- **StdTuple2D/3D**: 2D and 3D coordinate structures

## How to Find New Patterns

### Method 1: Using Cheat Engine (Recommended)

This is the most common method described in the pattern comments:

#### For Game States Pattern:
1. Open Cheat Engine and attach to Path of Exile
2. Search for string "InGameState" (UTF-16, case insensitive)
3. Use "Find out what accesses this address" on results
4. Look through registers (RAX, RBX, RCX) for HashNode address
5. Validate: HashNode Address + 0x20 should point to "InGameState" string
6. Open HashNode in "Dissect data/structure" window
7. At offset 0x08 is the Root HashNode (copy this value)
8. Validate Root: offset 0x19 (byte) should be 1
9. Pointer scan with settings:
   - Maximum Offset Value: 1024
   - Maximum Level: 2-3
   - "Allow stack addresses": unchecked
10. Use "Find out what accesses this address" and create pattern

#### For File Root Pattern:
1. Search for "Mods.dat" string (UTF-16, case sensitive, writable: false)
2. "Find out what accesses this address"
3. Find instructions in POE memory (not kernel/Windows)
4. Check static addresses in registers
5. Look for float value 1.0 or 0.0 before/at the green address
6. Use "Find out what accesses this address" on the float
7. Change areas 2-3 times in game
8. Sort addresses and pick lowest one
9. Create pattern from the instruction

#### For AreaChangeCounter Pattern:
1. Search for 4 bytes, type "UnknownInitialValue"
2. Change areas repeatedly
3. Each area change: "Next Scan" with "Increased Value"
4. Find 2 final addresses, pick the smaller one
5. Use "What writes to this address" (not "accesses") for pattern
6. Validate: larger address increments by 3 when going from character select to in-game

### Method 2: Static Analysis

1. Use disassemblers like IDA Pro, Ghidra, or x64dbg
2. Look for string references and cross-references
3. Find functions that access the data you need
4. Create patterns from unique instruction sequences

### Method 3: Dynamic Analysis

1. Use debuggers to set breakpoints on memory access
2. Trace execution to find consistent access patterns
3. Create byte patterns from the instruction sequences

### Pattern Creation Guidelines

1. **Make patterns unique**: Ensure your pattern only matches one location
2. **Use enough context**: Include surrounding bytes for uniqueness
3. **Minimize wildcards**: Use ?? only when necessary
4. **Test thoroughly**: Verify pattern works across game restarts
5. **Document purpose**: Always comment what the pattern finds

### Pattern Testing

```csharp
// Add your pattern to StaticOffsetsPatterns.cs
new(
    "Your Pattern Name",
    "48 8B ?? ^ ?? ?? ?? ?? 89 ?? ?? ?? ?? ??"
),

// Test with PatternFinder.Find() method
var patterns = PatternFinder.Find(handle, baseAddress, processSize);
```

## How to Use Offsets

### Basic Memory Reading

```csharp
// Get static address from pattern
var gameStatesAddress = Core.Process.StaticAddresses["Game States"];

// Read the game state structure
var gameState = Core.Process.Handle.ReadMemory<GameStateOffset>(gameStatesAddress);

// Access specific state (InGameState is usually State2)
var inGameStatePtr = gameState.State2;

// Read InGameState structure
var inGameState = Core.Process.Handle.ReadMemory<InGameStateOffset>(inGameStatePtr);
```

### Reading Entity Data

```csharp
// Get entity list from area instance
var areaInstance = Core.Process.Handle.ReadMemory<AreaInstanceOffsets>(inGameState.AreaInstanceData);
var entityList = Core.Process.Handle.ReadStdVector<IntPtr>(areaInstance.MonsterList);

foreach (var entityPtr in entityList)
{
    // Read entity structure
    var entity = Core.Process.Handle.ReadMemory<EntityOffsets>(entityPtr);
    
    // Check if entity is valid
    if (!EntityHelper.IsValidEntity(entity.IsValid)) continue;
    
    // Read entity details
    var entityDetails = Core.Process.Handle.ReadMemory<EntityDetails>(entity.ItemBase.EntityDetailsPtr);
    var entityName = Core.Process.Handle.ReadStdWString(entityDetails.name);
    
    // Read component (e.g., Life component)
    var componentLookup = Core.Process.Handle.ReadMemory<ComponentLookUpStruct>(entityDetails.ComponentLookUpPtr);
    // ... lookup specific component
}
```

### Reading Component Data

```csharp
// Example: Reading Life component
var lifeComponentPtr = GetComponentAddress(entity, "Life");
if (lifeComponentPtr != IntPtr.Zero)
{
    var lifeComponent = Core.Process.Handle.ReadMemory<LifeOffset>(lifeComponentPtr);
    
    // Access health data
    var health = lifeComponent.Health;
    var currentHealth = health.Current;
    var maxHealth = health.Total;
    var healthPercent = health.CurrentInPercent();
    
    // Check if regenerating
    var isRegenerating = health.Regeneration > 0;
}
```

### Reading UI Elements

```csharp
// Access UI root
var uiRoot = Core.Process.Handle.ReadMemory<UiElementBaseOffset>(inGameState.UiRootPtr);

// Access important UI elements
var importantUi = Core.Process.Handle.ReadMemory<ImportantUiElementsOffsets>(inGameState.IngameUi);

// Example: Check if inventory is open
var inventoryPtr = importantUi.OpenLeftPanel;
if (inventoryPtr != IntPtr.Zero)
{
    var inventoryElement = Core.Process.Handle.ReadMemory<UiElementBaseOffset>(inventoryPtr);
    var isVisible = inventoryElement.IsVisible;
}
```

## Examples

### Example 1: Monster Health Bar Overlay

```csharp
public void DrawMonsterHealthBars()
{
    var inGameState = GetInGameState();
    if (inGameState.AreaInstanceData == IntPtr.Zero) return;
    
    var areaInstance = Core.Process.Handle.ReadMemory<AreaInstanceOffsets>(inGameState.AreaInstanceData);
    var monsters = Core.Process.Handle.ReadStdVector<IntPtr>(areaInstance.MonsterList);
    
    foreach (var monsterPtr in monsters)
    {
        var monster = Core.Process.Handle.ReadMemory<EntityOffsets>(monsterPtr);
        if (!EntityHelper.IsValidEntity(monster.IsValid)) continue;
        
        // Get monster position
        var renderPtr = GetComponentAddress(monster, "Render");
        if (renderPtr == IntPtr.Zero) continue;
        
        var render = Core.Process.Handle.ReadMemory<RenderOffsets>(renderPtr);
        var worldPos = render.CurrentWorldPosition;
        
        // Get monster health
        var lifePtr = GetComponentAddress(monster, "Life");
        if (lifePtr == IntPtr.Zero) continue;
        
        var life = Core.Process.Handle.ReadMemory<LifeOffset>(lifePtr);
        var health = life.Health;
        
        // Convert world position to screen position
        var screenPos = WorldToScreen(worldPos);
        
        // Draw health bar
        DrawHealthBar(screenPos, health.Current, health.Total);
    }
}
```

### Example 2: Area Change Detection

```csharp
public class AreaChangeDetector
{
    private int lastAreaCounter = -1;
    
    public bool HasAreaChanged()
    {
        var counterAddress = Core.Process.StaticAddresses["AreaChangeCounter"];
        var areaChange = Core.Process.Handle.ReadMemory<AreaChangeOffset>(counterAddress);
        
        if (lastAreaCounter == -1)
        {
            lastAreaCounter = areaChange.counter;
            return false;
        }
        
        if (areaChange.counter != lastAreaCounter)
        {
            lastAreaCounter = areaChange.counter;
            return true;
        }
        
        return false;
    }
}
```

### Example 3: Player Stats Display

```csharp
public PlayerStats GetPlayerStats()
{
    var player = GetPlayerEntity();
    if (player == IntPtr.Zero) return null;
    
    var entity = Core.Process.Handle.ReadMemory<EntityOffsets>(player);
    
    // Get player name
    var playerPtr = GetComponentAddress(entity, "Player");
    var playerComp = Core.Process.Handle.ReadMemory<PlayerOffsets>(playerPtr);
    var playerName = Core.Process.Handle.ReadStdWString(playerComp.Name);
    
    // Get life data
    var lifePtr = GetComponentAddress(entity, "Life");
    var life = Core.Process.Handle.ReadMemory<LifeOffset>(lifePtr);
    
    return new PlayerStats
    {
        Name = playerName,
        Health = life.Health.Current,
        MaxHealth = life.Health.Total,
        Mana = life.Mana.Current,
        MaxMana = life.Mana.Total,
        EnergyShield = life.EnergyShield.Current,
        MaxEnergyShield = life.EnergyShield.Total
    };
}
```

## Tips and Best Practices

1. **Always check for null/zero pointers** before reading memory
2. **Validate entity data** using helper functions like `IsValidEntity`
3. **Cache static addresses** rather than looking them up repeatedly
4. **Handle game updates gracefully** - patterns may need updating
5. **Use try-catch blocks** for memory reading operations
6. **Test patterns thoroughly** across different game states
7. **Document your patterns** with clear comments about how to find them
8. **Keep patterns unique** to avoid false matches
9. **Update patterns promptly** when game patches break them
10. **Use minimal wildcards** for better pattern stability

## Troubleshooting

### Pattern Not Found
- Check if game version changed
- Verify pattern uniqueness with disassembler
- Try different instruction sequences around the target
- Update byte pattern if game code changed

### Offset Reading Crashes
- Verify pointer is valid before reading
- Check if entity/object is still alive
- Ensure correct structure alignment
- Validate memory permissions

### Incorrect Data
- Check if offset structure matches game version
- Verify component lookup is working correctly
- Ensure reading correct data type/size
- Check for endianness issues

## Conclusion

This documentation covers all patterns and data offsets available in the GameOverlay repository. The system provides comprehensive access to Path of Exile's internal data structures through pattern scanning and memory reading techniques. Use this information responsibly and in accordance with the game's terms of service.

For updates or new patterns, refer to the game's current version and use the methods described in the "How to Find New Patterns" section.