# Accessibility Features

The ChatGPTEditor plugin is designed with accessibility in mind to ensure all users can effectively use the tool regardless of their abilities.

## Overview

This document outlines the accessibility features implemented in the ChatGPTEditor plugin for Unreal Engine 5.5, including compliance with WCAG 2.1 guidelines where applicable.

## Visual Accessibility

### Adjustable Font Sizes

**Feature:** Users can adjust the conversation history font size from 8pt to 24pt.

**How to Use:**
- **Mouse:** Click the A- or A+ buttons in the accessibility controls section
- **Keyboard:** Press Ctrl+Plus to increase, Ctrl+Minus to decrease, Ctrl+0 to reset
- **Default:** 10pt font size

**Benefits:**
- Accommodates users with low vision
- Reduces eye strain during extended use
- Customizable for different display sizes and resolutions

### High Contrast Support

**Feature:** Inherits from Unreal Editor's theme system.

**How It Works:**
- Plugin UI automatically adapts to the editor's current theme
- Uses standard Unreal Engine AppStyle for consistent contrast ratios
- Maintains readability across different theme configurations

### Visual Indicators

**Feature:** Icons and symbols provide visual cues for different UI elements.

**Indicators:**
- 🔒 Lock icon - Security-sensitive permissions
- ⏳ Hourglass - Loading/processing indicator
- 💡 Light bulb - Tips and helpful information
- ⌨️ Keyboard - Shortcut information
- ✓/✗ Checkmarks - Success/failure status

**Benefits:**
- Quick visual scanning of UI state
- Redundant cues (not relying solely on color)
- Clear feedback for actions

## Keyboard Accessibility

### Full Keyboard Navigation

**Feature:** All plugin functionality is accessible via keyboard.

**Key Shortcuts:**

| Shortcut | Action | Context |
|----------|--------|---------|
| Ctrl+Enter | Send message | When input box has focus |
| Ctrl+L | Clear history | Any time |
| Ctrl+Plus | Increase font size | Any time |
| Ctrl+Minus | Decrease font size | Any time |
| Ctrl+0 | Reset font size | Any time |
| Tab | Navigate between elements | Standard focus order |
| Space/Enter | Activate buttons/checkboxes | When focused |

**Platform Notes:**
- On macOS, use Cmd instead of Ctrl
- Standard tab order follows logical reading flow
- No keyboard traps - users can always navigate away

### Focus Management

**Feature:** Clear visual focus indicators for keyboard navigation.

**Implementation:**
- Uses Unreal Editor's standard focus visualization
- Focus order follows logical top-to-bottom, left-to-right pattern
- Focus never gets trapped in any component

## Cognitive Accessibility

### Clear UI Organization

**Feature:** Consistent, predictable layout with clear visual hierarchy.

**Structure:**
- Header with title at top
- Security permissions section
- Blueprint Scripting Assistant controls
- Conversation history (main area)
- Input controls at bottom
- Help text and keyboard shortcuts reference

**Benefits:**
- Predictable location of controls
- Logical grouping of related functions
- Minimal cognitive load for navigation

### Helpful Tooltips

**Feature:** Every interactive element has descriptive tooltip text.

**Content:**
- Brief description of what the control does
- Keyboard shortcuts where applicable
- Security warnings for dangerous operations
- Context-specific help information

**Benefits:**
- On-demand help without leaving the interface
- Reduces need to memorize functions
- Provides immediate feedback

### Status Feedback

**Feature:** Clear feedback for all user actions.

**Examples:**
- "Sending request to OpenAI..." when API call is made
- "✓ Successfully executed: Create Material" after asset creation
- Error messages explain what went wrong and how to fix it
- Loading indicators show when operations are in progress

## Motor Accessibility

### Large Click Targets

**Feature:** Buttons and interactive elements sized for easy clicking.

**Implementation:**
- Standard Unreal Engine button sizes (minimum 24x24 pixels)
- Adequate padding around all clickable elements
- Checkboxes and radio buttons use standard sizes

### Keyboard Alternatives

**Feature:** Every mouse action has a keyboard equivalent.

**Examples:**
- Send button → Ctrl+Enter
- Clear button → Ctrl+L
- Font size buttons → Ctrl+/-/0

**Benefits:**
- Reduces reliance on precise mouse control
- Supports alternative input devices
- Faster workflow for power users

## Compliance Information

### WCAG 2.1 Conformance

The plugin aims to meet WCAG 2.1 Level AA guidelines where applicable within the Unreal Engine editor context:

#### Level A (Fully Compliant)
- ✅ **1.1.1 Non-text Content** - UI icons have text alternatives via tooltips
- ✅ **1.3.1 Info and Relationships** - Semantic grouping via visual separators
- ✅ **1.4.1 Use of Color** - Not solely relying on color (uses icons and text)
- ✅ **2.1.1 Keyboard** - All functionality available via keyboard
- ✅ **2.4.7 Focus Visible** - Focus indicators visible (inherited from UE)
- ✅ **3.1.1 Language of Page** - English UI text
- ✅ **4.1.2 Name, Role, Value** - All controls properly labeled via tooltips

