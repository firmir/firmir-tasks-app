# firmir-tasks

Tasks + notes in one window. Tasks: a checkbox list with a text input
(Enter or "add task"); each row has a delete button. Notes: free multiline
text. Everything is saved to `~/.local/share/firmir-tasks/store.txt` on every
change.

```
cmake -S . -B build && cmake --build build -j$(nproc)

Firmir is located via pkg-config (installed `firmir-*.pc` files); if not
found there, the sibling `../firmir` checkout is configured and built.
./build/firmir-tasks
```

The text input (`EditableText`) is a minimal firmir-gui item:
click to focus, UTF-8 text input, Backspace, Enter.
