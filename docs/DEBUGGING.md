# Debugging

Run `make debug`; QEMU pauses at startup and exposes a GDB stub on TCP port 1234. In another terminal:

```gdb
gdb build/kyronos.kernel
(gdb) target remote :1234
(gdb) continue
```

Use `break kmain` to stop at kernel entry. The kernel is linked without a host runtime, so inspect memory and registers directly when diagnosing boot failures.
