# Exhaustive Number Sense Device

A C++ expression solver that finds mathematical equations by inserting operators between digits while preserving their order.

**Latest: v0.3** · Complete solution search with configurable timeout

##  Quick Start

Download the latest release: **[NumberSolver-v0.3.exe](../../releases)**

```bash
# Run directly - no installation required
./NumberSolver-v0.3.exe
```

## Features

- **Complete search**: Finds ALL valid solutions (verified no missing solutions)
- **Multi-threaded**: Utilizes all CPU cores for faster search
- **Configurable timeout**: Set per-task timeout (ms) or unlimited for complete results
- **Auto-retry**: Automatically retry timed-out tasks with increased timeout
- **Smart output**: Solutions sorted by length (shorter = better)
- **Supported operators**: `+ - * / ^`

## Example

Input: `12345`

Output: 11 solutions found, including:
```
(12-3)-4=5
((1+2)*3)-4=5
1-((2-3)*4)=5
...
```

Shortest solution: `(12-3)-4=5`

## Version History

- **v0.3** (2026-05-01): Fixed pruning logic bug, now finds ALL solutions. Added configurable timeout and auto-retry.
- **v0.2** (2025-10-31): Added timeout mechanism and 2x performance boost.
- **v0.1** (2025-10-29): Initial release.
