#pragma once




enum class tactic_target
{
    nil,
    self,
    enemy,
    ally
};




enum class tactic_condition
{
    nil,
    any,
    adjacent,
};




enum class tactic_action
{
    nil,
    wait,
    move,
    attack
};




typedef struct {
    tactic_target target;
    tactic_condition condition;
    tactic_action action;
} tactic;
