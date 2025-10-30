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

> **Note**: A Phase 1 initialization PR has been opened to establish the foundational infrastructure for MCP POC stabilization. This PR includes the runbook, smoke-test harness, and PR template. Use the [MCP Phase 1 PR template](.github/PULL_REQUEST_TEMPLATE/MCP_PHASE1.md) for all Phase 1 PRs.

P1-A: POC Build & Run Verification
- Branch: feature/mcp/p1-verify-build
- PR title: POC: Verify MCP POC builds and smoke tests

Checklist:
- [ ] Create branch from main including merged PR #15
- [ ] Add docs/MCP_POC_RUNBOOK.md with exact build and run commands
- [ ] Add smoke-test harness or script (unit/in-editor) for: initialize, tools/list, EchoTool, SpawnActorTool
- [ ] Run full build locally (Windows/Linux as applicable) and capture logs
- [ ] Fix any compilation/link/runtime issues found
- [ ] Attach smoke test logs/screenshots to PR
- [ ] Add reviewers and mark PR ready for review

Acceptance criteria:
- [ ] Project builds cleanly and smoke-tests run successfully

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