AMLP MUD — Work Summary (2026-01-28)

Files changed:
- lib/clone/user.lpc
  - Rewrote `process_input()` to call `COMMAND_D->execute_command(this_object(), input)` first and fall back to legacy/local commands.
  - Changed `query_wiztool()` to return the wiztool object (`object query_wiztool()`) instead of a boolean.

- lib/clone/login.lpc
  - Fixed first-player detection check (use `creating_new == 1`).
  - Added debug logging when granting Admin.
  - After granting Admin, force a second `set_wizard_level(ADMIN_LEVEL)` attempt if `query_wiztool()` did not produce an object; log results to `/log/server.log`.

- lib/std/wiztool.lpc
  - Added debug writes to `/log/server.log` in `attach()` to record attach attempts and success.

- lib/daemon/command.lpc
  - Temporarily added debug logging around wiztool lookup and `process_command()` result; logs are written to `/log/server.log` as `[DEBUG command]` entries.

- QUICK_TEST.md
  - Added a quick test and port-fix guide at project root.

What I ran and why:
- Freed port 3000, restarted the driver repeatedly to ensure a clean environment.
- Removed test player saves to allow first-player promotion testing.
- Ran the automated test (`robust_mud_test.py`) several times while iterating on fixes.
- Added logging so we can inspect whether `attach()` is actually called and whether `COMMAND_D` sees a wiztool object.

Key findings so far:
- The login flow now promotes the first-created player to Admin (`Privilege Level: 2`) when saves are removed.
- `wiztool.attach()` was not being observed in the logs originally; we added debug output to confirm attachment.
- After forcing an attach attempt in `enter_game()` (re-invoking `set_wizard_level`), the server logs indicate the first-player creation and subsequent command routing attempts. If attach still does not appear, it suggests the attach logic in `set_wizard_level()` may be failing silently.

Next steps to continue tomorrow:
1. Inspect `/home/thurtea/amlp-driver/lib/log/server.log` for `[DEBUG wiztool]` and `[DEBUG command]` entries produced during the automated test runs.
2. If `attach()` debug entries are absent, instrument `set_wizard_level()` in `lib/clone/user.lpc` to log its attempt to clone and attach the wiztool (write to `/log/server.log`).
3. If `attach()` entries appear but `COMMAND_D` still reports no wiztool, add a small accessor in `user.lpc` named `attach_wiztool(object wt)` to let `login.lpc` set the `_wiztool` field directly after cloning.
4. When the above confirms `wiztool` is attached and `COMMAND_D` sees it, remove temporary debug writes.

How to reproduce locally (commands used):

```bash
# Ensure port is free
pkill -9 -f 'build/driver' || true
sleep 1
ss -tln | grep :3000 || true

# Remove test saves
rm -f ~/amlp-driver/save/players/Admin.o ~/amlp-driver/save/players/Testplayer.o ~/amlp-driver/save/players/TestAdmin.o ~/amlp-driver/save/players/TestPlayer.o || true

# Start server and capture logs
cd ~/amlp-driver
./build/driver 3000 > /tmp/driver.log 2>&1 &

# Run automated test
python3 robust_mud_test.py 2>&1 | tee /tmp/auto_test.log

# Inspect server log for debug tags
tail -200 lib/log/server.log | egrep "DEBUG wiztool|DEBUG command|First player"
```

Notes & Warnings:
- Several temporary debug writes were added to `/log/server.log` (search for `DEBUG wiztool` and `DEBUG command`). Remove them once resolved.
- `user.lpc` currently returns the wiztool object from `query_wiztool()` — this is required by `COMMAND_D`.

If you want, tomorrow I can:
- Add `attach_wiztool()` helper on `user.lpc` to make attaching from `login.lpc` explicit.
- Clean up debug logs and finalize the implementation.

-- End of notes
