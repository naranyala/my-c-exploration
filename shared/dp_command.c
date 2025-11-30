/**
 * Command pattern implementation for undo/redo functionality
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMANDS 10

typedef struct Command Command;

typedef struct {
  void (*execute)(Command *cmd);
  void (*undo)(Command *cmd);
  void (*destroy)(Command *cmd);
} CommandVTable;

struct Command {
  CommandVTable vtable;
  char description[64];
};

typedef struct {
  Command *commands[MAX_COMMANDS];
  int top;
  int capacity;
} CommandHistory;

// Concrete command: Add operation
typedef struct {
  Command base;
  int *value;
  int operand;
} AddCommand;

void add_execute(Command *cmd) {
  AddCommand *add_cmd = (AddCommand *)cmd;
  *(add_cmd->value) += add_cmd->operand;
  printf("Executed: %s (result: %d)\n", cmd->description, *(add_cmd->value));
}

void add_undo(Command *cmd) {
  AddCommand *add_cmd = (AddCommand *)cmd;
  *(add_cmd->value) -= add_cmd->operand;
  printf("Undone: %s (result: %d)\n", cmd->description, *(add_cmd->value));
}

void add_destroy(Command *cmd) { free(cmd); }

Command *create_add_command(int *value, int operand, const char *description) {
  AddCommand *cmd = malloc(sizeof(AddCommand));
  if (!cmd)
    return NULL;

  cmd->base.vtable.execute = add_execute;
  cmd->base.vtable.undo = add_undo;
  cmd->base.vtable.destroy = add_destroy;
  strncpy(cmd->base.description, description,
          sizeof(cmd->base.description) - 1);
  cmd->value = value;
  cmd->operand = operand;

  return (Command *)cmd;
}

CommandHistory *command_history_create(int capacity) {
  CommandHistory *history = malloc(sizeof(CommandHistory));
  if (!history)
    return NULL;

  history->top = -1;
  history->capacity = capacity;
  return history;
}

void command_history_push(CommandHistory *history, Command *cmd) {
  if (history->top >= history->capacity - 1) {
    // Remove oldest command
    history->commands[0]->vtable.destroy(history->commands[0]);
    for (int i = 0; i < history->top; i++) {
      history->commands[i] = history->commands[i + 1];
    }
    history->top--;
  }

  history->commands[++history->top] = cmd;
}

Command *command_history_pop(CommandHistory *history) {
  if (history->top < 0)
    return NULL;
  return history->commands[history->top--];
}

void command_history_destroy(CommandHistory *history) {
  for (int i = 0; i <= history->top; i++) {
    history->commands[i]->vtable.destroy(history->commands[i]);
  }
  free(history);
}

// Example usage
int main(void) {
  int value = 10;
  CommandHistory *history = command_history_create(5);

  printf("Initial value: %d\n", value);

  // Execute commands
  Command *cmd1 = create_add_command(&value, 5, "Add 5");
  cmd1->vtable.execute(cmd1);
  command_history_push(history, cmd1);

  Command *cmd2 = create_add_command(&value, 3, "Add 3");
  cmd2->vtable.execute(cmd2);
  command_history_push(history, cmd2);

  Command *cmd3 = create_add_command(&value, 7, "Add 7");
  cmd3->vtable.execute(cmd3);
  command_history_push(history, cmd3);

  // Undo last command
  Command *last_cmd = command_history_pop(history);
  if (last_cmd) {
    last_cmd->vtable.undo(last_cmd);
    last_cmd->vtable.destroy(last_cmd);
  }

  printf("Final value: %d\n", value);

  command_history_destroy(history);
  return 0;
}
