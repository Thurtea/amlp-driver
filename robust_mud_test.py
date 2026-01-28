#!/usr/bin/env python3
"""
Robust MUD Testing Script
Handles interactive prompts properly and verifies wiztool functionality
"""

import socket
import time
import sys
import re

class MUDTester:
    def __init__(self, host='localhost', port=3000):
        self.host = host
        self.port = port
        self.sock = None
        self.buffer = ""
        
    def connect(self):
        """Connect to MUD server"""
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(2.0)
        try:
            self.sock.connect((self.host, self.port))
            time.sleep(0.5)  # Wait for welcome banner
            return True
        except Exception as e:
            print(f"Connection failed: {e}")
            return False
    
    def disconnect(self):
        """Disconnect from server"""
        if self.sock:
            try:
                self.sock.close()
            except:
                pass
            self.sock = None
    
    def receive_until(self, pattern, timeout=5.0):
        """Receive data until pattern is found or timeout"""
        self.buffer = ""
        start_time = time.time()
        
        while time.time() - start_time < timeout:
            try:
                data = self.sock.recv(4096)
                if not data:
                    break
                
                text = data.decode('utf-8', errors='ignore')
                self.buffer += text
                
                # Check if we found the pattern
                if re.search(pattern, self.buffer, re.IGNORECASE | re.MULTILINE):
                    return self.buffer
                    
            except socket.timeout:
                continue
            except Exception as e:
                print(f"Receive error: {e}")
                break
        
        return self.buffer
    
    def send_line(self, text):
        """Send a line of text"""
        try:
            self.sock.sendall((text + "\n").encode('utf-8'))
            time.sleep(0.3)  # Give server time to process
        except Exception as e:
            print(f"Send error: {e}")
    
    def login(self, username, password):
        """Complete login sequence"""
        print(f"\n{'='*60}")
        print(f"LOGGING IN AS: {username}")
        print(f"{'='*60}\n")
        
        # Wait for name prompt
        output = self.receive_until(r"Enter your name:")
        print(output)
        
        # Send username
        self.send_line(username)
        
        # Check if new user or existing
        output = self.receive_until(r"(Choose a password:|Enter your password:)")
        print(output)
        
        if "Choose a password:" in output:
            # New user - set password
            self.send_line(password)
            time.sleep(0.2)
            
            # Confirm password
            output = self.receive_until(r"Confirm password:")
            print(output)
            self.send_line(password)
            
            # Wait for character creation
            output = self.receive_until(r"(materialize|room)", timeout=3.0)
            print(output)
            
        else:
            # Existing user
            self.send_line(password)
            output = self.receive_until(r"materialize", timeout=3.0)
            print(output)
        
        # Wait a moment for any additional messages (like wiztool attachment)
        time.sleep(0.5)
        try:
            extra = self.sock.recv(4096).decode('utf-8', errors='ignore')
            if extra:
                print(extra)
        except:
            pass
        
        return True
    
    def send_command(self, cmd, wait_for=None):
        """Send a command and wait for output"""
        print(f"\n> {cmd}")
        self.send_line(cmd)
        
        if wait_for:
            output = self.receive_until(wait_for, timeout=2.0)
        else:
            time.sleep(0.5)
            try:
                output = self.sock.recv(4096).decode('utf-8', errors='ignore')
            except:
                output = ""
        
        print(output)
        return output
    
    def test_wiztool_commands(self):
        """Test essential wiztool commands"""
        tests = [
            ("wiz help", "Wiztool Commands"),
            ("pwd", "Current directory:"),
            ("ls", "Contents of"),
            ("cd /lib", "Changed directory"),
            ("pwd", "/lib"),
            ("ls", "std"),
            ("cd std", "Changed directory"),
            ("ls", "player"),
            ("eval 2 + 2", "int: 4"),
        ]
        
        results = []
        for cmd, expected in tests:
            output = self.send_command(cmd)
            success = expected.lower() in output.lower()
            results.append((cmd, success))
            print(f"  {'✓' if success else '✗'} Expected: {expected}")
        
        return results
    
    def test_standard_commands(self):
        """Test standard MUD commands"""
        tests = [
            ("help", "commands"),
            ("look", ""),
            ("who", "player"),
            ("stats", ""),
        ]
        
        results = []
        for cmd, expected in tests:
            output = self.send_command(cmd)
            success = expected == "" or expected.lower() in output.lower()
            results.append((cmd, success))
            if expected:
                print(f"  {'✓' if success else '✗'} Expected: {expected}")
        
        return results

def main():
    """Run comprehensive tests"""
    
    print("="*70)
    print(" AMLP MUD - COMPREHENSIVE TEST SUITE")
    print("="*70)
    
    # Test 1: Admin User (first player)
    print("\n" + "="*70)
    print(" TEST 1: ADMIN USER (First Player)")
    print("="*70)
    
    admin = MUDTester()
    if not admin.connect():
        print("❌ Failed to connect to server")
        return 1
    
    try:
        admin.login("TestAdmin", "admin123")
        
        print("\n" + "-"*70)
        print(" Testing Standard Commands")
        print("-"*70)
        std_results = admin.test_standard_commands()
        
        print("\n" + "-"*70)
        print(" Testing Wiztool Commands")
        print("-"*70)
        wiz_results = admin.test_wiztool_commands()
        
        # Quit
        admin.send_command("quit")
        
    finally:
        admin.disconnect()
    
    time.sleep(2)
    
    # Test 2: Normal Player (second player)
    print("\n" + "="*70)
    print(" TEST 2: NORMAL PLAYER (Second Player)")
    print("="*70)
    
    player = MUDTester()
    if not player.connect():
        print("❌ Failed to connect to server")
        return 1
    
    try:
        player.login("TestPlayer", "player123")
        
        print("\n" + "-"*70)
        print(" Testing Standard Commands")
        print("-"*70)
        std_results2 = player.test_standard_commands()
        
        print("\n" + "-"*70)
        print(" Testing Wiztool Access (Should Fail)")
        print("-"*70)
        output = player.send_command("wiz help")
        
        if "Unknown command" in output or "wiztool" not in output.lower():
            print("  ✓ Correctly denied wiztool access")
        else:
            print("  ✗ ERROR: Normal player has wiztool access!")
        
        output = player.send_command("cd /lib")
        if "Unknown command" in output or "Changed directory" not in output:
            print("  ✓ Correctly denied cd command")
        else:
            print("  ✗ ERROR: Normal player can use cd!")
        
        # Quit
        player.send_command("quit")
        
    finally:
        player.disconnect()
    
    # Summary
    print("\n" + "="*70)
    print(" TEST SUMMARY")
    print("="*70)
    
    print("\nAdmin Wiztool Commands:")
    for cmd, success in wiz_results:
        print(f"  {'✓' if success else '✗'} {cmd}")
    
    admin_success = sum(1 for _, s in wiz_results if s)
    admin_total = len(wiz_results)
    print(f"\nAdmin Tests: {admin_success}/{admin_total} passed")
    
    if admin_success == admin_total:
        print("\n✅ ALL TESTS PASSED - Wiztool is working!")
        return 0
    else:
        print(f"\n⚠️  SOME TESTS FAILED - {admin_total - admin_success} issues found")
        return 1

if __name__ == "__main__":
    sys.exit(main())
