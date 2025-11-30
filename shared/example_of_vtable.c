#include <stdio.h>

// Define function signatures
typedef struct AnimalVtbl AnimalVtbl;
struct AnimalVtbl {
  void (*speak)(void *self);
  void (*move)(void *self);
};

typedef struct {
  const AnimalVtbl *vtbl; // Pointer to v-table
  int id;
  char name[50];
} Animal;

// Dog implementations
void dog_speak(void *self) {
  Animal *animal = (Animal *)self;
  printf("%s (Dog) says Woof!\n", animal->name);
}

void dog_move(void *self) { printf("Dog walks\n"); }

// V-table for Dog
static const AnimalVtbl dog_vtable = {.speak = dog_speak, .move = dog_move};

// Initialize a dog
void init_dog(Animal *animal, int id, const char *name) {
  animal->vtbl = &dog_vtable;
  animal->id = id;
  snprintf(animal->name, sizeof(animal->name), "%s", name);
}

int main() {
  Animal my_animal;
  init_dog(&my_animal, 1, "Buddy");

  // Call functions through v-table
  my_animal.vtbl->speak(&my_animal);
  my_animal.vtbl->move(&my_animal);

  return 0;
}
