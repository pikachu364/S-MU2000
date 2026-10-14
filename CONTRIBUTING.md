# Development Workflow

This fork follows the upstream repository, `tarboh/S-MU2000`, as its primary source of truth. Independent features are developed separately and proposed upstream whenever practical.

## Branches

- `main`: Keep synchronized with upstream. Avoid unfinished experimental features.
- `feature/*`: Develop independent features and fixes in isolated branches.
- Draft pull requests: Share implementation plans and request early feedback from upstream.

## Upstream synchronization

1. Review upstream changes before synchronizing.
2. Update `main` and verify the build and existing tests.
3. Rebase or merge feature branches as appropriate, keeping upstream changes separate from feature work.
4. Re-run relevant tests after integration.

## Pull request readiness

Before marking a pull request ready for review:

- Verify the build and relevant tests.
- Document known limitations and unresolved format behavior.
- Use synthetic test data where practical.
- Do not include private research materials or copyrighted sample files without permission.

## RCP2 support

RCP2 support is developed incrementally. Header parsing, event parsing, timing conversion, and playback integration should be reviewed as distinct stages. Unverified behavior must be documented rather than presented as complete.

## Future independent distribution

This repository remains an upstream-following fork for now. If an independent distribution becomes necessary, create a separate repository at that time rather than turning this fork's `main` into an independent product line.