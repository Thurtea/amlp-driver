# Windows Assistant Integration Guide
**For lpc-development-assistant -> amlp-driver integration**

## Overview

This document describes how to integrate the Windows-based `lpc-development-assistant` (Rust/Tauri) with the Linux-based `amlp-driver` using WSL.

## Architecture

```
+=============================================+
|  Windows (lpc-development-assistant)        |
|  |= Rust Backend (Tauri)                    |
|  |= RAG System (LPC knowledge base)         |
|  |= Code Generator (Claude API)             |
|  |= File Watcher                             |
|============+================================+
             | WSL Commands via std::process
             ?
+=============================================+
|  WSL/Linux (amlp-driver)                    |
|  |= Compiler (lexer -> parser -> codegen)    |
|  |= VM Executor                              |
|  |= CLI Interface                            |
|============+================================+
             | File I/O
             ?
+=============================================+
|  WSL Filesystem (amlp-library)              |
|  |= master.c                                 |
|  |= std/ (base objects)                      |
|  |= domains/ (game content)                  |
|=============================================+
```

## WSL Integration Module (Rust)

### Module Structure

```rust
// src/wsl/mod.rs
pub mod driver;
pub mod filesystem;
pub mod output;

// src/wsl/driver.rs
use std::process::{Command, Output};
use std::path::PathBuf;

pub struct AmlpDriver {
    driver_path: String,
    library_path: String,
}

impl AmlpDriver {
    pub fn new() -> Self {
        Self {
            driver_path: "/home/thurtea/amlp-driver".to_string(),
            library_path: "/home/thurtea/amlp-library".to_string(),
        }
    }
    
    pub fn compile(&self, file_path: &str) -> Result<CompileResult, DriverError> {
        let wsl_path = self.to_wsl_path(file_path)?;
        let output = self.execute_command(&["compile", &wsl_path])?;
        CompileResult::parse(output)
    }
    
    pub fn show_ast(&self, file_path: &str) -> Result<String, DriverError> {
        let wsl_path = self.to_wsl_path(file_path)?;
        let output = self.execute_command(&["ast", &wsl_path])?;
        Ok(String::from_utf8_lossy(&output.stdout).to_string())
    }
    
    pub fn show_bytecode(&self, file_path: &str) -> Result<String, DriverError> {
        let wsl_path = self.to_wsl_path(file_path)?;
        let output = self.execute_command(&["bytecode", &wsl_path])?;
        Ok(String::from_utf8_lossy(&output.stdout).to_string())
    }
    
    fn execute_command(&self, args: &[&str]) -> Result<Output, DriverError> {
        let driver_exec = format!("{}/build/driver", self.driver_path);
        let cmd = format!(
            "cd {} && {} {}",
            self.driver_path,
            driver_exec,
            args.join(" ")
        );
        
        let output = Command::new("wsl")
            .args(&["bash", "-c", &cmd])
            .output()
            .map_err(|e| DriverError::ExecutionFailed(e.to_string()))?;
        
        Ok(output)
    }
    
    fn to_wsl_path(&self, windows_path: &str) -> Result<String, DriverError> {
        // Convert Windows path to WSL path
        // C:\Users\... -> /mnt/c/Users/...
        // Or use library path for relative paths
        if windows_path.starts_with("C:") || windows_path.starts_with("c:") {
            let path = windows_path.replace("\\", "/").replace("C:", "/mnt/c");
            Ok(path)
        } else {
            Ok(format!("{}/{}", self.library_path, windows_path))
        }
    }
}

#[derive(Debug)]
pub enum DriverError {
    ExecutionFailed(String),
    CompilationFailed(Vec<CompileError>),
    FileNotFound(String),
    InvalidOutput(String),
}

#[derive(Debug, Clone)]
pub struct CompileResult {
    pub success: bool,
    pub errors: Vec<CompileError>,
    pub warnings: Vec<CompileWarning>,
    pub functions: Vec<String>,
    pub bytecode_size: usize,
}

#[derive(Debug, Clone)]
pub struct CompileError {
    pub file: String,
    pub line: usize,
    pub column: usize,
    pub message: String,
    pub context: Option<String>,
}

#[derive(Debug, Clone)]
pub struct CompileWarning {
    pub file: String,
    pub line: usize,
    pub message: String,
}

impl CompileResult {
    pub fn parse(output: Output) -> Result<Self, DriverError> {
        let stdout = String::from_utf8_lossy(&output.stdout);
        let stderr = String::from_utf8_lossy(&output.stderr);
        
        if output.status.success() {
            // Parse success output
            Ok(Self::parse_success(&stdout))
        } else {
            // Parse error output
            Ok(Self::parse_errors(&stderr))
        }
    }
    
    fn parse_success(output: &str) -> Self {
        // Parse "Compilation successful!" output
        // Extract function names and bytecode size
        let functions = Self::extract_functions(output);
        let bytecode_size = Self::extract_bytecode_size(output);
        
        Self {
            success: true,
            errors: vec![],
            warnings: vec![],
            functions,
            bytecode_size,
        }
    }
    
    fn parse_errors(output: &str) -> Self {
        // Parse error output with line/column info
        let errors = Self::extract_errors(output);
        let warnings = Self::extract_warnings(output);
        
        Self {
            success: false,
            errors,
            warnings,
            functions: vec![],
            bytecode_size: 0,
        }
    }
    
    fn extract_functions(output: &str) -> Vec<String> {
        // Extract function names from output
        // Look for "Functions: main, create, query_name"
        if let Some(line) = output.lines().find(|l| l.starts_with("Functions:")) {
            line.strip_prefix("Functions: ")
                .unwrap_or("")
                .split(',')
                .map(|s| s.trim().to_string())
                .collect()
        } else {
            vec![]
        }
    }
    
    fn extract_bytecode_size(output: &str) -> usize {
        // Extract bytecode size from "Bytecode size: 24 bytes"
        if let Some(line) = output.lines().find(|l| l.starts_with("Bytecode size:")) {
            line.split_whitespace()
                .nth(2)
                .and_then(|s| s.parse().ok())
                .unwrap_or(0)
        } else {
            0
        }
    }
    
    fn extract_errors(output: &str) -> Vec<CompileError> {
        // Parse error format:
        // Error: Undefined variable 'x'
        //   File: /path/to/file.c
        //   Line: 5, Column: 12
        //   Context: int y = x + 1;
        let mut errors = vec![];
        let lines: Vec<&str> = output.lines().collect();
        
        for (i, line) in lines.iter().enumerate() {
            if line.starts_with("Error:") {
                let message = line.strip_prefix("Error: ").unwrap_or(line).to_string();
                let file = Self::extract_field(&lines, i, "File:");
                let (line_num, column) = Self::extract_line_column(&lines, i);
                let context = Self::extract_field(&lines, i, "Context:");
                
                errors.push(CompileError {
                    file,
                    line: line_num,
                    column,
                    message,
                    context: if context.is_empty() { None } else { Some(context) },
                });
            }
        }
        
        errors
    }
    
    fn extract_warnings(output: &str) -> Vec<CompileWarning> {
        // Parse warning format similar to errors
        vec![] // TODO: Implement if driver adds warnings
    }
    
    fn extract_field(lines: &[&str], start_idx: usize, prefix: &str) -> String {
        lines.iter()
            .skip(start_idx + 1)
            .take(5)
            .find(|l| l.trim().starts_with(prefix))
            .and_then(|l| l.strip_prefix(prefix))
            .map(|s| s.trim().to_string())
            .unwrap_or_default()
    }
    
    fn extract_line_column(lines: &[&str], start_idx: usize) -> (usize, usize) {
        if let Some(line) = lines.iter()
            .skip(start_idx + 1)
            .take(5)
            .find(|l| l.contains("Line:")) {
            
            let parts: Vec<&str> = line.split(',').collect();
            let line_num = parts.get(0)
                .and_then(|s| s.split(':').nth(1))
                .and_then(|s| s.trim().parse().ok())
                .unwrap_or(0);
            let column = parts.get(1)
                .and_then(|s| s.split(':').nth(1))
                .and_then(|s| s.trim().parse().ok())
                .unwrap_or(0);
            
            (line_num, column)
        } else {
            (0, 0)
        }
    }
}
```

