# Scene Editing Examples

This document provides comprehensive examples of scene editing commands supported by the ChatGPTEditor plugin.

## Prerequisites

1. Enable the "Allow Scene Editing" permission in the ChatGPT window
2. Have a level open in the Unreal Editor
3. Confirm the warning dialog when enabling the permission

## Actor Spawning Examples

### Lights

```
Add 5 lights to this room
Add 3 point lights
Spawn 10 spot lights
Add a directional light
Place 2 lights
```

### Cameras

```
Add a camera at PlayerStart
Place a camera
Spawn 3 cameras
```

### Other Actors

```
Add 5 props
Spawn static meshes
```

## Actor Movement Examples

### Moving Up/Down

```
Move all props up by 100 units
Move all lights up by 200
Move all cameras down by 50
Move triggers up by 150
```

**Note:** Units are in Unreal units (typically centimeters)

## Actor Deletion Examples

```
Delete all trigger volumes
Remove all lights
Delete all cameras
Remove all props
Delete all static meshes
```

**Warning:** Deletion operations are destructive. The preview dialog will show which actors will be affected. Critical actors like PlayerStart and Brushes are protected.

## Property Modification Examples

### Light Color Changes

```
Change light color to red
Set light color to blue
Change light color to green
```

## Advanced Usage Tips

### Combining Operations

While the current implementation processes one command at a time, you can:

1. Send multiple commands sequentially
2. Review each preview dialog
3. Confirm or cancel each operation

### Using the Audit Log

1. Click "View Audit Log" to see all operations
2. Review timestamps and affected actors
3. Check for any failed operations
4. Use this for debugging and accountability

### Undoing Changes

All scene editing operations support Unreal's native Undo system:

1. Press `Ctrl+Z` (Windows/Linux) or `Cmd+Z` (macOS) to undo the last operation
2. Press `Ctrl+Y` (Windows/Linux) or `Cmd+Shift+Z` (macOS) to redo

### Safety Best Practices

1. **Always review the preview** before confirming changes
2. **Use descriptive commands** to ensure correct parsing
3. **Start with small operations** to understand the behavior
4. **Keep the audit log open** during complex editing sessions
5. **Save your level frequently** when using scene editing
6. **Test in a copy** of your level first if unsure

## Command Syntax Patterns

The plugin recognizes these patterns:

### Spawning
- `add [number] [actor type]`
- `spawn [number] [actor type]`
- `place [actor type] at [location]`

### Moving
- `move [actor type] up by [number]`
- `move [actor type] down by [number]`

### Deleting
- `delete all [actor type]`
- `remove all [actor type]`

### Modifying
- `change [actor type] [property] to [value]`
- `set [actor type] [property] to [value]`

## Troubleshooting

### "Could not parse scene editing command"

This means the command didn't match any known patterns. Try:
- Using one of the example commands above
- Simplifying your command
- Breaking complex operations into smaller steps

### "Scene editing is disabled"

You need to:
1. Enable the "Allow Scene Editing" permission checkbox
2. Confirm the warning dialog

### "No active world found"

This means no level is open. Solution:
1. Open an existing level (File → Open Level)
2. Or create a new level (File → New Level)

### Unexpected Results

If the operation didn't work as expected:
1. Check the audit log for details
2. Use Ctrl+Z to undo the change
3. Try a more specific command
4. Review the preview dialog more carefully next time

## Future Enhancements

Planned features for future versions:
- More sophisticated natural language parsing
- Support for more actor properties
- Batch operations
- Saved operation templates
- Integration with OpenAI for smarter command interpretation
- Support for Blueprint actors
- Landscape and foliage editing

## Feedback

If you have suggestions for new scene editing commands or features, please open an issue on GitHub!
