# ChatGPTEditor Accessibility Guide

This document outlines the accessibility features and considerations for the ChatGPTEditor plugin.

## Overview

The ChatGPTEditor plugin is designed with accessibility in mind, providing features that accommodate different user needs and preferences. This guide explains how to use these features and what accessibility standards the plugin follows.

## Accessibility Features

### 1. Visual Accessibility

#### Adjustable Font Sizes
- **Range**: 8pt to 24pt
- **Default**: 10pt
- **Purpose**: Accommodates different visual acuity levels and display sizes

**How to Adjust**:
- Use **A-** button or **Ctrl+Minus** to decrease
- Use **A+** button or **Ctrl+Plus** to increase
- Use font size display or **Ctrl+0** to reset

**Recommended Sizes**:
- **8-10pt**: Good visual acuity, compact display preference
- **12-14pt**: Average reading comfort
- **16-18pt**: Low vision assistance, large displays
- **20-24pt**: Maximum size for visual impairment

#### High Contrast Support
- Uses Unreal Engine's standard Slate styling
- Inherits editor's theme (light or dark)
- Maintains WCAG AA contrast ratios for text
- Clear visual separation between UI sections

#### Visual Indicators
- **Icons**: Emoji symbols provide visual cues (🔒, ⏳, 💡, ⌨️)
- **Color**: Status messages use appropriate colors (gray for system, red for errors)
- **Layout**: Clear visual hierarchy with separators and spacing
- **Focus**: Standard Unreal Editor focus indicators on interactive elements

### 2. Keyboard Accessibility

#### Full Keyboard Support
All functionality is accessible without a mouse:

**Message Operations**:
- Send message: **Ctrl+Enter**
- Clear history: **Ctrl+L**

**Font Size Control**:
- Increase: **Ctrl+Plus** or **Ctrl+=**
- Decrease: **Ctrl+Minus** or **Ctrl+-**
- Reset: **Ctrl+0**

**Navigation**:
- **Tab**: Move to next element
- **Shift+Tab**: Move to previous element
- **Space/Enter**: Activate focused button
- **Arrow Keys**: Navigate within text areas

#### Keyboard Navigation Order
The focus order follows a logical top-to-bottom, left-to-right pattern:
1. Security permission checkboxes (Asset Write, Console Commands, File I/O)
2. Conversation history area (read-only)
3. Message input box
4. Send button
5. Clear button
6. Accessibility controls (A-, font size, A+)

#### No Keyboard Traps
- All elements can be reached and exited via keyboard
- Tab order cycles through all interactive elements
- Standard Unreal Editor escape behaviors work

### 3. Cognitive Accessibility

#### Clear Labels
- All buttons have descriptive text
- Checkboxes explain their purpose
- Input fields include hint text
- No ambiguous icons or symbols without labels

#### Tooltips
Every interactive element includes descriptive tooltips:
- **Buttons**: Action description and keyboard shortcut
- **Checkboxes**: Security implications and warnings
- **Input Fields**: Usage instructions
- **Controls**: Purpose and keyboard alternatives

#### Consistent Layout
- UI elements remain in predictable locations
- Similar functions grouped together
- Clear visual sections with separators
- Standard Unreal Editor patterns

#### Status Feedback
- Loading indicators show when requests are processing
- Success/error messages clearly displayed
- Visual and textual confirmation of actions
- Conversation history shows all interactions

### 4. Motor Accessibility

#### Large Click Targets
- Buttons sized for easy clicking
- Checkboxes use standard Slate sizing
- Input areas have adequate padding
- No tiny controls that require precision

#### Keyboard Alternatives
- All mouse actions have keyboard equivalents
- No drag-and-drop requirements
- Click-to-activate (no hover requirements)
- Reasonable timing (no time-limited actions)

#### Reduced Mouse Dependency
- Primary workflow (type, send, read) works keyboard-only
- Font adjustment available via keyboard
- Navigation doesn't require mouse
- Docking uses standard Unreal Editor behaviors

## Accessibility Standards Compliance

### WCAG 2.1 Guidelines

The plugin aims to meet WCAG 2.1 Level AA standards where applicable:

#### Perceivable
- ✅ **Text Alternatives**: Tooltips provide text for all interactive elements
- ✅ **Adaptable**: Font size can be adjusted without losing functionality
- ✅ **Distinguishable**: High contrast ratios, clear focus indicators

#### Operable
- ✅ **Keyboard Accessible**: Full keyboard support for all functions
- ✅ **Enough Time**: No time limits on user interactions
- ✅ **Navigable**: Logical focus order, clear page titles (tab name)

#### Understandable
- ✅ **Readable**: Clear text, adjustable size, good contrast
- ✅ **Predictable**: Consistent navigation, standard behaviors
- ✅ **Input Assistance**: Clear labels, error messages, help text

#### Robust
- ✅ **Compatible**: Uses standard Slate widgets and Unreal patterns
- ✅ **Standard Controls**: Familiar UI elements and behaviors

### Current Limitations

While the plugin includes many accessibility features, some limitations exist:

#### Screen Reader Support
- ⚠️ **Limited**: Unreal Editor's Slate framework has limited screen reader support
- ⚠️ **Alternative**: Tooltips and clear labels provide textual descriptions
- 🔮 **Future**: Improved screen reader support planned for future versions