### File System Operations

```rust
// src/wsl/filesystem.rs
use std::fs;
use std::path::PathBuf;

pub struct WslFilesystem {
    library_root: PathBuf,
}

impl WslFilesystem {
    pub fn new(library_root: PathBuf) -> Self {
        Self { library_root }
    }
    
    pub fn write_lpc_file(&self, relative_path: &str, content: &str) -> Result<(), std::io::Error> {
        let full_path = self.library_root.join(relative_path);
        
        // Create parent directories if needed
        if let Some(parent) = full_path.parent() {
            fs::create_dir_all(parent)?;
        }
        
        fs::write(full_path, content)
    }
    
    pub fn read_lpc_file(&self, relative_path: &str) -> Result<String, std::io::Error> {
        let full_path = self.library_root.join(relative_path);
        fs::read_to_string(full_path)
    }
    
    pub fn list_lpc_files(&self) -> Result<Vec<String>, std::io::Error> {
        let mut files = vec![];
        self.scan_directory(&self.library_root, &mut files)?;
        Ok(files)
    }
    
    fn scan_directory(&self, dir: &PathBuf, files: &mut Vec<String>) -> Result<(), std::io::Error> {
        for entry in fs::read_dir(dir)? {
            let entry = entry?;
            let path = entry.path();
            
            if path.is_dir() {
                self.scan_directory(&path, files)?;
            } else if path.extension().and_then(|s| s.to_str()) == Some("c") {
                if let Ok(relative) = path.strip_prefix(&self.library_root) {
                    files.push(relative.to_string_lossy().to_string());
                }
            }
        }
        Ok(())
    }
}
```

