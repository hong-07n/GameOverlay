# Quick Reference: Patterns and Offsets

## Available Patterns (6 total)

| Pattern Name | Purpose | Key Use Cases |
|--------------|---------|---------------|
| **Game States** | Main game state structure | Detect current game state (login/character select/in-game) |
| **File Root** | File loading system root | Detect loaded files, area file changes |
| **AreaChangeCounter** | Area change counter | Detect when player changes areas/zones |
| **GameWindowScaleValues** | UI scaling values | Proper overlay positioning with UI scale |
| **Terrain Rotator Helper** | Terrain rotation data | Map positioning and rotation calculations |
| **Terrain Rotation Selector** | Additional terrain rotation | Enhanced terrain rotation support |

## Key Data Structures

### Core Game Access
- **GameStateOffsets**: Access different game states (login, character select, in-game)
- **InGameStateOffset**: Primary in-game data (area, world, UI root)
- **AreaChangeOffset**: Simple area change counter

### Entity System
- **EntityOffsets**: Base entity structure (players, monsters, items, NPCs)
- **EntityDetails**: Entity names and component lookup

### Components (for entities)
- **LifeOffset**: Health, mana, energy shield with reservations and regeneration
- **PlayerOffsets**: Player character name
- **RenderOffsets**: World position (X,Y,Z), terrain height
- **ActorOffset**: Animation data
- **Buffs, Charges, Chest, ObjectMagicProperties, Positioned, Shrine, TriggerableBlockage**

### UI System
- **UiElementBaseOffset**: Base UI element structure
- **ImportantUiElementsOffsets**: Key UI panels (inventory, skills, map)

### File/Data System
- **LoadedFilesOffset**: Files loaded in current area
- **BaseItemTypesDatOffsets**: Item type definitions
- **WorldAreaDatOffsets**: Area/zone definitions

## Quick Usage Examples

### Check Current Game State
```csharp
var gameStatesAddr = Core.Process.StaticAddresses["Game States"];
var gameState = Core.Process.Handle.ReadMemory<GameStateOffset>(gameStatesAddr);
var inGameStatePtr = gameState.State2; // InGameState is usually State2
```

### Detect Area Changes
```csharp
var counterAddr = Core.Process.StaticAddresses["AreaChangeCounter"];
var areaChange = Core.Process.Handle.ReadMemory<AreaChangeOffset>(counterAddr);
// Compare areaChange.counter with previous value
```

### Read Player Health
```csharp
var playerEntity = GetPlayerEntity();
var lifePtr = GetComponentAddress(playerEntity, "Life");
var life = Core.Process.Handle.ReadMemory<LifeOffset>(lifePtr);
var health = life.Health; // VitalStruct with Current, Total, etc.
```

### Get Entity Position
```csharp
var renderPtr = GetComponentAddress(entity, "Render");
var render = Core.Process.Handle.ReadMemory<RenderOffsets>(renderPtr);
var worldPos = render.CurrentWorldPosition; // StdTuple3D<float>
```

## Pattern Format
- **Hex bytes**: `48`, `8B`, `F1` (exact match)
- **Wildcards**: `??` or `?` (any byte)
- **Skip marker**: `^` (indicates BytesToSkip)
- **Example**: `"48 83 EC ?? 48 8B F1 33 ED 48 39 2D ^ ?? ?? ?? ??"`

## Finding New Patterns (Cheat Engine Method)
1. Search for relevant strings or known values
2. "Find out what accesses this address"
3. Analyze instructions and registers
4. Create unique byte pattern from instruction sequence
5. Test pattern for uniqueness and stability

## Memory Reading Safety
- Always check for null/zero pointers
- Validate entities with `EntityHelper.IsValidEntity()`
- Use try-catch blocks for memory operations
- Cache static addresses for performance

For complete documentation, see `PATTERNS_AND_OFFSETS_DOCUMENTATION.md`