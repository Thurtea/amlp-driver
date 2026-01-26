// Movement command

int execute(string args, object player) {
    object room, dest;
    string dest_path;
    
    if (!args || args == "") {
        tell_object(player, "Go where?\n");
        return 1;
    }
    
    room = player->query_environment();
    if (!room) {
        tell_object(player, "You are nowhere.\n");
        return 1;
    }
    
    dest_path = room->query_exit(args);
    if (!dest_path) {
        tell_object(player, "You can't go that way.\n");
        return 1;
    }
    
    dest = load_object(dest_path);
    if (!dest) {
        tell_object(player, "That destination doesn't exist.\n");
        return 1;
    }
    
    tell_object(player, "You go " + args + ".\n");
    player->move(dest);
    tell_object(player, dest->look());
    
    return 1;
}