### Output Formatting

```rust
// src/wsl/output.rs
use regex::Regex;

pub struct OutputFormatter;

impl OutputFormatter {
    pub fn strip_ansi_codes(text: &str) -> String {
        let re = Regex::new(r"\x1b\[[0-9;]*m").unwrap();
        re.replace_all(text, "").to_string()
    }
    
    pub fn format_error_for_display(error: &CompileError) -> String {
        format!(
            "{}:{} - {}\n  {}",
            error.file,
            error.line,
            error.message,
            error.context.as_ref().unwrap_or(&String::new())
        )
    }
    
    pub fn format_compile_result(result: &CompileResult) -> String {
        if result.success {
            format!(
                "[PASS] Compilation successful\n  Functions: {}\n  Bytecode: {} bytes",
                result.functions.join(", "),
                result.bytecode_size
            )
        } else {
            format!(
                "? Compilation failed\n  {} error(s)\n\n{}",
                result.errors.len(),
                result.errors.iter()
                    .map(Self::format_error_for_display)
                    .collect::<Vec<_>>()
                    .join("\n\n")
            )
        }
    }
}
```

## Usage in lpc-development-assistant

### Tauri Command Example

```rust
// src-tauri/src/commands.rs
use crate::wsl::driver::{AmlpDriver, CompileResult};

#[tauri::command]
pub async fn compile_lpc_code(file_path: String) -> Result<CompileResult, String> {
    let driver = AmlpDriver::new();
    driver.compile(&file_path)
        .map_err(|e| format!("Compilation error: {:?}", e))
}

#[tauri::command]
pub async fn generate_and_compile(
    prompt: String,
    target_file: String
) -> Result<CompileResult, String> {
    // 1. Generate code using Claude API + RAG
    let generated_code = generate_code_from_prompt(&prompt).await?;
    
    // 2. Write to file in amlp-library
    let filesystem = WslFilesystem::new(PathBuf::from("\\\\wsl$\\Ubuntu\\home\\thurtea\\amlp-library"));
    filesystem.write_lpc_file(&target_file, &generated_code)
        .map_err(|e| e.to_string())?;
    
    // 3. Compile with driver
    let driver = AmlpDriver::new();
    let result = driver.compile(&target_file)
        .map_err(|e| format!("Compilation error: {:?}", e))?;
    
    Ok(result)
}
```

