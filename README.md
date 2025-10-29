# exhaustive-number-sense-device
A C++ expression solver that finds valid equations by exhaustively inserting operators between digits while preserving their order.
## ⚡ Performance Notes

**Input Length Guidelines:**
- ✅ **5-7 digits**: Instant to few seconds
- ✅ **8 digits**: Several minutes (acceptable)
- ❌ **9 digits**: Computationally infeasible - optimization in progress

*For 9-digit inputs, the solution space grows exponentially making exhaustive search impractical with the current algorithm.*
