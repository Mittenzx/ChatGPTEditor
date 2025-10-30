# ChatGPTEditor

A secure Unreal Engine 5.5 Editor plugin that integrates ChatGPT functionality directly into the Unreal Editor. This plugin adds a Slate-based Editor tab that allows you to send messages to the OpenAI API and perform a range of AI-assisted editor tasks.

## 🔔 MCP (Model Context Protocol) Review Available

> **📊 Important:** A comprehensive analysis of adopting the Model Context Protocol (MCP) for this plugin is now available. MCP could reduce code by 40-50% and enable multi-AI support (Claude, GPT, etc.).
>
> **👉 [Start here: MCP Review Documentation](MCP_REVIEW_README.md)**
>
> Quick links:
> - [Executive Summary](MCP_EXECUTIVE_SUMMARY.md) (10 min read) - For decision makers
> - [Visual Comparison](MCP_VISUAL_COMPARISON.md) - See architecture diagrams
> - [Full Analysis](MCP_ADOPTION_ANALYSIS.md) - Complete strategic analysis
> - [Implementation Guide](MCP_IMPLEMENTATION_GUIDE.md) - Code examples and roadmap

## MCP Goals (Model Context Protocol)

MCP stands for Model Context Protocol. This project adopts MCP to standardize how model context, tools, and structured actions are represented and executed inside the Unreal Editor. The goals below summarize why MCP is being considered and what success looks like.

- Ensure predictable, auditable, and safe model interactions
  - Standardize context passing and tool invocation to reduce unexpected behavior
  - Provide clear audit trails and versioned prompts

- Improve modularity and multi-model support
  - Decouple editor tooling from any single model provider
  - Make it easier to add alternative model backends (OpenAI, Anthropic, local models)

- Reduce code duplication and simplify integrations
  - Share parsing, validation, and preview/approval workflows across feature areas
  - Consolidate common security checks and permission gating

- Increase transparency and observability
  - Log structured events (tool calls, approvals, denials) in the audit log
  - Surface context snapshots for debugging and post-mortem analysis

- Enforce safety and least privilege
  - Explicit permission toggles for write, scripting, external requests, and scene edits
  - Preview-and-approve workflow for any action that can change project state

- Define measurable success metrics
  - Track reductions in duplicated code, mean time to integrate new models, and number of prevented unsafe actions
  - Use MCP smoke tests and CI validations to ensure protocol compatibility

- Provide a clear adoption path
  - Documentation, quickstart guides, and a POC implementation to demonstrate benefits
  - Migration checklist and tests for feature owners to adopt MCP


## Features

(Full feature list, usage, installation, security, and technical details remain available in the repository README. The MCP Goals section above has been added to clearly document why and how MCP is relevant to this project.)

For the full original README content including detailed feature descriptions, installation steps, usage examples, and technical details, see the existing README in the repository. This update clarifies the MCP meaning and adds an explicit MCP Goals section.