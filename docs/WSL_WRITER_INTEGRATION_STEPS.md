# Integration Steps for wsl_writer.rs

## 1. Add module declaration to src-tauri/src/main.rs

After the existing module declarations (look for lines like `mod config;`, `mod commands;`, etc.), add:

```rust
mod wsl_writer;
```

## 2. Verify the module compiles

Run from PowerShell in the Windows project directory:

```powershell
cd E:\Work\lpc-development-assistant\src-tauri
cargo check
```

This will verify wsl_writer.rs compiles correctly with the rest of the project.

## 3. Optional: Export functions as Tauri commands

If you want to expose the wsl_writer functions directly to the frontend, add these command wrappers to src-tauri/src/commands/mod.rs or create a new commands file:

```rust
use crate::wsl_writer::{save_driver_file, save_library_file};

#[tauri::command]
pub fn save_driver_code(filename: String, content: String, is_header: bool) -> Result<String, String> {
    let distro = crate::commands::file_operations::get_default_wsl_distro()?;
    save_driver_file(&distro, &filename, &content, is_header)
}

#[tauri::command]
pub fn save_library_code(category: String, domain: Option<String>, filename: String, content: String) -> Result<String, String> {
    let distro = crate::commands::file_operations::get_default_wsl_distro()?;
    save_library_file(&distro, &category, domain.as_deref(), &filename, &content)
}
```

Then register these commands in main.rs invoke_handler:

```rust
.invoke_handler(tauri::generate_handler![
    // ... existing commands ...
    save_driver_code,
    save_library_code,
])
```

## 4. Test the integration

After integration, test with a simple call from the Tauri app to verify file writing works correctly.

---
Generated: 2026-01-26
Reference: WINDOWS_FILE_PLACEMENT_GUIDE.md