#### Level AA (Partially Compliant)
- ✅ **1.4.3 Contrast (Minimum)** - Inherits from UE editor theme (7:1 ratio typically)
- ✅ **1.4.4 Resize Text** - Font size adjustable up to 200%
- ⚠️ **1.4.5 Images of Text** - Uses standard UI fonts (except for icons)
- ✅ **2.4.6 Headings and Labels** - Clear section labels and headings
- ✅ **3.2.3 Consistent Navigation** - Consistent layout and controls

#### Not Applicable
- **1.2.x Audio/Video** - Plugin contains no multimedia content
- **1.4.2 Audio Control** - No audio is played by the plugin
- **2.2.x Timing** - No time-based restrictions on user actions
- **2.3.x Seizures** - No flashing or rapidly changing content

## Known Limitations

### Screen Reader Support

**Current Status:** Limited  
**Reason:** Unreal Engine's Slate UI framework does not natively support ARIA-like attributes for screen readers.

**Workarounds:**
- Comprehensive tooltip text on all elements
- Clear button labels and section headers
- Keyboard navigation fully supported
- Status messages in conversation history

**Future Plans:**
- Investigate UE accessibility APIs as they evolve
- Consider adding audio feedback for operations
- Explore third-party accessibility solutions

### Magnification

**Current Status:** Partial support  
**Feature:** Font size adjustment (8-24pt)  
**Limitation:** UI chrome and button sizes don't scale

**Workarounds:**
- Operating system-level magnification works
- UE editor zoom settings apply
- Font size helps with main content area

## Future Enhancements

### Short Term (Planned)
- Custom keyboard shortcut configuration
- Additional font size presets (quick access to common sizes)
- Color theme customization options
- Enhanced focus indicators

### Medium Term (Under Consideration)
- High-contrast theme toggle independent of editor theme
- Dyslexia-friendly font options
- Customizable icon sizes
- Voice command integration

### Long Term (Exploring)
- Screen reader compatibility layer
- Braille display support (if UE APIs permit)
- Alternative input method support
- AI-powered accessibility improvements

## Testing Accessibility Features

### Manual Testing Checklist

**Keyboard Navigation:**
- [ ] Tab through all elements in logical order
- [ ] All shortcuts work (Ctrl+Enter, Ctrl+L, Ctrl+/-/0)
- [ ] No keyboard traps encountered
- [ ] Focus indicators visible on all elements

**Visual Accessibility:**
- [ ] Font size adjusts correctly (8-24pt range)
- [ ] A-/A+ buttons work as expected
- [ ] Reset to default works (Ctrl+0)
- [ ] UI readable at minimum and maximum sizes

**Tooltips:**
- [ ] All buttons have tooltips
- [ ] All checkboxes have tooltips
- [ ] Input fields have hint text
- [ ] Tooltips appear on hover

**Status Feedback:**
- [ ] Loading indicators show during API calls
- [ ] Success/failure messages appear in conversation
- [ ] Error messages are clear and actionable

### Testing with Assistive Technologies

**Keyboard-Only Testing:**
1. Unplug mouse or disable trackpad
2. Navigate entire interface using only keyboard
3. Verify all functions accessible
4. Check focus visibility

**High Contrast Testing:**
1. Enable high-contrast mode in UE editor
2. Verify all text is readable
3. Check that UI elements are distinguishable
4. Confirm icons and indicators are visible

**Font Size Testing:**
1. Test at minimum size (8pt)
2. Test at maximum size (24pt)
3. Test at default size (10pt)
4. Verify layout doesn't break at any size

## Reporting Accessibility Issues

We take accessibility seriously. If you encounter accessibility barriers:

### How to Report

1. **GitHub Issues:** Open an issue with the "accessibility" label
2. **Include:**
   - Your accessibility needs
   - Specific barrier encountered
   - Steps to reproduce
   - Assistive technology being used (if applicable)
   - Suggested improvements

### Response Time

- Accessibility issues are treated as high priority
- We aim to respond within 2 business days
- Critical accessibility bugs are addressed immediately

### Collaboration

We welcome collaboration from the accessibility community:
- Feature suggestions
- Testing assistance
- Code contributions
- Documentation improvements

## Resources

### Accessibility Standards
- [WCAG 2.1 Guidelines](https://www.w3.org/WAI/WCAG21/quickref/)
- [Unreal Engine Accessibility](https://www.unrealengine.com/en-US/accessibility)

### Testing Tools
- Keyboard navigation (built into OS)
- High-contrast modes (OS settings)
- Screen magnification (OS accessibility features)

### Related Documentation
- [USAGE_GUIDE.md](./USAGE_GUIDE.md) - Keyboard shortcuts and features
- [README.md](./README.md) - General plugin information
- [SECURITY.md](./SECURITY.md) - Security and permission information

---

**Last Updated:** October 26, 2025  
**Plugin Version:** 1.2  
**WCAG Version:** 2.1 Level AA (partial)

We are committed to continuous improvement of accessibility features. Your feedback helps us build a more inclusive tool.
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
