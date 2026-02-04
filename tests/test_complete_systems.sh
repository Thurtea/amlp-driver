#!/bin/bash
# Complete test script for currency, shops, and wizard commands

echo "=========================================="
echo "   AMLP DRIVER - COMPLETE FEATURE TEST"
echo "=========================================="
echo ""

cat << 'EOF'
This script provides commands to test:
1. Currency system (UC and BMC)
2. Shop system (buying/selling)
3. Wizard commands (eval, goto, force)
4. Prompt customization

===========================================
PRE-REQUISITES:
===========================================

1. Start the driver:
   cd /home/thurtea/amlp-driver
   ./mud.ctl start

2. Connect via telnet:
   telnet localhost 3000

3. Create a character and ensure you have wizard privileges
   (First player gets admin automatically)

===========================================
TEST SEQUENCE:
===========================================

--- WIZARD COMMANDS TEST ---

1. Test eval command (give yourself credits):
   eval me->set_universal_credits(10000)
   eval me->set_black_market_credits(5000)

2. Check the result:
   eval me->query_universal_credits()
   (Should show: 10000)

3. Test another eval:
   eval here->query_short()
   (Should show current room name)

--- CURRENCY SYSTEM TEST ---

4. Check your balance:
   credits
   
   Should display:
   ==========================[ CREDIT BALANCE ]==========================
   Universal Credits (UC):     10000 credits
   Black Market Credits (BMC): 5000 credits  
   ======================================================================

5. Alternative commands:
   money
   balance
   (All should show same info)

--- SHOP SYSTEM TEST ---

6. Go to the General Store:
   east

7. Browse shop inventory:
   list

   Should show:
   - Weapons (vibro-blade, wilks laser, ion blaster)
   - Armor (light, medium, heavy)
   - Supplies (medkit, rations, rope, torch)

8. Buy a vibro-blade (5000 UC):
   buy vibro-blade

   Should deduct 5000 UC from your balance

9. Check inventory:
   i

   Should show the vibro-blade

10. Check credits again:
    credits
    
    Should now show: 5000 UC, 5000 BMC

11. Sell the vibro-blade back:
    sell vibro-blade

    Should give you 2500 UC (50% of buy price)

12. Final credit check:
    credits
    
    Should show: 7500 UC, 5000 BMC

--- TELEPORT TEST ---

13. Go back to village:
    west

14. Test goto command:
    goto /domains/castle/entry

    Should teleport you to the castle

15. Return to village:
    goto /domains/start/village_center

--- PROMPT CUSTOMIZATION TEST ---

16. View current prompt:
    prompt

17. Set a custom prompt with HP/MDC:
    prompt [%h/%H HP %m/%M MDC]

18. Set a prompt with credits:
    prompt [%c UC %b BMC]

19. Set a simple level prompt:
    prompt [Lvl %l]

20. Reset to default:
    prompt default

--- MEDKIT TEST ---

21. Go to shop and buy medkit:
    east
    buy medkit
    west

22. Take some damage (if in combat or via eval):
    eval me->set_hp(5)

23. Use the medkit:
    use medkit

    Should restore 3d6 HP

===========================================
EXPECTED RESULTS:
===========================================

✓ eval command works without @ prefix
✓ Currency tracks UC and BMC separately
✓ Shop displays inventory with prices
✓ Buy/sell transactions work correctly
✓ Sell-back is 50% of buy price  
✓ goto teleports between rooms
✓ Prompt customizes with status codes
✓ Medkit heals HP/SDC

===========================================
TROUBLESHOOTING:
===========================================

If eval doesn't work:
- Check you have wizard privilege: eval me->query_privilege()
- Make sure you're using "me->" not "@me->"

If shop doesn't work:
- Verify you're in the shop room: look
- Check item exists: list
- Confirm you have enough credits: credits

If teleport doesn't work:
- Check room path is correct
- Try: goto /domains/start/village_center

===========================================
ADDITIONAL TESTS (Optional):
===========================================

Test force command (admin only):
   force <playername> say I am being forced!

Test complex eval:
   eval environment(me)->set_long("New room description!")

Test array eval:
   eval all_inventory(me)

Test prompt with multiple codes:
   prompt [%n L%l | %h/%H HP | %c UC]

===========================================

After completing these tests, all core systems
should be verified as working correctly!

EOF

echo ""
echo "=========================================="
echo "Ready to test? Start the driver and connect!"
echo "=========================================="
