### Suggested Live Changes

- **Trigger the capture error (Q3 rule):** delete `[count]` from `byValue`'s capture list (change it to `[]`) and recompile — the "use of `count` not captured" error is a good concrete anchor for "a lambda may use a local variable only if it captures it."
- **Remove `mutable` (Q5):** delete it from `counter` and show the `const`-violation error; then do the same thing to `SquareFO::operator()` by trying to add a mutating statement without removing `const`, to show it's the identical rule.
- **Turn the safe return into a dangling one (Q2):** change `makeMultiplier` to capture `[&factor]` instead of `[factor]`. Calling `triple(7)` afterward is now undefined behavior — a live, visible instance of Q3. Flag clearly that this is *intentionally* broken code, not a demo of correct style.
- **Flip the `bind` placeholders (Q4):** change `bind(subtract, _2, _1)` to `bind(subtract, _1, _2)` and watch the result go from `-7` to `7`.
- **Swap the default capture mode:** change `[=]` in the `for_each` call to `[&]` — behavior is unchanged here (nothing is mutated), which is itself a useful discussion point: *when* does the choice between `[=]` and `[&]` actually matter versus just being a style preference?