### Frontend (Svelte) Example

```typescript
// src/lib/components/CompilePanel.svelte
import { invoke } from '@tauri-apps/api/tauri';

async function compileFile(filePath: string) {
    try {
        const result = await invoke('compile_lpc_code', { filePath });
        
        if (result.success) {
            showSuccess(`Compiled ${result.functions.length} functions`);
        } else {
            showErrors(result.errors);
        }
    } catch (error) {
        showError(`Compilation failed: ${error}`);
    }
}

async function generateAndCompile(prompt: string, targetFile: string) {
    try {
        const result = await invoke('generate_and_compile', { 
            prompt, 
            targetFile 
        });
        
        displayCompileResult(result);
    } catch (error) {
        showError(`Generation/compilation failed: ${error}`);
    }
}
```

## File Path Mappings

### Windows -> WSL Path Conversion

```
Windows                              WSL
-------                              ---
C:\Users\Name\amlp-library\...    -> /mnt/c/Users/Name/amlp-library/...
\\wsl$\Ubuntu\home\thurtea\...    -> /home/thurtea/...
```

### Recommended Approach

Use WSL home directory for library:
- **Windows access:** `\\wsl$\Ubuntu\home\thurtea\amlp-library\`
- **WSL access:** `/home/thurtea/amlp-library/`
- **Driver path:** `/home/thurtea/amlp-driver/`

## Testing the Integration

### Basic Test

```rust
#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_compile_simple_file() {
        let driver = AmlpDriver::new();
        let result = driver.compile("tests/simple.c").unwrap();
        assert!(result.success);
    }
    
    #[test]
    fn test_compile_with_error() {
        let driver = AmlpDriver::new();
        let result = driver.compile("tests/error.c").unwrap();
        assert!(!result.success);
        assert!(result.errors.len() > 0);
    }
}
```

### Integration Test Script

```bash
#!/bin/bash
# test-integration.sh

echo "Testing Windows -> WSL -> Driver integration"

# 1. Create test file
cat > /home/thurtea/amlp-library/test_integration.c << 'EOF'
void create() {
    int x = 42;
}
EOF

# 2. Compile from Windows
echo "Compiling from WSL..."
/home/thurtea/amlp-driver/build/driver compile /home/thurtea/amlp-library/test_integration.c

# 3. Check result
if [ $? -eq 0 ]; then
    echo "[PASS] Integration test passed"
else
    echo "? Integration test failed"
fi
```

## Auto-compilation Workflow

```
User Prompt
    ?
[RAG: Search LPC knowledge base]
    ?
[Claude: Generate LPC code]
    ?
[Write to amlp-library/file.c]
    ?
[WSL: compile file.c]
    ?
+=============+
|  Success?   |
|=====+=======+
      |
  Yes | No
      |  |==-> [Parse errors] -> [Show to user] -> [Iterate]
      ?
[Show success] -> [Update RAG index]
```

## Next Steps

1. Implement WSL module in lpc-development-assistant
2. Test basic compilation from Windows
3. Integrate with RAG system
4. Add file watcher for auto-compilation
5. Build UI for displaying results
6. Test full prompt -> code -> compile pipeline

## References

- Driver CLI: See `./build/driver --help`
- Driver source: `/home/thurtea/amlp-driver/src/driver.c`
- Compilation pipeline: `PHASE7.2_QUICK_REFERENCE.md`
- Error formats: See driver output examples
