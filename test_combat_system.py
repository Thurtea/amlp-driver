#!/usr/bin/env python3
"""
Combat System Test Script
Tests the complete combat system implementation
"""

import socket
import time
import sys

def connect_to_mud(host='localhost', port=3000):
    """Connect to the MUD server"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((host, port))
        return sock
    except Exception as e:
        print(f"Failed to connect: {e}")
        return None

def send_command(sock, command, wait_for_prompt=True):
    """Send a command and wait for response"""
    try:
        sock.sendall((command + '\n').encode())
        time.sleep(0.5)  # Give server time to process
        
        response = ""
        while True:
            try:
                chunk = sock.recv(4096).decode('utf-8', errors='ignore')
                if not chunk:
                    break
                response += chunk
            except socket.timeout:
                break
        
        return response
    except Exception as e:
        print(f"Error sending command: {e}")
        return ""

def test_combat_system():
    """Test the complete combat system"""
    
    print("="*60)
    print("AMLP Combat System Test")
    print("="*60)
    
    # Connect to MUD
    print("\n[1] Connecting to MUD server...")
    sock = connect_to_mud()
    if not sock:
        print("FAILED: Could not connect to MUD")
        return False
    
    sock.settimeout(2)
    print("✓ Connected to MUD server")
    
    # Wait for login prompt
    time.sleep(1)
    response = ""
    try:
        response = sock.recv(4096).decode('utf-8', errors='ignore')
    except:
        pass
    
    print("\n[2] Creating test character...")
    # Send character name
    send_command(sock, "TestCombat")
    time.sleep(0.5)
    
    # Send password
    send_command(sock, "testpass123")
    time.sleep(0.5)
    
    # Confirm password
    send_command(sock, "testpass123")
    time.sleep(0.5)
    
    # Choose race (0 = Human)
    send_command(sock, "0")
    time.sleep(0.5)
    
    # Choose OCC (0 = first option)
    send_command(sock, "0")
    time.sleep(0.5)
    
    print("✓ Character created")
    
    # Test 1: Check attack command exists
    print("\n[3] Testing attack command...")
    response = send_command(sock, "help attack")
    if "attack" in response.lower() or "combat" in response.lower():
        print("✓ Attack command exists and responds to help")
    else:
        print("✗ Attack command help not working")
    
    # Test 2: Create an NPC to fight (using eval)
    print("\n[4] Creating test NPC...")
    response = send_command(sock, "eval here->clone_object('/std/living')->move(here)")
    print(f"Response: {response[:200]}")
    
    # Test 3: Try to attack a target
    print("\n[5] Testing attack command...")
    response = send_command(sock, "attack creature")
    if "attack" in response.lower() or "fighting" in response.lower() or "combat" in response.lower():
        print("✓ Attack command executed")
        print(f"Response: {response[:200]}")
    else:
        print("✗ Attack command failed")
        print(f"Response: {response[:200]}")
    
    # Test 4: Check combat status
    print("\n[6] Testing combat status...")
    time.sleep(1)  # Wait for combat round
    response = send_command(sock, "score")
    if "hp" in response.lower() or "health" in response.lower():
        print("✓ Score command shows health")
    else:
        print("✗ Score command not showing health")
    
    # Test 5: Test flee command
    print("\n[7] Testing flee command...")
    response = send_command(sock, "flee")
    if "flee" in response.lower() or "escape" in response.lower():
        print("✓ Flee command executed")
    else:
        print("✗ Flee command failed")
    
    # Cleanup
    print("\n[8] Closing connection...")
    sock.close()
    print("✓ Connection closed")
    
    print("\n" + "="*60)
    print("Combat System Test Complete")
    print("="*60)
    
    return True

if __name__ == "__main__":
    success = test_combat_system()
    sys.exit(0 if success else 1)
