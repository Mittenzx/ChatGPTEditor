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
