#include "controllablesystem.h"
ENGINE_NAMESPACE_BEGIN

//
// SYSTEM
////////////////////////////////////////

void ControllableSystem::update( Entity& entity )
{
    if ( !entity.has_component( ctype_Controllable ) )
        return;

    Controllable& creature = (Controllable&) entity.get_component( ctype_Controllable );

    process_cmds( creature );

    if ( creature.moveUp    ) 
        entity.position.y += creature.moveSpeed;
    if ( creature.moveDown  ) 
        entity.position.y -= creature.moveSpeed;
    if ( creature.moveLeft  ) 
        entity.position.x -= creature.moveSpeed;
    if ( creature.moveRight ) 
        entity.position.x += creature.moveSpeed;
}

void ControllableSystem::create_snapshot_full( Entity& dest, Entity& src)
{
    // 1# Find Component
    if ( !src.has_component( ctype_Controllable ) )
        return;

    Controllable& cSrc = (Controllable&) src.get_component( ctype_Controllable );

    // 2# Create Snapshot
    unique<Controllable> cDest = make_unique<Controllable>();

    cDest->health     = cSrc.health;
    cDest->stamina    = cSrc.stamina;
    cDest->name       = cSrc.name;
    cDest->moveSpeed  = cSrc.moveSpeed;

    dest.add_component( std::move( cDest ) );
}

void ControllableSystem::process_cmds( Controllable& controlable )
{
    for ( auto& command : controlable.commandQ )
        if ( !(command->consumed) ) {
            if ( command->type == MOVE_UP ) {
                controlable.moveUp = ((unique<CmdMove>&) command)->started;
            }
            if ( command->type == MOVE_DOWN ) {
                controlable.moveDown = ((unique<CmdMove>&) command)->started;
            }
            if ( command->type == MOVE_LEFT ) {
                controlable.moveLeft = ((unique<CmdMove>&) command)->started;
            }
            if ( command->type == MOVE_RIGHT ) {
                controlable.moveRight = ((unique<CmdMove>&) command)->started;
            }
        }

    controlable.commandQ.clear();
}

//
// ADDITIONAL
////////////////////////////////////////

Command::Command() : type(UNKOWN), consumed(false) {}

CmdMove::CmdMove() : Command(), started(false) {}


ENGINE_NAMESPACE_END