#### Custom Themes
- ⚠️ **Limited**: Custom high-contrast themes not yet available
- ⚠️ **Alternative**: Uses editor's theme settings
- 🔮 **Future**: Theme customization planned for future versions

#### Keyboard Shortcut Customization
- ⚠️ **Limited**: Shortcuts are currently fixed
- ⚠️ **Alternative**: Button clicks work as fallback
- 🔮 **Future**: Customizable shortcuts planned for future versions

## Best Practices for Accessible Use

### For Visual Impairment

1. **Adjust Font Size**:
   - Start with default (10pt) and adjust as needed
   - Use **Ctrl+Plus** to increase if text is too small
   - Maximum 24pt available for low vision

2. **Use High Contrast**:
   - Enable Unreal Editor's high contrast theme if available
   - The plugin inherits editor contrast settings

3. **Reduce Visual Clutter**:
   - Dock the window to reduce overlapping
   - Clear conversation history when starting new topics
   - Focus on one conversation at a time

### For Motor Impairment

1. **Maximize Keyboard Use**:
   - Learn keyboard shortcuts to minimize mouse use
   - Use **Ctrl+Enter** instead of clicking Send
   - Use **Tab** for navigation

2. **Adjust Input Method**:
   - Use speech-to-text for message input (OS-level)
   - Take breaks between long typing sessions
   - Use copy-paste for repeated phrases

3. **Customize Layout**:
   - Dock window in easiest-to-reach location
   - Resize window for comfortable viewing
   - Use larger buttons via font size adjustment

### For Cognitive Accessibility

1. **Use Clear Communication**:
   - Read tooltips for unfamiliar controls
   - Follow the visual icons (🔒, ⏳, etc.)
   - Use the keyboard shortcuts reference at bottom

2. **Manage Complexity**:
   - Keep security permissions off unless needed
   - Clear history to reduce information overload
   - Focus on one task at a time

3. **Leverage Visual Structure**:
   - Note the clear sections (Security, Conversation, Input)
   - Follow the top-to-bottom workflow
   - Use separators as visual guides

## Testing Accessibility

### Keyboard Navigation Test
1. Open the ChatGPT window
2. Click in the message input box
3. Try each keyboard shortcut:
   - Type a message and press **Ctrl+Enter**
   - Press **Ctrl+L** to clear
   - Press **Ctrl+Plus** several times
   - Press **Ctrl+0** to reset
   - Press **Ctrl+Minus** several times
4. Use **Tab** to navigate all elements
5. Use **Shift+Tab** to navigate backwards

### Font Size Test
1. Press **Ctrl+0** to ensure default size
2. Press **Ctrl+Plus** multiple times
3. Verify text increases up to 24pt
4. Press **Ctrl+Minus** multiple times
5. Verify text decreases down to 8pt
6. Press **Ctrl+0** to verify reset to 10pt

### Tooltip Test
1. Hover over the Send button
2. Wait 1 second
3. Verify tooltip appears with description
4. Test all other buttons and checkboxes
5. Verify each has a descriptive tooltip

### Focus Indication Test
1. Press **Tab** repeatedly
2. Verify visible focus indicator on each element
3. Verify focus order is logical
4. Verify no keyboard traps exist

## Reporting Accessibility Issues

If you encounter accessibility issues:

1. **Document the Issue**:
   - Describe what you're trying to do
   - Explain what happens vs. what should happen
   - Note your assistive technology (if using)
   - Include steps to reproduce

2. **Report via GitHub**:
   - Open an issue with "Accessibility:" prefix
   - Include your documentation
   - Tag with "accessibility" label if possible

3. **Include Context**:
   - Your Unreal Engine version
   - Your operating system
   - Your input devices
   - Any assistive technology used

## Future Accessibility Enhancements

Planned improvements for future versions:

### Short Term
- [ ] Customizable keyboard shortcuts
- [ ] Additional font size presets (quick select)
- [ ] Conversation timestamp display
- [ ] Message search/filter functionality

### Medium Term
- [ ] High-contrast theme toggle
- [ ] Color customization options
- [ ] Voice input integration
- [ ] Improved focus indicators

### Long Term
- [ ] Screen reader support (WAI-ARIA equivalent)
- [ ] Braille display support
- [ ] Alternative input methods
- [ ] Accessibility settings persistence

## Resources

### External Resources
- [WCAG 2.1 Guidelines](https://www.w3.org/WAI/WCAG21/quickref/)
- [Unreal Engine Accessibility](https://www.unrealengine.com/en-US/accessibility)
- [Keyboard Accessibility Guide](https://webaim.org/techniques/keyboard/)

### Internal Documentation
- [README.md](README.md) - Main plugin documentation
- [USAGE_GUIDE.md](USAGE_GUIDE.md) - Detailed usage instructions

## Contact

For accessibility-specific questions or feedback:
- Open a GitHub issue with "Accessibility:" prefix
- Describe your specific needs or concerns
- We welcome feedback from users of assistive technology

---

**Commitment**: We are committed to making the ChatGPTEditor plugin accessible to all users and welcome feedback to improve accessibility.

**Last Updated**: Version 1.1.0 (UX & Accessibility Update)
