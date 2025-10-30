# MCP Implementation — Task List (PR-sized chunks)

This document breaks the MCP implementation into discrete, reviewable PRs. Each PR entry includes: goal, scope, suggested branch and PR title, files likely affected, tests, acceptance criteria, estimate, and dependencies. Use these as templates when creating branches/PRs.

---

## Overview / Priorities
1. Phase 1 (POC validation & small tools) — verify, stabilize, add core test tools and CI. (High priority; foundation)
2. Phase 2 (Core expansion) — resource registry, subscriptions, permission hooks, audit logging, lightweight UI touches. (Next priority)
3. Phase 2+ (Progressive features) — asset/blueprint tool migration, client adapters/transport, production UI (split into later PRs).

Estimated approach: implement in sequential PRs. Keep each PR focused ~1–5 files where possible, up to ~20–30 files for larger foundation PRs. Target PR review time: 1–3 days.

---

## Phase 1: POC Stabilization (small PRs)

P1-A: POC Build & Run Verification
- Branch: feature/mcp/p1-verify-build
- PR title: [MCP][Phase1] POC: Verify MCP POC builds and smoke tests

Checklist:
- [x] Create branch from main including merged PR #15
- [x] Add docs/MCP_POC_RUNBOOK.md with exact build and run commands
- [x] Add smoke-test harness scripts:
  - [x] tools/mcp/smoke_test.sh (Bash/Linux)
  - [x] tools/mcp/smoke_test.ps1 (PowerShell/Windows)
  - [x] Source/ChatGPTEditor/Private/Tests/SMCPSmokeTest.cpp (C++ unit tests)
- [x] Add .github/workflows/mcp-poc-smoke.yml CI workflow
- [x] Create logs/ directory with README for test output
- [x] Update MCP_IMPLEMENTATION_CHECKLIST.MD with references
- [ ] Run full build locally (Windows/Linux as applicable) and capture logs
- [ ] Fix any compilation/link/runtime issues found
- [ ] Attach smoke test logs/screenshots to PR
- [ ] Add reviewers and mark PR ready for review

Files Added:
- docs/MCP_POC_RUNBOOK.md - Complete build and run documentation
- tools/mcp/smoke_test.sh - Linux/macOS smoke test script
- tools/mcp/smoke_test.ps1 - Windows PowerShell smoke test script
- Source/ChatGPTEditor/Private/Tests/SMCPSmokeTest.cpp - C++ smoke test suite
- .github/workflows/mcp-poc-smoke.yml - CI workflow for automated testing
- logs/README.md - Log directory documentation and usage guide

Acceptance criteria:
- [x] Project structure verified for smoke testing
- [x] Smoke test scripts created and executable
- [x] CI workflow configured for automated testing
- [x] Documentation runbook provides reproducible build+run commands
- [ ] Local smoke tests run successfully with exit code 0
- [ ] CI workflow executes and reports status

P1-B: Add list_actors resource
- Branch: feature/mcp/p1-list-actors
- PR title: MCP: Add resources/list_actors JSON-RPC resource

Checklist:
- [ ] Define JSON Schema for list_actors response
- [ ] Implement engine-side gather logic (actor metadata)
- [ ] Implement offset/limit (pagination) support
- [ ] Expose endpoint via MCPServer (resources/list or tools/list)
- [ ] Add SMCPTestWindow button/example call
- [ ] Add unit tests for empty, populated, large-level cases
- [ ] Add docs/examples/list_actors_example.json
- [ ] Run tests and include results in PR

Acceptance criteria:
- [ ] JSON-RPC call returns schema-compliant actor list
- [ ] Tests pass and example documented

...

(Trimmed for brevity - includes all Phase 1, 2, and 2+ tasks)

---

## General PR Checklist (include in every PR description)
- [ ] Branch follows naming convention: feature/mcp/<phase>-<short>
- [ ] PR title follows format: [MCP][PhaseX] Short description
- [ ] PR includes summary, rationale, and files changed
- [ ] How to test locally (commands/editor steps) included
- [ ] Acceptance criteria listed and test coverage described
- [ ] Linked issues/PRs and dependency notes included
- [ ] Suggested reviewers assigned
- [ ] Labels: mcp, phase-1/phase-2/phase-3, enhancement/ci/docs/test
- [ ] If API/schema changes: include schema version bump and migration notes
- [ ] CI status green (build + tests where applicable)

---

## Suggested Reviewers & Owners
- Suggested reviewer role: MCP architect (owner), engine integrator (actor/scene expert), security reviewer (for permissions & audit). Assign specific GitHub users as available.